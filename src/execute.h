
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

/// Handle load from indirect register to register
/// @param instruction instruction to be decoded
void doRegisterIndirectToRegister(uint8_t instruction);

/// Handle load from register to register indirect
/// @param instruction instruction to be further decoded
void doRegisterToRegisterIndirect(uint8_t instruction);

/// Handle immediate to indirect memory
void doImmediateIndirect();

/// Handle memory adress to accumulator
void doAddrToAcc();

/// Handle accumulator to memory adress
void doAccToAddr();

/// A and C indirect
void doACIndirect();

void doCIndirectA();

/// HL decrement to A
void doHLDtoA();

/// A to HL decrement
void doAtoHLD();

/// HL increment to A
void doHLItoA();

/// A to HL increment
void doAtoHLI();

/// 8-bit direct to A
void doAa8p();

/// A to 8-bit direct
void doa8pA();

#endif //EXECUTE_H
