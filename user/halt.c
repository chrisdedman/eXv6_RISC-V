/*
 * Halt the system
 * Author: Chris Dedman
 * Date: 10/10/2024
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    fprintf(1, "halt system not implemented... (Ctrl-a x to shutdown QEMU)\n");
    // To-DO: Implement the halt system call
    return 0;
}
