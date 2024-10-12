/*****************************************************************
 *
 * @file console.c
 * @brief Console driver for the eXv6 RISC-V operating system kernel.
 *
 * This file contains the implementation of the console driver, which handles
 * input and output operations for the console. It includes functions for
 * writing characters to the console, reading input from the console, and
 * handling console interrupts. The console driver also manages a buffer for
 * storing input characters and provides initialization routines for setting
 * up the console.
 *
 * The console driver supports special control characters for managing input,
 * such as backspace, end-of-file, and line kill. It also provides mechanisms
 * for echoing input characters back to the console and waking up processes
 * waiting for input.
 *
 * The main functions provided by this file are:
 * - consputc: Outputs a character to the console, handling backspace appropriately.
 * - consolewrite: Writes a sequence of characters to the console.
 * - consoleread: Reads input from the console into a user-space buffer.
 * - consoleintr: Handles console interrupts and processes input characters.
 * - consoleinit: Initializes the console and sets up the necessary locks and UART.
 *
 * The console driver uses a spinlock to synchronize access to the console buffer
 * and ensures that input and output operations are performed atomically.
 *
 * Implements special input characters:
 * - newline -- end of line
 * - control-h -- backspace
 * - control-u -- kill line
 * - control-d -- end of file
 * - control-p -- print process list
 *
 *****************************************************************/

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

#define BACKSPACE 0x100
#define C(x) ((x) - '@') // Control-x

/**
 * @brief Outputs a character to the console, handling backspace appropriately.
 *
 * This function sends a character to the console. If the character is a backspace,
 * it overwrites the previous character with a space and then moves the cursor back.
 * Otherwise, it simply outputs the character.
 *
 * @param c The character to output. If c is BACKSPACE, the function handles it
 *          by overwriting the previous character with a space and moving the cursor back.
 */
void consputc(int c)
{
    if (c == BACKSPACE)
    {
        // if the user typed backspace, overwrite with a space.
        uartputc_sync('\b');
        uartputc_sync(' ');
        uartputc_sync('\b');
    }
    else
    {
        uartputc_sync(c);
    }
}

struct
{
    struct spinlock lock;

#define INPUT_BUF_SIZE 128
    char buf[INPUT_BUF_SIZE];
    uint r; // Read index
    uint w; // Write index
    uint e; // Edit index
} cons;

/**
 * Writes a sequence of characters to the console.
 *
 * @param user_src Indicates whether the source is user space (non-zero) or kernel space (zero).
 * @param src The source address of the characters to write.
 * @param n The number of characters to write.
 * @return The number of characters successfully written to the console.
 *
 * This function copies characters from the specified source address and writes them
 * to the console using the uartputc function. If either_copyin fails to copy a character,
 * the function stops and returns the number of characters written so far.
 */
int consolewrite(int user_src, uint64 src, int n)
{
    int i;

    for (i = 0; i < n; i++)
    {
        char c;
        if (either_copyin(&c, user_src, src + i, 1) == -1)
        {
            break;
        }

        uartputc(c);
    }

    return i;
}

/**
 * @brief Reads input from the console into a user-space buffer.
 *
 * This function reads up to `n` bytes from the console input buffer into the
 * user-space buffer pointed to by `dst`. It handles end-of-file and newline
 * characters appropriately, ensuring that the caller gets a 0-byte result
 * when end-of-file is encountered and returning when a whole line has been
 * read.
 *
 * @param user_dst Indicates whether the destination buffer is in user space.
 * @param dst The destination buffer where the input will be copied.
 * @param n The maximum number of bytes to read.
 * @return The number of bytes actually read, or -1 if the process was killed.
 */
int consoleread(int user_dst, uint64 dst, int n)
{
    uint target;
    int c;
    char cbuf;

    target = n;
    acquire(&cons.lock);
    while (n > 0)
    {
        // wait until interrupt handler has put some
        // input into cons.buffer.
        while (cons.r == cons.w)
        {
            if (killed(myproc()))
            {
                release(&cons.lock);
                return -1;
            }

            sleep(&cons.r, &cons.lock);
        }

        c = cons.buf[cons.r++ % INPUT_BUF_SIZE];

        if (c == C('D'))
        { // end-of-file
            if (n < target)
            {
                // Save ^D for next time, to make sure
                // caller gets a 0-byte result.
                cons.r--;
            }
            break;
        }

        // copy the input byte to the user-space buffer.
        cbuf = c;
        if (either_copyout(user_dst, dst, &cbuf, 1) == -1)
        {
            break;
        }

        dst++;
        --n;

        if (c == '\n')
        {
            // a whole line has arrived, return to
            // the user-level read().
            break;
        }
    }
    release(&cons.lock);

    return target - n;
}

/**
 * @brief Handles console interrupts.
 *
 * This function is called when a character is received from the console.
 * It processes special control characters and stores regular characters
 * in the console buffer for later consumption by consoleread().
 *
 * @param c The character received from the console.
 *
 * Special characters:
 * - C('P'): Prints the process list by calling procdump().
 * - C('U'): Kills the current line by deleting characters until a newline is found.
 * - C('H') or '\x7f': Acts as a backspace, deleting the last character in the buffer.
 *
 * Regular characters are echoed back to the console and stored in the buffer.
 * If a newline, end-of-file (C('D')), or the buffer is full, it wakes up
 * consoleread() to process the input.
 */
void consoleintr(int c)
{
    acquire(&cons.lock);

    switch (c)
    {
    case C('P'): // Print process list.
        procdump();
        break;

    case C('U'): // Kill line.
        while ((cons.e != cons.w) && (cons.buf[(cons.e - 1) % INPUT_BUF_SIZE] != '\n'))
        {
            cons.e--;
            consputc(BACKSPACE);
        }
        break;

    case C('H'): // Backspace
    case '\x7f': // Delete key
        if (cons.e != cons.w)
        {
            cons.e--;
            consputc(BACKSPACE);
        }
        break;

    default:
        if (c != 0 && cons.e - cons.r < INPUT_BUF_SIZE)
        {
            c = (c == '\r') ? '\n' : c;

            consputc(c); // echo back to the user

            // store for consumption by consoleread().
            cons.buf[cons.e++ % INPUT_BUF_SIZE] = c;

            if (c == '\n' || c == C('D') || cons.e - cons.r == INPUT_BUF_SIZE)
            {
                // wake up consoleread() if a whole line (or end-of-file)
                // has arrived.
                cons.w = cons.e;
                wakeup(&cons.r);
            }
        }
        break;
    }

    release(&cons.lock);
}

/**
 * @brief Initializes the console.
 *
 * This function sets up the console by initializing the console lock,
 * initializing the UART, and connecting the read and write system calls
 * to the console read and write functions.
 */
void consoleinit(void)
{
    initlock(&cons.lock, "cons");

    uartinit();

    devsw[CONSOLE].read = consoleread;
    devsw[CONSOLE].write = consolewrite;
}
