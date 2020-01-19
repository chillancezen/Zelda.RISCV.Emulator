/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <fdt.h>
#include <string.h>
#include <malloc.h> 
#include<fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vm.h>

void
fdt_build_init(struct fdt_build_blob * blob, int buffer_size,
               int string_buffer_size)
{
    memset(blob, 0, sizeof(struct fdt_build_blob));
    blob->phandle_generator = 1;
    blob->buffer_size = buffer_size;
    blob->buffer = malloc(buffer_size);
    ASSERT(blob->buffer);
    memset(blob->buffer, 0, buffer_size);

    blob->string_buffer_size = string_buffer_size;
    blob->string_buffer = malloc(string_buffer_size);
    ASSERT(blob->string_buffer);
    memset(blob->string_buffer, 0, string_buffer_size);

    struct fdt_header * fdt = (struct fdt_header *)blob->buffer;
    fdt->magic = BIG_ENDIAN32(FDT_MAGIC);
    fdt->version = BIG_ENDIAN32(17);
    fdt->last_comp_version = BIG_ENDIAN32(2);
    blob->buffer_iptr = sizeof(struct fdt_header);
    blob->buffer_iptr = ALIGN32(blob->buffer_iptr, 4);

    // insert an empty memory reservation block right after the fdt header
    fdt->off_mem_rsvmap = BIG_ENDIAN32(blob->buffer_iptr);
    blob->buffer_iptr += 16;
    blob->buffer_iptr = ALIGN32(blob->buffer_iptr, 4);

    // begin the structures
    fdt->off_dt_struct = BIG_ENDIAN32(blob->buffer_iptr);
}


void
fdt_begin_node(struct fdt_build_blob * blob, const char * node_name)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4 + strlen(node_name) + 1;
    room_needed = ALIGN32(room_needed, 4);
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_BEGIN_NODE);
    memcpy(blob->buffer + blob->buffer_iptr + 4, node_name, strlen(node_name));
    blob->buffer_iptr += room_needed;
}

void
fdt_prop(struct fdt_build_blob * blob, const char * prop_name,
         uint32_t prop_len, void * prop_value)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4 + sizeof(struct fdt_property) + prop_len;
    room_needed = ALIGN32(room_needed, 4);
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_PROP);
    struct fdt_property * fdt_prop =
        (struct fdt_property *)(blob->buffer + blob->buffer_iptr + 4);
    fdt_prop->len = BIG_ENDIAN32(prop_len);
    fdt_prop->nameoff = BIG_ENDIAN32(blob->string_buffer_iptr);
    memcpy(blob->buffer + blob->buffer_iptr + 4 + sizeof(struct fdt_property),
           prop_value, prop_len);
    blob->buffer_iptr += room_needed;
    // make room for the property name string
    uint32_t name_size = strlen(prop_name) + 1;
    ASSERT(name_size < (blob->string_buffer_size - blob->string_buffer_iptr));
    memcpy(blob->string_buffer + blob->string_buffer_iptr,
           prop_name, name_size);
    blob->string_buffer_iptr += name_size;
}

void
fdt_end_node(struct fdt_build_blob * blob)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4;
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_END_NODE);
    blob->buffer_iptr += room_needed;
}

void
fdt_nop(struct fdt_build_blob * blob)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4;
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_NOP);
    blob->buffer_iptr += room_needed;
}


