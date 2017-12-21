
#include <stdbool.h>
#include <SDL2/SDL.h>
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
  IO_PORTS[0x05] = 0;
  IO_PORTS[0x06] = 0;
  IO_PORTS[0x07] = 0;
  IO_PORTS[0x10] = 0x80;
  IO_PORTS[0x11] = 0xBF;
  IO_PORTS[0x12] = 0xF3;
  IO_PORTS[0x14] = 0xBF;
  IO_PORTS[0x16] = 0x3F;
  IO_PORTS[0x17] = 0;
  IO_PORTS[0x19] = 0xBF;
  IO_PORTS[0x1A] = 0x7F;
  IO_PORTS[0x1B] = 0xFF;
  IO_PORTS[0x1C] = 0x9F;
  IO_PORTS[0x1E] = 0xBF;
  IO_PORTS[0x20] = 0xFF;
  IO_PORTS[0x21] = 0;
  IO_PORTS[0x22] = 0;
  IO_PORTS[0x23] = 0xBF;
  IO_PORTS[0x24] = 0x77;
  IO_PORTS[0x25] = 0xF3;
  IO_PORTS[0x26] = 0xF1;
  IO_PORTS[0x40] = 0x91;
  IO_PORTS[0x42] = 0;
  IO_PORTS[0x43] = 0;
  IO_PORTS[0x45] = 0;
  IO_PORTS[0x47] = 0xFC;
  IO_PORTS[0x48] = 0xFF;
  IO_PORTS[0x49] = 0x00;
  IO_PORTS[0x4a] = 0x00;
  IO_PORTS[0x4b] = 0x00;
  ier = 0;
  imf = 1;
  return;
}

void startExecutionGB()
{
  uint64_t last_cycle_count = 0;
  uint32_t elapsed = 0;
  bool debug = true;
  uint16_t break_on;
  initGraphics();
  // For halting, etc.
  program_state = RUNNING;
  uint8_t instruction = 0;
  // Initialize everything
  initMemory();
  initRegisters();
  GBStartUp();

  // If debug macro is enabled, print the registers
#ifdef DEBUG
  printRegisters();
#endif

  while (program_state == RUNNING)
  {
    elapsed = SDL_GetTicks();
    while (cycle_counter - last_cycle_count <= 4194304)
    {
      if (pc == break_on) 
        debug = true;
      if (debug)
      {
        char choice = getchar();
        switch (choice)
        {
          case 'd':
            debug = false;
            break;
          case 'j':
            scanf("%hx", &pc);
            break;
          case 'e':
            scanf("%hx", &break_on);
            debug = false;
            break;
        }
        printRegisters();
      }
      instruction = readMemory(pc);
      decodeAndExecuteInstruction(instruction);
      doGraphics();
      if (cycle_counter % 50000)
        doEventLoop();
      doInterrupts();
    }
    elapsed = SDL_GetTicks() - elapsed;
    printf("Elapsed: %d\n", elapsed);
    last_cycle_count = cycle_counter;
    // if (1000 - (int) timer > 0)
    //  SDL_Delay(1000 - timer);
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

void doEventLoop()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT:
        exit(0);
        break;
    }
  }
}

void doInterrupts()
{
  // If interrupt master flag is disabled
  if (!imf)
    return;
  uint8_t enabled_interrupts = readMemory(0xffff);
  uint8_t int_flags = readMemory(0xff0f);
  if (
        ((enabled_interrupts & 0x1) && (int_flags & 0x1)) ||
        ((enabled_interrupts & 0x2) && (int_flags & 0x2)) ||
        ((enabled_interrupts & 0x4) && (int_flags & 0x4)) ||
        ((enabled_interrupts & 0x8) && (int_flags & 0x8)) ||
        ((enabled_interrupts & 0x10) && (int_flags & 0x10)) 
     )
  {
    // Disable interrupts
    imf = 0;
    // Save pc on stack
    writeMemory(stack_ptr, (uint8_t) (pc >> 8));
    stack_ptr += 1;
    writeMemory(stack_ptr, (uint8_t) pc);
    stack_ptr += 1;
    // I think its twelve??!!
    sleepCycles(12);
    if ((enabled_interrupts & 0x1) && (int_flags & 0x1))
      pc = 0x40;
    else if ((enabled_interrupts & 0x2) && (int_flags & 0x2))
      pc = 0x48;
    else if ((enabled_interrupts & 0x4) && (int_flags & 0x4))
      pc = 0x50;
    else if ((enabled_interrupts & 0x8) && (int_flags & 0x8))
      pc = 0x58;
    else if ((enabled_interrupts & 0x10) && (int_flags & 0x10))
      pc = 0x60;
  }
}

void requestInterrupt(interrupt i)
{
  switch(i)
  {
    case BLANK:
      IO_PORTS[0x0f] |= 0x1;
      break;
    case LCD_STAT:
      IO_PORTS[0x0f] |= 0x2;
      break;
    case TIMER:
      IO_PORTS[0x0f] |= 0x4;
      break;
    case SERIAL:
      IO_PORTS[0x0f] |= 0x8;
      break;
    case JOYPAD:
      IO_PORTS[0x0f] |= 0x10;
      break;
  }
}
