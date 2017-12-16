
// Main file
// (C) C. Kummer

#include "cpu.h"
#include "rom.h"
#include "util.h"
#include <stdio.h>

#define ARG_NUMBER 2
#define INVALID_ARGS 1

int main(int argc, char* argv[])
{
  if (argc != ARG_NUMBER)
  {
    printf("Usage: ./emu rom-file\n");
    return INVALID_ARGS;
  }
  loadROM(ROM, GB_ROM_SIZE, argv[1]);
  startExecutionGB();
  return SUCCESS;
}

