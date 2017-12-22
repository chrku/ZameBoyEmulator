#include "util.h"
#include "cpu.h"
#include <stdio.h>

void printRegisters()
{
  printf("Executing instruction: %hhx\n", readMemory(pc));
  // First, print the general purpose registers
  printf("A: %hhx; B: %hhx; C: %hhx; D: %hhx; E: %hhx; H: %hhx, L: %hhx;\n",
      a_reg, b_reg, c_reg, d_reg, e_reg, h_reg, l_reg);
  // Flags, stack pointer and program counter
  printf("Flags: %hhx; Stack pointer: %hx; Program counter: %hx;\n",
      flags, stack_ptr, pc);
  // Cycle counter
  printf("Cycle counter: %lx;\n", cycle_counter);
  printf("Interrupt master flag: %d; Interrupt flag: %hhx, Interrupt enable %hhx;\n",
      imf, IO_PORTS[0x0F], ier);
  printf("LCDC: %hhx; STAT: %hhx; LY: %hhx\n", IO_PORTS[0x40], IO_PORTS[0x41], IO_PORTS[0x44]);
  printf("TIMA: %hx TMA: %hx; DIV %hx\n", readMemory(TIMA), readMemory(TMA), IO_PORTS[0x4]);
  printf("Memory around stack_ptr %hhx %hhx %hhx\n", readMemory(stack_ptr), readMemory(stack_ptr + 1), readMemory(stack_ptr - 1));
}
