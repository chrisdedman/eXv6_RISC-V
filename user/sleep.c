/**************************************************************************
 *
 * @file sleep.c
 * @brief A simple program to sleep for a specified number of seconds.
 *
 * This program takes a single command-line argument representing
 * the number of seconds to sleep. It converts the seconds to
 * ticks (assuming 10 ticks per second) and then calls the sleep function.
 *
 * Usage:
 *   sleep <seconds>
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 *
 * @return Returns 0 on successful completion, or exits with an error code if the input is invalid.
 *
 * @author Chris Dedman
 * @date 10/10/2024
 *
 *************************************************************************/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (2 != argc)
  {
    fprintf(2, "Usage: sleep <seconds>\n");
    exit(1);
  }

  int seconds = atoi(argv[1]);
  if (0 > seconds)
  {
    fprintf(2, "Invalid argument: must be a positive number\n");
    exit(1);
  }

  int ticks = seconds * 10; // Convert seconds to ticks (10 ticks per second)
  sleep(ticks);
  exit(0);
}
