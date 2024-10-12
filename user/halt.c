/****************************************************************************
 * 
 * @file halt.c
 * @brief This file contains the implementation of the halt command for the eXv6 RISC-V operating system.
 *
 * The halt command is intended to stop the operating system. 
 * Currently, it is not implemented and simply prints a message
 * instructing the user to manually shut down QEMU using Ctrl-a x.
 *
 * @note To-Do: Implement the halt system call.
 *
 * @return int Returns 0 upon completion.
 *
 * @author Chris Dedman
 * @date 10/10/2024
 *
 ***************************************************************************/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    fprintf(1, "halt system not implemented... (Ctrl-a x to shutdown QEMU)\n");
    // To-DO: Implement the halt system call
    return 0;
}
