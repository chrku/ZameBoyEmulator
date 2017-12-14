
// CPU data structures for emulator project
// (C) C. Kummer
// Gameboy CPU (LR35902)

// Registers


#ifndef CPU_H
#define CPU_H

#include <stdint.h>

// CPU specs

#define CLK_SPEED 4.194304

// Size of the memory bus
#define BUS_SIZE 65536
// Size of the RAM of the original game boy
#define GB_RAM_SIZE 8000

// 8 bit registers

extern uint8_t accumulator; // Accumulator register
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

// Memory



#endif // CPU_H
