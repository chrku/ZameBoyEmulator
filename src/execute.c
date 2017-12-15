
#include "opcode.h"
#include "execute.h"
#include "cpu.h"

void do_load_8bit_immediate(uint8_t instruction)
{
  uint8_t immediate = *(&pc + 1);
  // Write to the correct register
  uint8_t* reg;
  switch(instruction)
  {
    case LD_B_d8:
      reg = &b_reg;
      break;
    case LD_D_d8:
      reg = &d_reg;
      break;
    case LD_H_d8:
      reg = &h_reg;
      break;
    case LD_C_d8:
      reg = &c_reg;
      break;
    case LD_E_d8:
      reg = &e_reg;
      break;
    case LD_L_d8:
      reg = &l_reg;
      break;
    case LD_A_d8:
      reg = &a_reg;
      break;
  }
  *reg = immediate;
  pc += LD_REGISTER_IMMEDIATE_LEN;
}
