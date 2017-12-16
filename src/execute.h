
// Code for all the assembly statements
//

#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdint.h>

// 8-Bit register immediate load
void doLoad8bitImmediate(uint8_t instruction);

// NOP instruction
void doNop();

// Handle HALT instruction
void doHalt();

#endif //EXECUTE_H
