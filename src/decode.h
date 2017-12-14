
// Instruction decoding for gameboy emulator
// (C) C. Kummer
//

#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

#define SUCCESS 0

/// Decode instruction
/// @param instruction opcode
/// @param data struct to be filled
/// @return int error code
int decodeInstruction(uint8_t instruction, InstructionData* data);

#endif //DECODE_H
