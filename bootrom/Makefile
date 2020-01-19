item = .
C_FILES = $(foreach item,.,$(wildcard $(item)/*.c))
C_OBJS = $(patsubst %.c,%.o,$(C_FILES))

AS_FILES = $(foreach item, .,$(wildcard $(item)/*.S))
AS_OBJS = $(patsubst %.S,%.o,$(AS_FILES))

ASPARAMS = -m64 -mno-red-zone
#CCPARAMS = -O0 -g3 -ffreestanding -mno-fdiv -mno-div -march=rv32ima -mabi=ilp32 -nostdlib -fno-builtin -fno-exceptions -Werror -Wall -Wstrict-prototypes
CCPARAMS = -O0 -g3 -ffreestanding -mno-fdiv -march=rv32imafd -mabi=ilp32 -nostdlib -fno-builtin -fno-exceptions -Werror -Wall -Wstrict-prototypes
LDPARAMS = -m elf32lriscv -static

ROM_ELF = bootrom.rv32.elf
ROM_IMG = bootrom.rv32.img
ROM_MAP = bootrom.rv32.map

CROSS_COMPILE = riscv32-unknown-elf-
%.o: %.S
	@echo "[AS] $<"
	@$(CROSS_COMPILE)gcc $(CCPARAMS) -I. -o $@ -c $<

%.o: %.c
	@echo "[CC] $<"
	@$(CROSS_COMPILE)gcc $(CCPARAMS) -I. -g -Wa,-adhls -o $@ -c $< > $@.plain

$(ROM_IMG):$(ROM_ELF)
	@echo "[CT] $@"
	@cp $< $@
	@$(CROSS_COMPILE)objcopy  $@ -O binary

$(ROM_ELF):$(AS_OBJS) $(C_OBJS)
	@echo "[LD] $@"
	@$(CROSS_COMPILE)ld $(LDPARAMS) -Map=$(ROM_MAP) -T linker.ld -o $(ROM_ELF) $(AS_OBJS) $(C_OBJS)

clean:
	@echo "[Cleaning] bootrom"
	@rm -f *.o $(ROM_ELF) $(ROM_IMG) $(ROM_MAP)
	@rm -f *.o.plain