void
fdt_end(struct fdt_build_blob * blob)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4;
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_END);
    blob->buffer_iptr += room_needed;
    // there should be NO any tokens.
    // merge two buffers and fill other fields
    struct fdt_header * fdt = (struct fdt_header *)blob->buffer;
    fdt->off_dt_strings = BIG_ENDIAN32(blob->buffer_iptr);
    fdt->size_dt_strings = BIG_ENDIAN32(blob->string_buffer_iptr);
    uint32_t size_struct = blob->buffer_iptr - LITTLE_ENDIAN32(fdt->off_dt_struct);
    fdt->size_dt_struct = BIG_ENDIAN32(size_struct);
    uint32_t size_total = blob->buffer_iptr + blob->string_buffer_iptr;
    fdt->totalsize = BIG_ENDIAN32(size_total);
    // copy string buffer
    room_available = blob->buffer_size - blob->buffer_iptr;
    room_needed = blob->string_buffer_iptr;
    ASSERT(room_needed <= room_available);
    memcpy(blob->buffer + blob->buffer_iptr, blob->string_buffer,
           blob->string_buffer_iptr);
    blob->buffer_iptr += blob->string_buffer_iptr;
}

void
fdt_blob_destroy(struct fdt_build_blob * blob)
{
    if (blob->buffer) {
        free(blob->buffer);
        blob->buffer = NULL;
    }

    if (blob->string_buffer) {
        free(blob->string_buffer);
        blob->string_buffer = NULL;
    }
    memset(blob, 0, sizeof(struct fdt_build_blob));
}

static void
dump_device_tree_to_file(struct fdt_build_blob * blob,
                         const char * dtb_path)
{
    int fd = open(dtb_path, O_CREAT | O_WRONLY);

    int nr_left = blob->buffer_iptr;
    int iptr = 0;

    while (nr_left > 0) {
        int rc = write(fd, blob->buffer + iptr, nr_left);
        if (rc <= 0) {
            break;
        }

        nr_left -= rc;
        iptr += rc;
    }
    close(fd);
}

static void
build_memory_fdt_node(struct fdt_build_blob *blob)
{
    struct virtual_machine * vm =
        CONTAINER_OF(blob, struct virtual_machine, fdt);
    char name[64];
    sprintf(name, "memory@%x", (uint32_t)vm->main_mem_base);
    fdt_begin_node(blob, name);
    fdt_prop(blob, "device_type", strlen("memory") + 1, "memory");
    uint32_t regs[4] = {
        0, BIG_ENDIAN32(vm->main_mem_base),
        0, BIG_ENDIAN32(vm->main_mem_size)
    };
    fdt_prop(blob, "reg", 16, regs);
    fdt_end_node(blob);
}

static void
build_bootrom_fdt_node(struct fdt_build_blob * blob)
{
    struct virtual_machine * vm =
        CONTAINER_OF(blob, struct virtual_machine, fdt);
    const char * rom_start_string = ini_get(vm->ini_config, "rom", "rom_start");
    const char * rom_size_string = ini_get(vm->ini_config, "rom", "rom_size");
    ASSERT(rom_start_string);
    ASSERT(rom_size_string);
    uint32_t rom_start = strtol(rom_start_string, NULL, 16);
    uint32_t rom_size = strtol(rom_size_string, NULL, 16);
    char name[64];
    sprintf(name, "bootrom@%x", rom_start);
    fdt_begin_node(blob, name);
    fdt_prop(blob, "device_type", strlen("bootrom") + 1, "bootrom");
    fdt_prop(blob, "compatible", strlen("zelda 16MB flash") + 1,
             "zelda 16MB flash");
    uint32_t regs[4] = {
        0, BIG_ENDIAN32(rom_start),
        0, BIG_ENDIAN32(rom_size)
    };
    fdt_prop(blob, "reg", 16, regs);
    fdt_end_node(blob);
}

