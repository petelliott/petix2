## petix2

a small research-unix like operating system for x86-32. it is released
under the GPL-v3

### build instructions

you will need an `i686-elf` toolchain see [osdev wiki for
instructions](https://wiki.osdev.org/GCC_Cross-Compiler)

```
$ make
```

to run with qemu:

```
$ make run
```

to run with qemu and gdb

```
$ make gdb
```

to build a bootable iso image:

```
$ make petix2.iso
```

### installation instructions

will be available once appropriate disk and file system drivers are ready
