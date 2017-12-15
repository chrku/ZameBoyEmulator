
#include "cpu.h"

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

// Function to stop execution for n cycles
void sleepCycles(size_t n)
{
  //TODO: Implement proper sleeping
  for (size_t c = 0; c < n; ++c)
    ;
}
