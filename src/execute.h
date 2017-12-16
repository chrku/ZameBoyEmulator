
// Code for all the assembly statements
//

#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdint.h>

/// 8-Bit register immediate load
/// @param instruction will be further decoded
void doLoad8bitImmediate(uint8_t instruction);

/// NOP instruction
void doNop();

/// Handle HALT instruction
void doHalt();

/// Handle register-to-register load
/// @param instruction will be further decoded
void doLoadRegisterRegister(uint8_t instruction);

#endif //EXECUTE_H
