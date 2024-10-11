/*
 * clear.c - simple program to clear the screen using ANSI escape sequences
 * Author: Chris Dedman
 * Date: 10/10/2024
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void clear(void)
{
  printf("\033[H\033[J"); // ANSI escape sequence to clear the screen
}

int main(void)
{
  clear();
  exit(0);
}
