
#include "opcode.h"
#include "execute.h"
#include "cpu.h"
#include <stdio.h>

void doLoad8bitImmediate(uint8_t instruction)
{
  uint8_t immediate = readMemory(pc + 1);
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

void doNop()
{
  // Only increment PC
  pc += NOP_ARGLEN;
}

void doHalt()
{
  // Increment the PC
  pc += HALT_ARGLEN;
  // TODO: Implement proper halting (will need interrupts first!)
  printf("HALT: q to quit\n");
  char c = getchar();
  if (c == 'q')
    program_state = OFF;
}

void doLoadRegisterRegister(uint8_t instruction)
{
  // Decode the instruction and move the data between the registers
  switch(instruction)
  {
    case LD_A_A:
      a_reg = a_reg;
      break;
    case LD_A_B:
      a_reg = b_reg;
      break;
    case LD_A_C:
      a_reg = c_reg;
      break;
    case LD_A_D:
      a_reg = d_reg;
      break;
    case LD_A_E:
      a_reg = e_reg;
      break;
    case LD_A_H:
      a_reg = h_reg;
      break;
    case LD_A_L:
      a_reg = l_reg;
      break;
    case LD_B_A:
      b_reg = a_reg;
      break;
    case LD_B_B:
      b_reg = b_reg;
      break;
    case LD_B_C:
      b_reg = c_reg;
      break;
    case LD_B_D:
      b_reg = d_reg;
      break;
    case LD_B_E:
      b_reg = e_reg;
      break;
    case LD_B_H:
      b_reg = h_reg;
      break;
    case LD_B_L:
      b_reg = l_reg;
      break;
    case LD_C_A:
      c_reg = a_reg;
      break;
    case LD_C_B:
      c_reg = b_reg;
      break;
    case LD_C_C:
      c_reg = c_reg;
      break;
    case LD_C_D:
      c_reg = d_reg;
      break;
    case LD_C_E:
      c_reg = e_reg;
      break;
    case LD_C_H:
      c_reg = h_reg;
      break;
    case LD_C_L:
      c_reg = l_reg;
      break;
    case LD_D_A:
      d_reg = a_reg;
      break;
    case LD_D_B:
      d_reg = b_reg;
      break;
    case LD_D_C:
      d_reg = c_reg;
      break;
    case LD_D_D:
      d_reg = d_reg;
      break;
    case LD_D_E:
      d_reg = e_reg;
      break;
    case LD_D_H:
      d_reg = h_reg;
      break;
    case LD_D_L:
      d_reg = l_reg;
      break;
    case LD_E_A:
      e_reg = a_reg;
      break;
    case LD_E_B:
      e_reg = b_reg;
      break;
    case LD_E_C:
      e_reg = c_reg;
      break;
    case LD_E_D:
      e_reg = d_reg;
      break;
    case LD_E_E:
      e_reg = e_reg;
      break;
    case LD_E_H:
      e_reg = h_reg;
      break;
    case LD_E_L:
      e_reg = l_reg;
      break;
    case LD_H_A:
      h_reg = a_reg;
      break;
    case LD_H_B:
      h_reg = b_reg;
      break;
    case LD_H_C:
      h_reg = c_reg;
      break;
    case LD_H_D:
      h_reg = d_reg;
      break;
    case LD_H_E:
      h_reg = e_reg;
      break;
    case LD_H_H:
      h_reg = h_reg;
      break;
    case LD_H_L:
      h_reg = l_reg;
      break;
    case LD_L_A:
      l_reg = a_reg;
      break;
    case LD_L_B:
      l_reg = b_reg;
      break;
    case LD_L_C:
      l_reg = c_reg;
      break;
    case LD_L_D: 
      l_reg = d_reg;
      break;
    case LD_L_E:
      l_reg = e_reg;
      break;
    case LD_L_H:
      l_reg = h_reg;
      break;
    case LD_L_L:
      l_reg = l_reg;
      break;
  }
  pc += LD_REGISTER_REGISTER_ARGLEN;
}

void doRegisterIndirectToRegister(uint8_t instruction)
{
  uint16_t addr = (((uint16_t) h_reg) << 8) | (l_reg);
  uint8_t value = readMemory(addr);
#ifdef DEBUG
  printf("Reading memory address %hx\n", addr);
#endif
  // Further decode instruction
  switch(instruction)
  {
    case LD_A_HL:
      a_reg = value;
      break;
    case LD_B_HL:
      b_reg = value;
      break;
    case LD_C_HL:
      c_reg = value;
      break;
    case LD_D_HL:
      d_reg = value;
      break;
    case LD_E_HL:
      e_reg = value;
      break;
    case LD_H_HL:
      h_reg = value;
      break;
    case LD_L_H:
      l_reg = value;
      break;
  }
  pc += LD_REG_INDIRECT_REGISTER_ARGLEN;
}

void doRegisterToRegisterIndirect(uint8_t instruction)
{
  uint16_t addr = (((uint16_t) h_reg) << 8) | (l_reg);
#ifdef DEBUG
  printf("Writing memory address %hx\n", addr);
#endif
  uint8_t value;
  // Further decode instruction
  switch(instruction)
  {
    case LD_HL_A:
      value = a_reg;
      break;
    case LD_HL_B:
      value = b_reg;
      break;
    case LD_HL_C:
      value = c_reg;
      break;
    case LD_HL_D:
      value = d_reg;
      break;
    case LD_HL_E:
      value = e_reg;
      break;
    case LD_HL_H:
      value = h_reg;
      break;
    case LD_HL_L:
      value = l_reg;
      break;
  }
  writeMemory(addr, value);
  pc += LD_REG_INDIRECT_REGISTER_ARGLEN;
}
