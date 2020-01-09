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
