
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
int decodeAndExecuteInstruction(uint8_t instruction);

#endif //DECODE_H
