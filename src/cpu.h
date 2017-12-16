
// CPU data structures for emulator project, definitions, specs...
// (C) C. Kummer
// Gameboy CPU (LR35902)

#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>

#define DEBUG

// CPU specs

// Clock speed
#define CLK_SPEED 4.194304

// Size of the memory bus
#define BUS_SIZE 65536
// Size of the RAM of the original game boy
#define GB_RAM_SIZE 8000
// Size of the VRAM of the original game boy
#define GB_VRAM_SIZE 8000
// Size of the ROM memory segment
#define GB_ROM_SIZE 32000
// Cartridge RAM
#define CARTRIDGE_ROM_SIZE 8000
// Working RAM
#define WORK_RAM_SIZE 127

#define START_LOCATION 0x100

#define INTERNAL_ROM_SIZE 256

////////////////////////////////////////////////////////////////////////////////
// INITIAL VALUES FOR REGISTERS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define A_INIT_VALUE_GB 0x01
#define FLAGS_INIT_VALUE_GB 0xb0
#define B_INIT_VALUE_GB 0x00
#define C_INIT_VALUE_GB 0x13
#define D_INIT_VALUE_GB 0x00
#define E_INIT_VALUE_GB 0xd8
#define H_INIT_VALUE_GB 0x01
#define L_INIT_VALUE_GB 0x4d
#define STACK_INIT_VALUE_GB 0xFFFe

////////////////////////////////////////////////////////////////////////////////
// MEMORY MAP //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define CART_LIMIT_GB 0x7fff

////////////////////////////////////////////////////////////////////////////////
// Constants ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RUNNING 1
#define OFF 0


// Registers
// 8 bit registers

extern uint8_t a_reg; // Accumulator register
extern uint8_t flags; // Flags register

extern uint8_t b_reg; // B, C, D, E, H, L registers
extern uint8_t c_reg;
extern uint8_t d_reg;
extern uint8_t e_reg;
extern uint8_t h_reg;
extern uint8_t l_reg;

// 16 bit registers

extern uint16_t stack_ptr; // Stack pointer
extern uint16_t pc; // Program counter

// Program state
extern int program_state;

// Memory

// Main RAM
extern uint8_t RAM[GB_RAM_SIZE];

// VRAM
extern uint8_t VRAM[GB_VRAM_SIZE];

// ROM segment 
extern uint8_t ROM[GB_ROM_SIZE];

// Working & stack RAM
extern uint8_t CPU_RAM[WORK_RAM_SIZE];

// Internal ROM
extern uint8_t INTERNAL_ROM[INTERNAL_ROM_SIZE];

// Sleep for n cycles
extern void sleepCycles(size_t n);

// Initialize the registers to the standarddized values
extern void initRegisters();

// Initialize the memory segments
extern void initMemory();

// Do the standard GB startup sequence
extern void GBStartUp();

// Start the execution loop for the GB
extern void startExecutionGB();

// Access the memory segment
uint8_t accessMemory(uint16_t addr);

#endif // CPU_H
