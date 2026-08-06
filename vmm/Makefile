item = .
C_FILES = $(foreach item,.,$(wildcard $(item)/*.c))
C_OBJS = $(patsubst %.c,%.o,$(C_FILES))

AS_FILES = $(foreach item, .,$(wildcard $(item)/*.S))
AS_OBJS = $(patsubst %.S,%.o,$(AS_FILES))


# Use gnu11. . .
# The translated code blocks embed absolute addresses of vmm callbacks, so the
# vmm has to be built and linked non-PIE.
CCPARAMS = -m64 -std=gnu11 -O0 -g3 -Werror -Wall -Wstrict-prototypes
CCPARAMS += -fno-pie -fno-stack-protector
CCPARAMS += -include config.h
# Header dependencies, so editing a header actually rebuilds what includes it.
CCPARAMS += -MMD -MP

LDPARAMS += -no-pie

GUEST_ELF = vmx
GUEST_MAP = vmx.map

# ../bootrom carries the handoff block layout the firmware and the vmm share.
INCLUDES = -I. -I../bootrom

%.o: %.S
	@echo "[AS] $<"
	@$(CC) $(CCPARAMS) $(INCLUDES) -o $@ -c $<

%.o: %.c
	@echo "[CC] $<"
	@$(CC) $(CCPARAMS) $(INCLUDES) -o $@ -c $<


$(GUEST_ELF):$(AS_OBJS) $(C_OBJS)
	@echo "[LD] $@"
	@$(CC) $(LDPARAMS) -Wl,-Map=$(GUEST_MAP) -lm -o $(GUEST_ELF) $(AS_OBJS) $(C_OBJS)

-include $(C_OBJS:.o=.d) $(AS_OBJS:.o=.d)

clean:
	@echo "[Cleaning] $(GUEST_ELF)"
	@rm -f *.o *.d $(GUEST_ELF) $(GUEST_IMG) $(GUEST_MAP)
run:$(GUEST_ELF)
	@./$(GUEST_ELF) ../rom/rom.rv32.img

debug:$(GUEST_ELF)
	@gdb ./$(GUEST_ELF) ../rom/rom.rv32.img