static void
build_cpu_fdt_node(struct fdt_build_blob * blob)
{
    struct hart * hart_ptr;
    int idx = 0;
    struct virtual_machine * vm =
        CONTAINER_OF(blob, struct virtual_machine, fdt);
    char name[64];
    fdt_begin_node(blob, "cpus");
    uint32_t address_cells = BIG_ENDIAN32(1);
    uint32_t size_cells = 0;
    uint32_t timebase_frequency = BIG_ENDIAN32(0x989680);
    fdt_prop(blob, "#address-cells", 4, &address_cells);
    fdt_prop(blob, "#size_cells", 4, &size_cells);
    fdt_prop(blob, "timebase-frequency", 4, &timebase_frequency);
    fdt_begin_node(blob, "cpu-map");
    fdt_begin_node(blob, "cluster0");
    for (idx = 0; idx < vm->nr_harts; idx++) {
        hart_ptr = hart_by_id(vm, idx);
        sprintf(name, "core%d", hart_ptr->hart_id);
        fdt_begin_node(blob, name);
        blob->harts_phandles[idx] = generate_phandle(blob);
        uint32_t cpu_phandle = BIG_ENDIAN32(blob->harts_phandles[idx]);
        fdt_prop(blob, "cpu", 4, &cpu_phandle);
        fdt_end_node(blob);
    }
    fdt_end_node(blob);
    fdt_end_node(blob);
    
    for (idx = 0; idx < vm->nr_harts; idx++) {
        hart_ptr = hart_by_id(vm, idx);
        sprintf(name, "cpu@%d", hart_ptr->hart_id);
        fdt_begin_node(blob, name);
        uint32_t cpu_phandle = BIG_ENDIAN32(blob->harts_phandles[idx]);
        fdt_prop(blob, "phandle", 4, &cpu_phandle);
        fdt_prop(blob, "device_type", strlen("cpu") + 1, "cpu");
        fdt_prop(blob, "status", strlen("okay") + 1, "okay");
        fdt_prop(blob, "compatible", strlen("riscv") + 1, "riscv");
        fdt_prop(blob, "riscv,isa", strlen("rv32ima") + 1, "rv32ima");
        fdt_prop(blob, "mmu-type", strlen("riscv,sv32") + 1, "riscv,sv32");
        uint32_t hartid = BIG_ENDIAN32(hart_ptr->hart_id);
        fdt_prop(blob, "reg", 4, &hartid);
        fdt_begin_node(blob, "interrupt-controller");
        uint32_t interrupt_cells = BIG_ENDIAN32(1);
        fdt_prop(blob, "#interrupt-cells", 4, &interrupt_cells);
        fdt_prop(blob, "interrupt-controller", 0, NULL);
        blob->hart_interrupt_controllers_phandles[idx] = generate_phandle(blob);
        uint32_t interrupt_controller_phandle =
            BIG_ENDIAN32(blob->hart_interrupt_controllers_phandles[idx]);
        fdt_prop(blob, "phandle", 4, &interrupt_controller_phandle);
        fdt_prop(blob, "compatible", strlen("riscv,cpu-intc") + 1,
                 "riscv,cpu-intc");
        fdt_end_node(blob);
        fdt_end_node(blob);
    }
    fdt_end_node(blob);
}

