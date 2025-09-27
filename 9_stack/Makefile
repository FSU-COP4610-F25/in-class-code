CC = gcc
CFLAGS = -z execstack -fno-stack-protector -no-pie -g
TARGET = stack
SRC = stack.c

# Default action: Disable ASLR -> Compile -> Set setuid
all: disable_aslr compile setuid

# 1. Disable ASLR
disable_aslr:
	@echo "Disabling ASLR..."
	@sudo sysctl -w kernel.randomize_va_space=0

# 2. Compile
compile: $(SRC)
	@echo "Compiling $(SRC) with stack execution enabled..."
	# Remove immutable attribute first to avoid "Operation not permitted"
	@sudo chattr -i $(TARGET) 2>/dev/null || true
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# 3. Set setuid
setuid:
	@echo "Setting $(TARGET) as setuid root..."
	@sudo chown root $(TARGET)
	@sudo chmod 4755 $(TARGET)

# --------- Additional Commands ---------

# Protect the file by setting the immutable attribute
protect:
	@echo "Protecting $(TARGET) from accidental deletion..."
	@sudo chattr +i $(TARGET)

# Remove the immutable attribute
unprotect:
	@echo "Removing immutable attribute from $(TARGET)..."
	@sudo chattr -i $(TARGET)

# Restore the file using /proc/$PID/exe if the process is running
restore:
	@echo "Attempting to restore $(TARGET) from a running process..."
	@if [ -e /proc/$$(/usr/bin/pgrep $(TARGET))/exe ]; then \
		cp /proc/$$(/usr/bin/pgrep $(TARGET))/exe ./$(TARGET); \
		sudo chmod 4755 $(TARGET); \
		echo "$(TARGET) restored successfully."; \
	else \
		echo "No running instance found. Recompile using 'make'."; \
	fi

# Clean up: remove the immutable attribute first, then delete the file
clean:
	@echo "Cleaning up..."
	@sudo chattr -i $(TARGET) 2>/dev/null || true
	@rm -f $(TARGET)