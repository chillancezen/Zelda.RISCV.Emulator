all:
	@make --no-print-directory -C rom
	@make --no-print-directory -C guest
	@make --no-print-directory -C vmm

clean:
	@make --no-print-directory clean -C rom
	@make --no-print-directory clean -C guest
	@make --no-print-directory clean -C vmm
run:all
	@./vmm/vmx test.vm.ini


# to start qemmu and make it blocking, SET QEMU_STOP=-S environmental variable
guest_qemu:
	@qemu-system-riscv32 -monitor null -nographic -machine virt -m 128M  -kernel guest/guest.rv32.elf  -gdb tcp::5070  ${QEMU_STOP}
guest_gdb:
	@riscv32-unknown-elf-gdb guest/guest.rv32.elf --eval-command "target remote 0:5070"
