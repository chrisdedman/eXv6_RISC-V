/*
 * sleep.c - sleep for a specified number of seconds
 * Author: Chris Dedman
 * Date: 10/10/2024
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (2 != argc)
  {
    fprintf(2, "Usage: sleep <ticks>\n");
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