static void
build_soc_fdt_node(struct fdt_build_blob * blob)
{
    struct virtual_machine * vm =
        CONTAINER_OF(blob, struct virtual_machine, fdt);
    fdt_begin_node(blob, "soc");
    uint32_t address_cells = BIG_ENDIAN32(2);
    uint32_t size_cells = BIG_ENDIAN32(2);
    fdt_prop(blob, "#address-cells", 4, &address_cells);
    fdt_prop(blob, "#size-cells", 4, &size_cells);
    fdt_prop(blob, "compatible", strlen("simple-bus") + 1, "simple-bus");
    fdt_prop(blob, "ranges", 0, NULL);
    
    // define Core Local Interrupt Controller CLINT which generates and delivers
    // machine timer interrupt and supervisor timer interrupt
    // interrupt 0x3 and 0x7.
    const char * clint_base_string = ini_get(vm->ini_config, "cpu", "clint_base");
    const char * clint_size_string = ini_get(vm->ini_config, "cpu", "clint_size");
    ASSERT(clint_base_string);
    ASSERT(clint_size_string);
    uint32_t clint_base = strtol(clint_base_string, NULL, 16);
    uint32_t clint_size = strtol(clint_size_string, NULL, 16);
    char name[64];
    sprintf(name, "clint@%x", clint_base);
    fdt_begin_node(blob, name);
    uint32_t regs[4] = {
        0x0, BIG_ENDIAN32(clint_base),
        0x0, BIG_ENDIAN32(clint_size)
    };
    fdt_prop(blob, "reg", 16, regs);
    fdt_prop(blob, "compatible", strlen("riscv,clint0") + 1, "riscv,clint0");
    uint32_t interrupt_extended[4 * MAX_NR_HARTS];
    int idx = 0;
    for (; idx < vm->nr_harts; idx++) {
        interrupt_extended[idx * 4 + 0] =
            BIG_ENDIAN32(blob->hart_interrupt_controllers_phandles[idx]);
        interrupt_extended[idx * 4 + 1] = BIG_ENDIAN32(0x3);
        interrupt_extended[idx * 4 + 2] =
            BIG_ENDIAN32(blob->hart_interrupt_controllers_phandles[idx]);
        interrupt_extended[idx * 4 + 1] = BIG_ENDIAN32(0x7);
    }
    fdt_prop(blob, "interrupts-extended", 16 * vm->nr_harts, interrupt_extended);
    fdt_end_node(blob);
    fdt_end_node(blob);
}

void
fdt_init(struct fdt_build_blob * blob)
{
    fdt_build_init(blob, 4096, 512);
    fdt_begin_node(blob, ""); // the root node
    uint32_t address_cells = BIG_ENDIAN32(2);
    uint32_t size_cells = BIG_ENDIAN32(2);
    fdt_prop(blob, "#address-cells", 4, &address_cells);
    fdt_prop(blob, "#size-cells", 4, &size_cells);
    fdt_prop(blob, "compatible", strlen("riscv-virtio") + 1, "riscv-virtio");
    fdt_prop(blob, "model", strlen("riscv-virtio,qemu") + 1, "riscv-virtio,qemu");

    build_bootrom_fdt_node(blob);
    build_uart_fdt_node(blob);
    build_memory_fdt_node(blob);
    build_cpu_fdt_node(blob);
    build_soc_fdt_node(blob);

    fdt_end_node(blob);
    fdt_end(blob);

    struct virtual_machine * vm = CONTAINER_OF(blob, struct virtual_machine, fdt);
    // dump dtb to file if we want
    const char * dump_dtb = ini_get(vm->ini_config, "misc", "dump_dtb");
    if (dump_dtb) {
        dump_device_tree_to_file(blob, dump_dtb);
    }
    
    // Load the dtb into the very beginning of rom
    ASSERT(blob->buffer_iptr < (4096 * 3));
    ASSERT(memcpy(vm->bootrom_host_base, blob->buffer, blob->buffer_iptr));
}

void
scan_fdt_tokens(uint8_t * dtb, int size)
{
    struct fdt_header * fdt = (struct fdt_header *)dtb;
    uint8_t * fdt_struct_base = dtb + LITTLE_ENDIAN32(fdt->off_dt_struct);
    uint8_t * ptr = fdt_struct_base;
    char * node_name = NULL;
    char * node_prop_name = NULL;
    int node_prop_length = NULL;
    void * node_prop_value = NULL;
    int to_stop = 0;
    while(!to_stop) {
        uint32_t token = LITTLE_ENDIAN32(*(uint32_t *)ptr);
        ptr += 4;
        switch(token)
        {
            case FDT_BEGIN_NODE:
                node_name = (char *)ptr;
                ptr += strlen(node_name) + 1;
                ptr = ALIGNADDR(ptr, 4);
                break;
            case FDT_PROP:
                {
                    struct fdt_property *prop = (struct fdt_property *)ptr;
                    node_prop_length = LITTLE_ENDIAN32(prop->len);
                    node_prop_name = (char *)(dtb + LITTLE_ENDIAN32(fdt->off_dt_strings) +
                                              LITTLE_ENDIAN32(prop->nameoff));
                    ptr += sizeof(struct fdt_property);
                    node_prop_value = ptr;
                    ptr += node_prop_length;
                    ptr = ALIGNADDR(ptr, 4);
                    log_debug("%s.%s length:%d <@%p>\n", node_name,
                              node_prop_name, node_prop_length,
                              node_prop_value);
                }
                break;
            case FDT_NOP:
                break;
            case FDT_END_NODE:
                break;
            case FDT_END:
                to_stop = 1;
                break;
            default:
                __not_reach();
                break;
        }
    }
}

