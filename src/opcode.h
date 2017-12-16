
// Opcode definitions for gameboy CPU
// (C) C. Kummer

#ifndef OPCODE_H
#define OPCODE_H

////////////////////////////////////////////////////////////////////////////////
// CONTROL INSTRUCTIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// NO-OP. Does nothing, no arguments
#define NOP 0x00
#define NOP_CYCLE_COUNT 4
#define NOP_ARGLEN 1

// HALT. Halts program execution until interrupt occurs
#define HALT 0x76
#define HALT_CYCLE_COUNT 4
#define HALT_ARGLEN 1

////////////////////////////////////////////////////////////////////////////////
// LOAD INSTRUCTIONS ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 8-bit loads

////////////////////////////////////////////////////////////////////////////////
// Register-immediate loads/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Cycle count for these loads
#define LD_CYCLE_COUNT_REGISTER_IMMEDIATE_8 8
// TODO: Fix name
#define LD_REGISTER_IMMEDIATE_LEN 2

#define LD_B_d8 0x06
#define LD_D_d8 0x16
#define LD_H_d8 0x26
#define LD_C_d8 0x0e
#define LD_E_d8 0x1e
#define LD_L_d8 0x2e
#define LD_A_d8 0x3e

////////////////////////////////////////////////////////////////////////////////
// Register-register loads /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define LD_CYCLE_COUNT_REGISTER_REGISTER 4
#define LD_REGISTER_REGISTER_ARGLEN 1

// Load into A
#define LD_A_A 0x7f
#define LD_A_B 0x78
#define LD_A_C 0x79
#define LD_A_D 0x7a
#define LD_A_E 0x7b
#define LD_A_H 0x7c
#define LD_A_L 0x7d

// Load into B
#define LD_B_A 0x47
#define LD_B_B 0x40
#define LD_B_C 0x41
#define LD_B_D 0x42
#define LD_B_E 0x43
#define LD_B_H 0x44
#define LD_B_L 0x45

// Load into C
#define LD_C_A 0x4f
#define LD_C_B 0x48
#define LD_C_C 0x49
#define LD_C_D 0x4a
#define LD_C_E 0x4b
#define LD_C_H 0x4c
#define LD_C_L 0x4d

// Load into D
#define LD_D_A 0x57
#define LD_D_B 0x50
#define LD_D_C 0x51
#define LD_D_D 0x52
#define LD_D_E 0x53
#define LD_D_H 0x54
#define LD_D_L 0x55

// Load into E
#define LD_E_A 0x5f
#define LD_E_B 0x58
#define LD_E_C 0x59
#define LD_E_D 0x5a
#define LD_E_E 0x5b
#define LD_E_H 0x5c
#define LD_E_L 0x5d

// Load into H
#define LD_H_A 0x67
#define LD_H_B 0x60
#define LD_H_C 0x61
#define LD_H_D 0x62
#define LD_H_E 0x63
#define LD_H_H 0x64
#define LD_H_L 0x65

// Load into L
#define LD_L_A 0x6f
#define LD_L_B 0x68
#define LD_L_C 0x69
#define LD_L_D 0x6a
#define LD_L_E 0x6b
#define LD_L_H 0x6c
#define LD_L_L 0x6d

////////////////////////////////////////////////////////////////////////////////

#endif // OPCODE_H
