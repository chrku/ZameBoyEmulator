
#include "util.h"
#include "rom.h"
#include "cpu.h"
#include <stdio.h>

int loadROM(uint8_t* ROM_mem, size_t mem_size, char* path)
{
  FILE* rom = fopen(path, "r");
  size_t amount_written = 0;
  int current_val;
  uint8_t current;
  uint8_t* memory_pos = ROM_mem;
  while ((current_val = fgetc(rom)) != EOF)
  {
    current = (uint8_t) current_val;
    if (mem_size <= amount_written)
    {
      fclose(rom);
      return ROM_TOO_BIG;
    }
    *memory_pos = current;
    ++memory_pos;
    ++amount_written;
  }
  fclose(rom);
  while (amount_written < mem_size)
  {
    *memory_pos = 0;
    ++memory_pos;
    ++amount_written;
  }
  return SUCCESS;
}
