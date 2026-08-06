# The emulator and its machine mode firmware. Both build in seconds.
all:
	@make --no-print-directory -C bootrom
	@make --no-print-directory -C vmm

clean:
	@make --no-print-directory clean -C bootrom
	@make --no-print-directory clean -C vmm

# The guest kernel is a separate target on purpose: it downloads and compiles
# Linux, which takes minutes, and it rarely needs rebuilding.
guest:
	@make --no-print-directory -C guest

run:all
	@./vmm/vmx test.vm.ini

.PHONY: all clean guest run
