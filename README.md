## eXv6 RISC-V OS - A Learning Base for Operating Systems
_eXv6 - Experimental Version 6_

This is a fork of the [xv6-riscv](https://github.com/mit-pdos/xv6-riscv) operating system, which is a re-implementation of the Unix Version 6 operating system for RISC-V. This project is intended to be used as a learning base for the Operating System class I am taking from my undergrad at California State University, Dominguez Hills. This project is not part of my class assignment, but rather a personal project to learn more about operating systems and [RISC-V](https://en.wikipedia.org/wiki/RISC-V) architecture.

### Building & Running eXv6
You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu.  Once they are installed, and in your shell
search path, you can run "make qemu".

## Acknowledgments
This following lines below are kept from the original fork **README** file.

```txt
xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.
```

```c
xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)).  See also https://pdos.csail.mit.edu/6.1810/, which provides
pointers to on-line resources for v6.
```