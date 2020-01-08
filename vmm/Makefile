item = .
C_FILES = $(foreach item,.,$(wildcard $(item)/*.c))
C_OBJS = $(patsubst %.c,%.o,$(C_FILES))

AS_FILES = $(foreach item, .,$(wildcard $(item)/*.S))
AS_OBJS = $(patsubst %.S,%.o,$(AS_FILES))


# Use gnu11. . .
CCPARAMS = -m64 -std=gnu11 -O0 -g3 -Werror -Wall -Wstrict-prototypes
CCPARAMS += -include config.h

GUEST_ELF = vmx
GUEST_MAP = vmx.map

%.o: %.S
	@echo "[AS] $<"
	@gcc $(CCPARAMS) -I. -o $@ -c $<

%.o: %.c
	@echo "[CC] $<"
	@gcc $(CCPARAMS) -I. -o $@ -c $<


$(GUEST_ELF):$(AS_OBJS) $(C_OBJS)
	@echo "[LD] $@"
	@gcc $(LDPARAMS) -Wl,-Map=$(GUEST_MAP) -lm -o $(GUEST_ELF) $(AS_OBJS) $(C_OBJS)

clean:
	@echo "[Cleaning] $(GUEST_ELF)"
	@rm -f *.o $(GUEST_ELF) $(GUEST_IMG) $(GUEST_MAP)
run:$(GUEST_ELF)
	@./$(GUEST_ELF) ../rom/rom.rv32.img

debug:$(GUEST_ELF)
	@gdb ./$(GUEST_ELF) ../rom/rom.rv32.img
