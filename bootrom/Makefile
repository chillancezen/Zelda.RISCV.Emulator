item = .
C_FILES = $(foreach item,.,$(wildcard $(item)/*.c))
C_OBJS = $(patsubst %.c,%.o,$(C_FILES))

AS_FILES = $(foreach item, .,$(wildcard $(item)/*.S))
AS_OBJS = $(patsubst %.S,%.o,$(AS_FILES))

# The emulator implements rv32ima only: no floating point and no compressed
# instructions. Building the firmware any wider would emit opcodes the vmm
# cannot translate.
CCPARAMS = -O2 -g3 -ffreestanding -march=rv32ima -mabi=ilp32 -mcmodel=medany
CCPARAMS += -nostdlib -fno-builtin -fno-exceptions -fno-stack-protector
# Nothing sets up gp for us, so keep everything out of the small data sections
# and off the linker relaxation path that would rewrite accesses to use it.
CCPARAMS += -mno-relax -msmall-data-limit=0
CCPARAMS += -Werror -Wall -Wstrict-prototypes

# Run the instruction level self tests at boot: they exercise the emulator's
# translator rather than the guest, so they are off by default.
ifeq ($(FW_SELFTEST),1)
CCPARAMS += -DFW_SELFTEST
endif
LDPARAMS = -m elf32lriscv -static --no-relax

ROM_ELF = bootrom.rv32.elf
ROM_IMG = bootrom.rv32.img
ROM_MAP = bootrom.rv32.map

# Override with e.g. CROSS_COMPILE=riscv32-unknown-linux-gnu- if you have a
# native rv32 toolchain; a riscv64 cross compiler targets rv32 just fine.
CROSS_COMPILE ?= riscv64-linux-gnu-

all: $(ROM_IMG)

%.o: %.S
	@echo "[AS] $<"
	@$(CROSS_COMPILE)gcc $(CCPARAMS) -I. -o $@ -c $<

%.o: %.c
	@echo "[CC] $<"
	@$(CROSS_COMPILE)gcc $(CCPARAMS) -I. -o $@ -c $<

$(ROM_IMG):$(ROM_ELF)
	@echo "[CT] $@"
	@$(CROSS_COMPILE)objcopy -O binary $< $@

$(ROM_ELF):$(AS_OBJS) $(C_OBJS)
	@echo "[LD] $@"
	@$(CROSS_COMPILE)ld $(LDPARAMS) -Map=$(ROM_MAP) -T linker.ld -o $(ROM_ELF) $(AS_OBJS) $(C_OBJS)

clean:
	@echo "[Cleaning] bootrom"
	@rm -f *.o $(ROM_ELF) $(ROM_IMG) $(ROM_MAP)
	@rm -f *.o.plain

.PHONY: all clean
