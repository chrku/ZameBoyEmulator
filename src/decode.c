
#include "decode.h"
#include "cpu.h"
#include "opcode.h"
#include "execute.h"
#include <stdlib.h>

int decodeAndExecuteInstruction(uint8_t instruction)
{
  switch (instruction)
  {
    // INSTRUCTION TYPE 1: NOP 
    // Do nothing
    case NOP:
      doNop();
      sleepCycles(NOP_CYCLE_COUNT);
      return SUCCESS;
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
    case LD_H_HL: case LD_L_HL:
      doRegisterIndirectToRegister(instruction);
      sleepCycles(LD_MEM_INDIRECT_REGISTER_CYCLE_COUNT);
      return SUCCESS;
    // Register to register-indirect memory
    case LD_HL_A: case LD_HL_B: case LD_HL_C: case LD_HL_D: case LD_HL_E:
    case LD_HL_H: case LD_HL_L:
      doRegisterToRegisterIndirect(instruction);
      sleepCycles(LD_MEM_INDIRECT_REGISTER_CYCLE_COUNT);
      return SUCCESS;
    // Immediate to register indirect
    case LD_HL_d8:
      doImmediateIndirect();
      sleepCycles(LD_IMM_TO_INDIRECT_REGISTER_CYCLE_COUNT);
      return SUCCESS;
    // HALT instruction: power off cpu until interrupt occurs
    case HALT:
      doHalt();
      sleepCycles(HALT_CYCLE_COUNT);
      return SUCCESS;
    default:
      return SUCCESS;
  }
}
