# Makefile
obj-m := driver.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

# Detect headers
HAVE_KDIR := $(wildcard $(KDIR)/Makefile)

all: info user $(if $(HAVE_KDIR),kmod,)

info:
	@echo "Kernel: $(shell uname -r)"
	@if [ -z "$(HAVE_KDIR)" ]; then \
	  echo ">>> Kernel headers NOT found at $(KDIR)"; \
	  echo ">>> Will build user program only (launch)."; \
	  echo ">>> Build kernel module requires linux-headers-$(shell uname -r)."; \
	else \
	  echo ">>> Found kernel headers at $(KDIR)"; \
	fi

kmod:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

user:
	gcc -static $(CFLAGS) -o launch user.c

clean:
	@if [ -n "$(HAVE_KDIR)" ]; then \
	  $(MAKE) -C $(KDIR) M=$(PWD) clean; \
	fi
	$(RM) launch

.PHONY: all info kmod user clean
