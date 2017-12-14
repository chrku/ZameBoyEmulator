
// Opcode definitions for gameboy CPU
// (C) C. Kummer

#ifndef OPCODE_H
#define OPCODE_H

// CONTROL INSTRUCTIONS

// NO-OP. Does nothing, no arguments
#define NOP 0x00
#define NOP_CYCLE_COUNT 4
#define NOP_ARGLEN 1

// LOAD INSTRUCTIONS

// 8-bit loads

////////////////////////////////////////////////////////////////////////////////
// Register-immediate loads/////////////////////////////////////////////////////
//
// Cycle count for these loads
#define LD_CYCLE_COUNT_REGISTER_IMMEDIATE 8

#define LD_B_d8 0x06
#define LD_D_d8 0x16
#define LD_H_d8 0x26
#define LD_C_d8 0x0e
#define LD_E_d8 0x1e
#define LD_L_d8 0x2e
#define LD_H_d8 0x3e

////////////////////////////////////////////////////////////////////////////////

#endif // OPCODE_H
