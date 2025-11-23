# Kernel module
obj-m := driver.o

# Path to the kernel source directory
KERNEL_SRC := /lib/modules/$(shell uname -r)/build

# Current working directory
PWD := $(shell pwd)

# Default target: build the kernel module and user program
all: modules user

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

user:
	gcc -static $(CFLAGS) -o launch user.c

# Load module: remove old one first, then insert new one
load: driver.ko
	@sudo rmmod driver 2>/dev/null || true
	sudo insmod driver.ko

# Create /dev/nuke0 and /dev/nuke1 automatically and set permissions
dev:
	@major=$$(awk '$$2=="nuke"{m=$$1} END{print m}' /proc/devices); \
	if [ -z "$$major" ]; then \
		echo "ERROR: cannot find 'nuke' in /proc/devices. Is the module loaded?"; \
		exit 1; \
	fi; \
	for minor in 0 1; do \
		node=/dev/nuke$$minor; \
		if [ -e $$node ] && [ ! -c $$node ]; then \
			echo "Removing non-char $$node"; \
			sudo rm -f $$node; \
		fi; \
		if [ ! -e $$node ]; then \
			echo "Creating $$node (c $$major,$$minor)"; \
			sudo mknod $$node c $$major $$minor; \
		fi; \
		sudo chmod 666 $$node; \
	done; \
	ls -l /dev/nuke*

# Test: write secret and show kernel log
test:
	echo "COP4610" > /dev/nuke0
	sudo dmesg | tail -n 40

# Unload the module
unload:
	sudo rmmod driver

# One-shot: build, load, create devices, test
run: all load dev test

# Clean the build artifacts
clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
	$(RM) launch

.PHONY: all modules user load dev test unload run clean
