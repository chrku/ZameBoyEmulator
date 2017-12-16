
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
    // HALT instruction: power off cpu until interrupt occurs
    case HALT:
      doHalt();
      sleepCycles(HALT_CYCLE_COUNT);
      return SUCCESS;
    default:
      return SUCCESS;
  }
}
