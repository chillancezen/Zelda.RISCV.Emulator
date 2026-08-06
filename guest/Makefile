# Builds a guest for the emulator: an rv32 Linux kernel with a small
# userspace built into it as an initramfs.
#
# This is deliberately not part of the top level build. It downloads and
# compiles a kernel, which takes minutes and a couple of gigabytes, while the
# emulator itself builds in seconds.
#
#   make -C guest          build the kernel image
#   make -C guest clean    remove the build products but keep the source tree
#   make -C guest mrproper remove everything including the source tree

KERNEL_VERSION ?= 6.1.75
KERNEL_DIR     := linux-$(KERNEL_VERSION)
KERNEL_TARBALL := $(KERNEL_DIR).tar.xz
KERNEL_URL     := https://cdn.kernel.org/pub/linux/kernel/v6.x/$(KERNEL_TARBALL)

# A riscv64 cross compiler builds rv32 fine; point this elsewhere if you have
# a native rv32 toolchain.
CROSS_COMPILE  ?= riscv64-linux-gnu-
JOBS           ?= $(shell nproc)

INITRAMFS_DIR  := initramfs
IMAGE          := Image

# The init is freestanding and talks to the kernel through raw system calls,
# so no rv32 libc is needed to build a working userspace.
INIT_CFLAGS = -march=rv32ima -mabi=ilp32 -Os -static -nostdlib -ffreestanding
INIT_CFLAGS += -fno-stack-protector -Wall

all: $(IMAGE)

$(KERNEL_TARBALL):
	@echo "[GET] $(KERNEL_URL)"
	@curl -fsSL --retry 3 -o $@ $(KERNEL_URL)

$(KERNEL_DIR)/Makefile: $(KERNEL_TARBALL)
	@echo "[TAR] $(KERNEL_TARBALL)"
	@tar xf $(KERNEL_TARBALL)

$(INITRAMFS_DIR)/init: init.c
	@echo "[CC] $<"
	@mkdir -p $(INITRAMFS_DIR)/proc $(INITRAMFS_DIR)/sys $(INITRAMFS_DIR)/dev
	@$(CROSS_COMPILE)gcc $(INIT_CFLAGS) -o $@ $<

# The kernel's own config machinery decides everything else; zelda.config only
# states where this platform differs from the generic rv32 defconfig.
$(KERNEL_DIR)/.config: $(KERNEL_DIR)/Makefile zelda.config
	@echo "[CFG] $(KERNEL_DIR)"
	@$(MAKE) -C $(KERNEL_DIR) ARCH=riscv CROSS_COMPILE=$(CROSS_COMPILE) \
		rv32_defconfig > /dev/null
	@$(KERNEL_DIR)/scripts/kconfig/merge_config.sh -m \
		-O $(KERNEL_DIR) $(KERNEL_DIR)/.config zelda.config > /dev/null
	@$(MAKE) -C $(KERNEL_DIR) ARCH=riscv CROSS_COMPILE=$(CROSS_COMPILE) \
		olddefconfig > /dev/null

$(IMAGE): $(KERNEL_DIR)/.config $(INITRAMFS_DIR)/init
	@echo "[MAKE] linux $(KERNEL_VERSION) rv32ima"
	@$(MAKE) -C $(KERNEL_DIR) ARCH=riscv CROSS_COMPILE=$(CROSS_COMPILE) \
		-j$(JOBS) Image
	@cp $(KERNEL_DIR)/arch/riscv/boot/$(IMAGE) $@
	@echo "[OK] $@ is ready"

clean:
	@echo "[Cleaning] guest"
	@rm -rf $(INITRAMFS_DIR) $(IMAGE)
	@if [ -d $(KERNEL_DIR) ]; then \
		$(MAKE) -C $(KERNEL_DIR) ARCH=riscv CROSS_COMPILE=$(CROSS_COMPILE) \
			clean > /dev/null; \
	fi

mrproper:
	@echo "[Cleaning] guest (including kernel source)"
	@rm -rf $(INITRAMFS_DIR) $(IMAGE) $(KERNEL_DIR) $(KERNEL_TARBALL)

.PHONY: all clean mrproper
