/***************************************************************************
 *
 * @file clear.c
 * @brief A simple program to clear the terminal screen using ANSI escape sequences.
 *
 * This program defines a function `clear` that sends an ANSI escape sequence
 * to the terminal to clear the screen and move the cursor to the home position.
 * The `main` function calls this `clear` function and then exits.
 *
 * @author Chris Dedman
 * @date 10/10/2024
 *
 **************************************************************************/

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
