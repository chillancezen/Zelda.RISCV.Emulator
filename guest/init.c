/*
 * A freestanding rv32 PID 1 with a tiny built-in shell.
 *
 * This exists so the emulator can be exercised end to end without needing a
 * riscv32 libc.  It is used as the initramfs /init when busybox is not
 * available, and is small enough to stay readable.
 */

#define SYS_read        63
#define SYS_write       64
#define SYS_openat      56
#define SYS_close       57
#define SYS_exit_group  94
#define SYS_reboot     142
#define SYS_mount       40
#define SYS_uname      160
#define SYS_getdents64  61

#define AT_FDCWD (-100)

static inline long
syscall3(long n, long a, long b, long c)
{
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0") = a;
    register long a1 __asm__("a1") = b;
    register long a2 __asm__("a2") = c;
    __asm__ volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1), "r"(a2) : "memory");
    return a0;
}

static inline long
syscall4(long n, long a, long b, long c, long d)
{
    register long a7 __asm__("a7") = n;
    register long a0 __asm__("a0") = a;
    register long a1 __asm__("a1") = b;
    register long a2 __asm__("a2") = c;
    register long a3 __asm__("a3") = d;
    __asm__ volatile("ecall"
                     : "+r"(a0)
                     : "r"(a7), "r"(a1), "r"(a2), "r"(a3)
                     : "memory");
    return a0;
}

static unsigned
slen(const char * s)
{
    unsigned n = 0;
    while (s[n]) {
        n++;
    }
    return n;
}

static void
put(const char * s)
{
    syscall3(SYS_write, 1, (long)s, slen(s));
}

static int
seq(const char * a, const char * b)
{
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

static void
cat(const char * path)
{
    char buf[512];
    long fd = syscall4(SYS_openat, AT_FDCWD, (long)path, 0 /*O_RDONLY*/, 0);
    if (fd < 0) {
        put("cannot open ");
        put(path);
        put("\n");
        return;
    }
    for (;;) {
        long n = syscall3(SYS_read, fd, (long)buf, sizeof(buf));
        if (n <= 0) {
            break;
        }
        syscall3(SYS_write, 1, (long)buf, n);
    }
    syscall3(SYS_close, fd, 0, 0);
}

struct utsname_rv {
    char sysname[65], nodename[65], release[65], version[65], machine[65],
        domainname[65];
};

static void
do_uname(void)
{
    struct utsname_rv u;
    if (syscall3(SYS_uname, (long)&u, 0, 0) < 0) {
        return;
    }
    put(u.sysname);
    put(" ");
    put(u.release);
    put(" ");
    put(u.machine);
    put("\n");
}

static void
help(void)
{
    put("builtins: help uname meminfo cpuinfo version ls poweroff\n");
}

static void
ls(const char * path)
{
    char buf[1024];
    long fd = syscall4(SYS_openat, AT_FDCWD, (long)path, 0x10000 /*O_DIRECTORY*/, 0);
    if (fd < 0) {
        put("cannot open ");
        put(path);
        put("\n");
        return;
    }
    for (;;) {
        long n = syscall3(SYS_getdents64, fd, (long)buf, sizeof(buf));
        if (n <= 0) {
            break;
        }
        long off = 0;
        while (off < n) {
            /* struct linux_dirent64: u64 ino, s64 off, u16 reclen, u8 type, name */
            unsigned short reclen = *(unsigned short *)(buf + off + 16);
            put(buf + off + 19);
            put("  ");
            off += reclen;
        }
    }
    put("\n");
    syscall3(SYS_close, fd, 0, 0);
}

void
_start(void)
{
    char line[128];

    syscall4(SYS_mount, (long)"proc", (long)"/proc", (long)"proc", 0);
    syscall4(SYS_mount, (long)"sysfs", (long)"/sys", (long)"sysfs", 0);

    put("\n");
    put("  Zelda RISC-V emulator -- rv32ima Linux userspace is alive.\n");
    put("  Type 'help' for the builtins, 'poweroff' to stop the machine.\n\n");

    for (;;) {
        unsigned n = 0;
        put("zelda:/# ");
        /*
         * The console is in the kernel's default line mode, so it does the
         * echoing and the line editing for us; doing either here as well
         * would show every keystroke twice.
         */
        for (;;) {
            char c;
            long r = syscall3(SYS_read, 0, (long)&c, 1);
            if (r == 0) {
                /* Console closed. As pid 1 we cannot exit, so idle. */
                struct { long sec; long nsec; } delay = {1, 0};
                syscall3(101 /* nanosleep */, (long)&delay, 0, 0);
                continue;
            }
            if (r < 0) {
                continue;
            }
            if (c == '\r' || c == '\n') {
                break;
            }
            if (n + 1 < sizeof(line)) {
                line[n++] = c;
            }
        }
        line[n] = 0;

        if (!n) {
            continue;
        } else if (seq(line, "help")) {
            help();
        } else if (seq(line, "uname")) {
            do_uname();
        } else if (seq(line, "meminfo")) {
            cat("/proc/meminfo");
        } else if (seq(line, "cpuinfo")) {
            cat("/proc/cpuinfo");
        } else if (seq(line, "version")) {
            cat("/proc/version");
        } else if (seq(line, "ls")) {
            ls("/");
        } else if (seq(line, "poweroff")) {
            put("powering off\n");
            /* LINUX_REBOOT_CMD_POWER_OFF */
            syscall4(SYS_reboot, 0xfee1deadu, 672274793, 0x4321fedc, 0);
        } else {
            put("unknown command: ");
            put(line);
            put("\n");
        }
    }
    syscall3(SYS_exit_group, 0, 0, 0);
}
