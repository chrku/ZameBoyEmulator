
#include "decode.h"
#include "cpu.h"
#include "opcode.h"
#include "execute.h"
#include <stdlib.h>

int decodeAndExecuteInstruction(uint8_t instruction)
{
  switch (instruction)
  {
    ////////////////////////////////////////////////////////////////////////////
    // Control instructions
    // INSTRUCTION TYPE 1: NOP 
    // Do nothing
    case NOP:
      doNop();
      sleepCycles(NOP_CYCLE_COUNT);
      return SUCCESS;
    // HALT instruction: power off cpu until interrupt occurs
    case HALT:
      doHalt();
      sleepCycles(HALT_CYCLE_COUNT);
      return SUCCESS;
    ////////////////////////////////////////////////////////////////////////////
    // 8-bit loads
    // REGISTER-IMMEDIATE LOADS WITH 8-BIT IMMEDIATE ARGUMENT
    case LD_B_d8: case LD_D_d8: case LD_H_d8: case LD_C_d8:
    case LD_E_d8: case LD_L_d8: case LD_A_d8:
      doLoad8bitImmediate(instruction);
      sleepCycles(LD_CYCLE_COUNT_REGISTER_IMMEDIATE_8);
      return SUCCESS;
    // REGISTER-REGISTER LOADS
    case LD_A_A: case LD_A_B: case LD_A_C: case LD_A_D: case LD_A_E:
    case LD_A_H: case LD_A_L:
    case LD_B_A: case LD_B_B: case LD_B_C: case LD_B_D: case LD_B_E:
    case LD_B_H: case LD_B_L:
    case LD_C_A: case LD_C_B: case LD_C_C: case LD_C_D: case LD_C_E:
    case LD_C_H: case LD_C_L:
    case LD_D_A: case LD_D_B: case LD_D_C: case LD_D_D: case LD_D_E:
    case LD_D_H: case LD_D_L:
    case LD_E_A: case LD_E_B: case LD_E_C: case LD_E_D: case LD_E_E:
    case LD_E_H: case LD_E_L:
    case LD_H_A: case LD_H_B: case LD_H_C: case LD_H_D: case LD_H_E:
    case LD_H_H: case LD_H_L:
    case LD_L_A: case LD_L_B: case LD_L_C: case LD_L_D: case LD_L_E:
    case LD_L_H: case LD_L_L:
      doLoadRegisterRegister(instruction);
      sleepCycles(LD_CYCLE_COUNT_REGISTER_REGISTER);
      return SUCCESS;
    // Register-indirect-memory to register
    case LD_A_HL: case LD_B_HL: case LD_C_HL: case LD_D_HL: case LD_E_HL:
    case LD_H_HL: case LD_L_HL: case LD_A_BC: case LD_A_DE:
      doRegisterIndirectToRegister(instruction);
      sleepCycles(LD_MEM_INDIRECT_REGISTER_CYCLE_COUNT);
      return SUCCESS;
    // Register to register-indirect memory
    case LD_HL_A: case LD_HL_B: case LD_HL_C: case LD_HL_D: case LD_HL_E:
    case LD_HL_H: case LD_HL_L: case LD_BC_A: case LD_DE_A:
      doRegisterToRegisterIndirect(instruction);
      sleepCycles(LD_MEM_INDIRECT_REGISTER_CYCLE_COUNT);
      return SUCCESS;
    // Memory address direct to accumulator
    case LD_A_a16p:
      doAddrToAcc();
      sleepCycles(LD_ADDR_TO_REG_CYCLE_COUNT);
      return SUCCESS;
    // Accumulator to memory address
    case LD_a16p_A:
      doAccToAddr();
      sleepCycles(LD_ADDR_TO_REG_CYCLE_COUNT);
      return SUCCESS;
    // Immediate to register indirect
    case LD_HL_d8:
      doImmediateIndirect();
      sleepCycles(LD_IMM_TO_INDIRECT_REGISTER_CYCLE_COUNT);
      return SUCCESS;
    // Accumulutor - C
    case LD_A_C_INDIRECT:
      doACIndirect();
      sleepCycles(LD_A_C_CYCLES);
      return SUCCESS;
    // C - Accumulator
    case LD_C_INDIRECT_A:
      doCIndirectA();
      sleepCycles(LD_A_C_CYCLES);
      return SUCCESS;
    // (HL) dec to A
    case LD_DEC_A:
      doHLDtoA();
      sleepCycles(LD_DEC_A_CYCLES);
      return SUCCESS;
    // A to (HL) dec
    case LD_A_DEC:
      doAtoHLD();
      sleepCycles(LD_DEC_A_CYCLES);
      return SUCCESS;
    // (HL) inc to A
    case LD_INC_A:
      doHLItoA();
      sleepCycles(LD_DEC_A_CYCLES);
      return SUCCESS;
    // A to (HL) inc
    case LD_A_INC:
      doAtoHLI();
      sleepCycles(LD_DEC_A_CYCLES);
      return SUCCESS;
    // 8-bit direct address to A
    case LD_A_a8p:
      doAa8p();
      sleepCycles(LD_A_a8p_CYCLES);
      return SUCCESS;
    // And from A
    case LD_a8p_A:
      doa8pA();
      sleepCycles(LD_A_a8p_CYCLES);
      return SUCCESS;
    ////////////////////////////////////////////////////////////////////////////
    // 16 bit loads
    // Combined register immediate
    case LD_BC_d16: case LD_DE_d16: case LD_HL_d16: case LD_SP_d16:
      doLoadReg16b(instruction);
      sleepCycles(LD_COMB_CYCLES);
      return SUCCESS;
    // HL to stack pointer
    case LD_SP_HL:
      doLoadHLSP();
      sleepCycles(LD_REG_SP_CYCLES);
      return SUCCESS;
    // SP + 8-bit into HL
    case LD_HL_SP_N:
      doLoadHLSPN();
      sleepCycles(LDHL_CYCLES);
      return SUCCESS;
    // SP to address nn
    case LD_d16p_SP:
      doLoadSP16();
      sleepCycles(LD_d16_SP_CYCLES);
      return SUCCESS;
    // Push reg onto stack
    case PUSH_AF: case PUSH_BC: case PUSH_DE: case PUSH_HL:
      doPush(instruction);
      sleepCycles(PUSH_REGS_CYCLES);
      return SUCCESS;
    // Pop reg off stack
    case POP_AF: case POP_BC: case POP_DE: case POP_HL:
      doPop(instruction);
      sleepCycles(POP_REGS_CYCLES);
      return SUCCESS;
    default:
      return -1;
  }
}
