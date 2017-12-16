
#include "cpu.h"
#include "util.h"
#include "decode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Definitions for CPU data structures
//

// Registers
uint8_t a_reg = 0;
uint8_t b_reg = 0;
uint8_t c_reg = 0;
uint8_t d_reg = 0;
uint8_t e_reg = 0;
uint8_t h_reg = 0;
uint8_t l_reg = 0;
uint8_t flags = 0;

// Program counter
uint16_t pc = START_LOCATION;

uint16_t stack_ptr = 0;

// RAM and VRAM
uint8_t RAM[GB_RAM_SIZE];
uint8_t VRAM[GB_VRAM_SIZE];
uint8_t ROM[GB_ROM_SIZE];
uint8_t CPU_RAM[WORK_RAM_SIZE];
uint8_t INTERNAL_ROM[INTERNAL_ROM_SIZE];

// Function to stop execution for n cycles
void sleepCycles(size_t n)
{
  //TODO: Implement proper sleeping
  for (size_t c = 0; c < n; ++c)
    ;
}

void initRegisters()
{
  // Initialize registers to standardized values at program start
  a_reg = A_INIT_VALUE_GB;
  flags = FLAGS_INIT_VALUE_GB;
  b_reg = B_INIT_VALUE_GB;
  c_reg = C_INIT_VALUE_GB;
  d_reg = D_INIT_VALUE_GB;
  e_reg = E_INIT_VALUE_GB;
  h_reg = H_INIT_VALUE_GB;
  l_reg = L_INIT_VALUE_GB;
  stack_ptr = STACK_INIT_VALUE_GB;
  pc = START_LOCATION;
}

void initMemory()
{
  // Initialize memory segments
  memset(RAM, 0, GB_RAM_SIZE);
  memset(VRAM, 0, GB_VRAM_SIZE);
  memset(CPU_RAM, 0, WORK_RAM_SIZE);
}

uint8_t accessMemory(uint16_t addr)
{
  // Adresses from 0x
  if (addr <= CART_LIMIT_GB)
  {
    return ROM[addr];
  }
  // TODO: Implement the rest of the memory map
  return 0;
}

void GBStartUp()
{
  // TODO: Implement the proper startup sequence
  return;
}

void startExecutionGB()
{
  // For halting, etc.
  int isRunning = RUNNING;
  uint8_t instruction = 0;
  char dbg;
  // Initialize everything
  initRegisters();
  initMemory();
  GBStartUp();

  // If debug macro is enabled, print the registers
#ifdef DEBUG
  printRegisters();
#endif

  while (isRunning)
  {
    instruction = accessMemory(pc);
    decodeAndExecuteInstruction(instruction);

#ifdef DEBUG
  printRegisters();
  dbg = getchar();
#endif
  }
}
