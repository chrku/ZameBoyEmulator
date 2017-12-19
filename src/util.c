#include "util.h"
#include "cpu.h"
#include <stdio.h>

void printRegisters()
{
  // First, print the general purpose registers
  printf("A: %hhx; B: %hhx; C: %hhx; D: %hhx; E: %hhx; H: %hhx, L: %hhx;\n",
      a_reg, b_reg, c_reg, d_reg, e_reg, h_reg, l_reg);
  // Flags, stack pointer and program counter
  printf("Flags: %hhx; Stack pointer: %hx; Program counter: %hx;\n",
      flags, stack_ptr, pc);
  // Cycle counter
  printf("Cycle counter: %llx;\n", cycle_counter);
}
