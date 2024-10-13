/*************************************************************************
 * @file main.c
 * @brief Main entry point for the eXv6 kernel on RISC-V architecture.
 *
 * This file contains the main function which is the entry point for the kernel
 * in supervisor mode on all CPUs. It initializes various subsystems and starts
 * the first user process.
 *
 * @details
 * The main function performs the following steps:
 * - Initializes the console and prints boot messages.
 * - Initializes the physical page allocator, kernel page table, process table,
 *   trap vectors, interrupt controller, buffer cache, inode table, file table,
 *   and emulated hard disk.
 * - Starts the first user process.
 * - Synchronizes and starts other harts (hardware threads) if present.
 * - Each hart initializes its own paging, trap vector, and interrupt controller.
 * - Finally, the scheduler is called to start process scheduling.
 *
 * @note The variable `started` is used to synchronize the startup of
 * multiple harts. Each hart waits until the primary hart (hart 0) has
 * completed the initialization.
 * The variable `waitcyle` is used to count the number of cycles each hart waits.
 *
 * @todo Add RISC-V cycle counter to measure the time taken for each hart to start.
 * For RISC-V, we can use the cycle counter, which is a special hardware
 * register available in most implementations. This register counts the number
 * of clock cycles since the core was started, and it can be accessed using
 * the rdcycle instruction.
 *
 * @see kernel/risv.h for RISC-V rdcycle implementation (TO-DO).
 *
 *************************************************************************/

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void main()
{
  uint waitcyle[NCPU];

  if (cpuid() == 0)
  {
    consoleinit();
    printfinit();

    printf("\n");
    printf("eXv6 kernel is booting\n");
    printf("\n");

    kinit();            // physical page allocator
    kvminit();          // create kernel page table
    kvminithart();      // turn on paging
    procinit();         // process table
    trapinit();         // trap vectors
    trapinithart();     // install kernel trap vector
    plicinit();         // set up interrupt controller
    plicinithart();     // ask PLIC for device interrupts
    binit();            // buffer cache
    iinit();            // inode table
    fileinit();         // file table
    virtio_disk_init(); // emulated hard disk
    userinit();         // first user process

    printf("\nhart %d started\n", cpuid());
    __sync_synchronize();

    started = 1;
  }
  else
  {
    waitcyle[cpuid()] = 0;

    while (started == 0)
    {
      waitcyle[cpuid()]++;
    }
    __sync_synchronize();

    printf("hart %d starting: wait cycle %d\n", cpuid(), waitcyle[cpuid()]);

    kvminithart();  // turn on paging
    trapinithart(); // install kernel trap vector
    plicinithart(); // ask PLIC for device interrupts
  }

  scheduler();
}
