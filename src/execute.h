
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

/// Combined register immediate
void doLoadReg16b(uint8_t instruction);

/// HL to SP
void doLoadHLSP();

/// SP + n address into HL
void doLoadHLSPN();

/// SP to 16-bit address
void doLoadSP16();

/// Push instruction
void doPush(uint8_t instruction);

/// Pop instruction
void doPop(uint8_t instruction);

/// Add accumulator register
void add(uint8_t instruction);

/// Add accumulator with carry
void adc(uint8_t instruction);

/// Subtract accumulator register
void sub(uint8_t instruction);

/// Subtract accumulator with carry
void sbc(uint8_t instruction);

/// And accumulator
void land(uint8_t instruction);

/// Or accumultor
void lor(uint8_t instruction);

/// Xor accumulator
void lxor(uint8_t instruction);

/// Compare accumulator
void cp(uint8_t instruction);

/// Increment accumulator
void inc(uint8_t instruction);

/// Decrement accumulator
void dec(uint8_t instruction);

/// 16-bit add
void add16(uint8_t instruction);

/// Add immediate to SP
void addSPN();

/// 16-bit inc
void inc16(uint8_t instruction);

/// 16-bit dec
void dec16(uint8_t instruction);

/// Swap upper and lower nibbles
void doSwap(uint8_t* arg, uint16_t addr);

/// WTF?!?!?
void daa();

#endif //EXECUTE_H
