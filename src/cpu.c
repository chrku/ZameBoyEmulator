
#include "cpu.h"
#include "util.h"
#include "decode.h"
#include "graphics.h"
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
uint64_t cycle_counter = 0;


// IO Registers
uint8_t ier = 0;
uint8_t imf = 0;
uint8_t change_interrupt = 0;

// Program counter
uint16_t pc = START_LOCATION;

uint16_t stack_ptr = 0;

// RAM and VRAM
uint8_t RAM[GB_RAM_SIZE];
uint8_t VRAM[GB_VRAM_SIZE];
uint8_t ROM[GB_ROM_SIZE];
uint8_t CPU_RAM[WORK_RAM_SIZE];
uint8_t EXT_RAM[GB_RAM_SIZE];
uint8_t OAM[OAM_SIZE];
uint8_t IO_PORTS[IO_SIZE];

int program_state;

// Function to stop execution for n cycles
void sleepCycles(size_t n)
{
  cycle_counter += n;
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
  memset(EXT_RAM, 0, GB_RAM_SIZE);
  memset(OAM, 0, OAM_SIZE);
  memset(IO_PORTS, 0, IO_SIZE);
}

uint8_t readMemory(uint16_t addr)
{
#ifdef DEBUG
  printf("Reading address %hx\n", addr);
#endif
  // Adresses from 0x
  if (addr <= CART_LIMIT_GB)
  {
    return ROM[addr];
  }
  // Adresses to RAM
  else if (addr >= INTERNAL_RAM_LOWER && addr <= INTERNAL_RAM_UPPER)
  {
    return RAM[addr - INTERNAL_RAM_LOWER];
  }
  // Adresses to working RAM
  else if (addr >= WORKING_RAM_LOWER && addr <= WORKING_RAM_UPPER)
  {
    return CPU_RAM[addr - WORKING_RAM_LOWER];
  }
  else if (addr >= VRAM_LOWER && addr <= VRAM_UPPER)
  {
    return VRAM[addr - VRAM_LOWER];
  }
  else if (addr >= CART_RAM_LOWER && addr <= CART_RAM_UPPER)
  {
    return EXT_RAM[addr - CART_RAM_LOWER];
  }
  else if (addr >= ECHO_LOWER && addr <= ECHO_UPPER)
  {
    return RAM[addr - ECHO_LOWER];
  }
  else if (addr >= OAM_LOWER && addr <= OAM_UPPER)
  {
    return OAM[addr - OAM_LOWER];
  }
  else if (addr >= UNUSABLE_LOWER && addr <= UNUSABLE_UPPER)
  {
#ifdef DEBUG
    printf("Accessed invalid memory location\n");
#endif
    return 0xff;
  }
  else if (addr >= IO_REGS_LOWER && addr <= IO_REGS_UPPER)
  {
    if (addr == DMA_REG)
      return 0xff;
    else
      return IO_PORTS[addr - IO_REGS_LOWER];
  }
  else 
  {
    return ier;
  }
  // TODO: Implement the rest of the memory map
  return 0;
}

int writeMemory(uint16_t addr, uint8_t data)
{
#ifdef DEBUG
  printf("Writing address %hx\n", addr);
#endif
  if (addr <= CART_LIMIT_GB)
  {
    return WRITE_ERROR;
  }
  else if (addr >= INTERNAL_RAM_LOWER && addr <= INTERNAL_RAM_UPPER)
  {
    RAM[addr - INTERNAL_RAM_LOWER] = data;
    return SUCCESS;
  }
  else if (addr >= WORKING_RAM_LOWER && addr <= WORKING_RAM_UPPER)
  {
    CPU_RAM[addr - WORKING_RAM_LOWER] = data;
    return SUCCESS;
  }
  else if (addr >= VRAM_LOWER && addr <= VRAM_UPPER)
  {
    VRAM[addr - VRAM_LOWER] = data;
    return SUCCESS;
  }
  else if (addr >= CART_RAM_LOWER && addr <= CART_RAM_UPPER)
  {
    EXT_RAM[addr - CART_RAM_LOWER] = data;
    return SUCCESS;
  }
  else if (addr >= ECHO_LOWER && addr <= ECHO_UPPER)
  {
    RAM[addr - ECHO_LOWER] = data;
    return SUCCESS;
  }
  else if (addr >= OAM_LOWER && addr <= OAM_UPPER)
  {
    OAM[addr - OAM_LOWER] = data;
    return SUCCESS;
  }
  else if (addr >= UNUSABLE_LOWER && addr <= UNUSABLE_UPPER)
  {
#ifdef DEBUG
    printf("Accessed invalid memory location\n");
#endif
    return 0xff;
  }
  else if (addr >= IO_REGS_LOWER && addr <= IO_REGS_UPPER)
  {
    if (addr == DMA_REG)
      executeDMA(data);
    else
      IO_PORTS[addr - IO_REGS_LOWER] = data;
    return SUCCESS;
  }
  else 
  {
    ier = data;
    return SUCCESS;
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
  initGraphics();
  // For halting, etc.
  program_state = RUNNING;
  uint8_t instruction = 0;
#ifdef DEBUG
  char dbg;
#endif
  // Initialize everything
  initRegisters();
  initMemory();
  GBStartUp();

  // If debug macro is enabled, print the registers
#ifdef DEBUG
  printRegisters();
#endif

  while (program_state == RUNNING)
  {
    instruction = readMemory(pc);
    decodeAndExecuteInstruction(instruction);
    doGraphics();

#ifdef DEBUG
  printRegisters();
  dbg = getchar();
#endif
  }
}

void executeDMA(uint8_t data)
{
  uint16_t addr = data << 8;
  for (int i = 0; i < 160; ++i)
  {
    writeMemory(0xfe00 + i, readMemory(addr + i));
  }
}
