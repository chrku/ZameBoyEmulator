
// Instruction decoding for gameboy emulator
// (C) C. Kummer
//

#ifndef DECODE_H
#define DECODE_H

// Register constants
typedef enum _Registers_
{
  A, F, B, C, D, E, H, L, SP, PC
} Register;

// Instruction type
typedef enum _Instructions_
{
  LD, ADD
} Instruction;

typedef struct _InstructionData_ 
{
  Instruction instruction; //
  Register reg1;
  Register reg2;
  uint8_t immediateValue1;
} InstructionData;

#endif //DECODE_H
