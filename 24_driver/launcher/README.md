# A Device Driver

**A Device Driver**: Once we understand that a device driver’s role is to translate system calls into data that the device can understand, we can implement the corresponding operations in `file_operations` to simulate a device.

This project implements a simple character device driver called `nuke`.  
The driver registers two device nodes, `/dev/nuke0` and `/dev/nuke1`.  
When the correct secret is written to `/dev/nuke0`, the kernel prints an ASCII “explosion” art to the system log.

---

## Repository Layout

- `driver.c`  
  Kernel module that:
  - registers a character device
  - implements `launcher_read` for `read(2)`
  - implements `launcher_write` for `write(2)`
  - sets up the device class and cleans it up on module unload

- `user.c`  
  A small user-space program that opens `/dev/nuke0` and writes a fixed secret to it.

- `Makefile`  
  Builds the kernel module and the user program and provides helper targets such as `load`, `dev`, `test`, `run`, and `unload`.

---

## Building

From the project directory:

```sh
make
````

This command:

1. Builds the kernel module `driver.ko` using the headers for the current kernel.
2. Builds the static user program `launch` from `user.c`.

To remove build artifacts:

```sh
make clean
```

---

## Make Targets and Typical Workflow

The Makefile defines several useful targets.

### 1. Build everything

```sh
make
```

Builds both `driver.ko` and `launch`.

### 2. Load the module

```sh
make load
```

The `load` target usually performs:

* Remove an existing `driver` module if it is already loaded.
* Insert the new module with:

  ```sh
  sudo insmod driver.ko
  ```

You can also run `sudo insmod driver.ko` manually if you prefer.

### 3. Create device nodes

```sh
make dev
```

The `dev` target typically:

1. Looks up the major number for `nuke` in `/proc/devices`.

2. Creates two character devices using that major number:

   * `/dev/nuke0` with minor 0
   * `/dev/nuke1` with minor 1

   using `mknod`.

3. Sets permissions so that both devices are readable and writable:

   ```sh
   sudo chmod 666 /dev/nuke0 /dev/nuke1
   ```

If `udev` already created the device nodes automatically, this target will just fix the permissions and list the devices.

### 4. Test the driver

There are two common ways to test.

#### Option A: Use the `test` target

```sh
make test
```

A typical `test` rule does:

1. Write the expected secret string to `/dev/nuke0`, for example:

   ```sh
   echo "COP4610" > /dev/nuke0
   ```

   (Replace the string if your driver compares a different secret.)

2. Show the recent kernel log:

   ```sh
   sudo dmesg | tail -n 40
   ```

If the secret matches, you should see in the `dmesg` output:

* `nuke: correct password entered.`
* The ASCII explosion art printed as multiple lines after that message

Note: the ASCII art appears in the **kernel log**, not directly in your terminal.

#### Option B: Use the user program `launch`

After the module is loaded and `/dev/nuke0` exists:

```sh
./launch
sudo dmesg | tail -n 40
```

The `launch` program writes the `SECRET` bytes defined in `user.c` to `/dev/nuke0`.
If the comparison logic in `driver.c` matches that secret, the ASCII art will be printed in the kernel log.

### 5. One-shot: build + load + dev + test

```sh
make run
```

The `run` target is usually defined as:

```make
run: all load dev test
```

This is equivalent to:

```sh
make           # build everything
make load      # insert the module
make dev       # create /dev/nuke0 and /dev/nuke1 and set permissions
make test      # write the secret and show the log
```

---

### 6. Unload the module

```sh
make unload
```

This target typically runs:

```sh
sudo rmmod driver
```

You can also call `sudo rmmod driver` manually to unload the module.

---

## Notes

* You must have the correct `linux-headers` package installed for your running kernel, otherwise the module will not build.
* On newer kernels the prototype of `class_create` has changed. The driver code may use `LINUX_VERSION_CODE` and `KERNEL_VERSION(...)` macros to choose the correct form at compile time.
* On some systems `dmesg` is restricted to `root`. In that case always use `sudo dmesg` when viewing the kernel log.
* If the device nodes become corrupted during repeated experiments (for example if `/dev/nuke0` turns into a regular file), you can:

  1. Unload the module,
  2. Remove `/dev/nuke0` and `/dev/nuke1`,
  3. Run `make load` and `make dev` again to recreate clean character devices.