void
dump_fdt(uint8_t * dtb, int size)
{
    struct fdt_header * fdt = (struct fdt_header *)dtb;
    log_debug("fdt.magicword:0x%x\n", LITTLE_ENDIAN32(fdt->magic));
    log_debug("fdt.version:%d\n", LITTLE_ENDIAN32(fdt->version));
    log_debug("fdt.last_comp_version:%d\n", LITTLE_ENDIAN32(fdt->last_comp_version));
    log_debug("fdt.off_dt_strings:%d\n", LITTLE_ENDIAN32(fdt->off_dt_strings));
    log_debug("fdt.off_dt_struct:%d\n", LITTLE_ENDIAN32(fdt->off_dt_struct));
    log_debug("fdt.off_mem_rsvmap:%d\n", LITTLE_ENDIAN32(fdt->off_mem_rsvmap));
    log_debug("fdt.size_dt_strings:%d\n", LITTLE_ENDIAN32(fdt->size_dt_strings));
    log_debug("fdt.size_dt_struct:%d\n", LITTLE_ENDIAN32(fdt->size_dt_struct));
    log_debug("fdt.totalsize:%d\n", LITTLE_ENDIAN32(fdt->totalsize));
    scan_fdt_tokens(dtb, size);
}

#define FDT_TEST 0
#if FDT_TEST
    #include <stdio.h>
    #include<fcntl.h>
    #include <sys/stat.h>
    #include <unistd.h>

__attribute__((constructor)) static void
fdt_test(void)
{
    struct fdt_build_blob blob;
    fdt_build_init(&blob, 8192, 512);
    fdt_begin_node(&blob, "");
    
    uint32_t address_cells = BIG_ENDIAN32(2);
    uint32_t size_cells = BIG_ENDIAN32(2);
    fdt_prop(&blob, "#address-cells", 4, &address_cells);
    fdt_prop(&blob, "#size-cells", 4, &size_cells);
    fdt_prop(&blob, "compatible", strlen("riscv-virtio") + 1, "riscv-virtio");
    fdt_prop(&blob, "model", strlen("riscv-virtio,qemu") + 1, "riscv-virtio,qemu");
    fdt_begin_node(&blob, "uart@10000000");
    fdt_prop(&blob, "compatible", strlen("ns16550a") + 1, "ns16550a");
    fdt_end_node(&blob);
    fdt_end_node(&blob);
    fdt_end(&blob);
    dump_fdt(blob.buffer, blob.buffer_iptr);
    int fd0 = open("../diskimage/zelda.dtb", O_CREAT | O_WRONLY);
    write(fd0, blob.buffer, blob.buffer_iptr);
    return;

    uint8_t * dtb_base = malloc(8192);
    ASSERT(dtb_base);
    log_debug("dtb_base: %p\n", dtb_base);
    int fd = open("../diskimage/riscv32-virt.dtb", O_RDONLY);
    int nr_left = 8192;
    int dtb_length = 0;
    
    while(nr_left > 0) {
        int rc = read(fd, dtb_base + dtb_length, nr_left);
        if (rc <= 0) {
            break;
        }
        nr_left -= rc;
        dtb_length += rc;   
    }
    ASSERT(dtb_length < nr_left);
    dump_fdt(dtb_base, dtb_length);
    close(fd);
}
#endif
