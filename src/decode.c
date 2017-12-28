
#include "decode.h"
#include "cpu.h"
#include "opcode.h"
#include "execute.h"
#include <stdlib.h>
#include <stdio.h>

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
      sleepCycles(4);
      return SUCCESS;
    // HALT instruction: power off cpu until interrupt occurs
    case HALT:
      doHalt();
      sleepCycles(4);
      return SUCCESS;
    case CB:
      pc += 1;
      instruction = readMemory(pc);
      return decodeAndExecuteCB(instruction);
    case DAA:
      daa();
      sleepCycles(4);
      return SUCCESS;
    case CPL:
      cpl();
      sleepCycles(4);
      return SUCCESS;
    case CCF:
      ccf();
      sleepCycles(4);
      return SUCCESS;
    case SCF:
      scf();
      sleepCycles(4);
      return SUCCESS;
    case STOP:
      stop();
      sleepCycles(4);
      return SUCCESS;
    case DI:
      di();
      sleepCycles(4);
      return SUCCESS;
    case EI:
      ei();
      sleepCycles(4);
      return SUCCESS;
    case JP:
      jumpImm();
      sleepCycles(16);
      return SUCCESS;
    case JPNZ:
      jumpImmNZ();
      return SUCCESS;
    case JPZ:
      jumpImmZ();
      return SUCCESS;
    case JPNC:
      jumpImmNC();
      return SUCCESS;
    case JPC:
      jumpImmC();
      return SUCCESS;
    case JPHL:
      jumpHL();
      sleepCycles(4);
      return SUCCESS;
    case JR:
      jr();
      sleepCycles(12);
      return SUCCESS;
    case JRNZ:
      jrNZ();
      return SUCCESS;
    case JRZ:
      jrZ();
      return SUCCESS;
    case JRNC:
      jrNC();
      return SUCCESS;
    case JRC:
      jrC();
      return SUCCESS;
    case CALL:
      call();
      sleepCycles(24);
      return SUCCESS;
    case CALLNZ:
      callNZ();
      return SUCCESS;
    case CALLC:
      callC();
      return SUCCESS;
    case CALLNC:
      callNC();
      return SUCCESS;
    case CALLZ:
      callZ();
      return SUCCESS;
    case RET:
      ret();
      sleepCycles(16);
      return SUCCESS;
    case RETZ:
      retZ();
      return SUCCESS;
    case RETC:
      retC();
      return SUCCESS;
    case RETNC:
      retNC();
      return SUCCESS;
    case RETNZ:
      retNZ();
      return SUCCESS;
    case RETI:
      reti();
      sleepCycles(16);
      return SUCCESS;
    case RST_0: case RST_8: case RST_10: case RST_18: case RST_20:
    case RST_28: case RST_30: case RST_38:
      rst(instruction);
      sleepCycles(16);
      return SUCCESS;
    ////////////////////////////////////////////////////////////////////////////
    // 8-bit loads
    // REGISTER-IMMEDIATE LOADS WITH 8-BIT IMMEDIATE ARGUMENT
    case LD_B_d8: case LD_D_d8: case LD_H_d8: case LD_C_d8:
    case LD_E_d8: case LD_L_d8: case LD_A_d8:
      doLoad8bitImmediate(instruction);
      sleepCycles(8);
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
      sleepCycles(4);
      return SUCCESS;
    // Register-indirect-memory to register
    case LD_A_HL: case LD_B_HL: case LD_C_HL: case LD_D_HL: case LD_E_HL:
    case LD_H_HL: case LD_L_HL: case LD_A_BC: case LD_A_DE:
      doRegisterIndirectToRegister(instruction);
      sleepCycles(8);
      return SUCCESS;
    // Register to register-indirect memory
    case LD_HL_A: case LD_HL_B: case LD_HL_C: case LD_HL_D: case LD_HL_E:
    case LD_HL_H: case LD_HL_L: case LD_BC_A: case LD_DE_A:
      doRegisterToRegisterIndirect(instruction);
      sleepCycles(8);
      return SUCCESS;
    // Memory address direct to accumulator
    case LD_A_a16p:
      doAddrToAcc();
      sleepCycles(16);
      return SUCCESS;
    // Accumulator to memory address
    case LD_a16p_A:
      doAccToAddr();
      sleepCycles(16);
      return SUCCESS;
    // Immediate to register indirect
    case LD_HL_d8:
      doImmediateIndirect();
      sleepCycles(8);
      return SUCCESS;
    // Accumulutor - C
    case LD_A_C_INDIRECT:
      doACIndirect();
      sleepCycles(8);
      return SUCCESS;
    // C - Accumulator
    case LD_C_INDIRECT_A:
      doCIndirectA();
      sleepCycles(8);
      return SUCCESS;
    // (HL) dec to A
    case LD_DEC_A:
      doHLDtoA();
      sleepCycles(8);
      return SUCCESS;
    // A to (HL) dec
    case LD_A_DEC:
      doAtoHLD();
      sleepCycles(8);
      return SUCCESS;
    // (HL) inc to A
    case LD_INC_A:
      doHLItoA();
      sleepCycles(8);
      return SUCCESS;
    // A to (HL) inc
    case LD_A_INC:
      doAtoHLI();
      sleepCycles(8);
      return SUCCESS;
    // 8-bit direct address to A
    case LD_A_a8p:
      doAa8p();
      sleepCycles(12);
      return SUCCESS;
    // And from A
    case LD_a8p_A:
      doa8pA();
      sleepCycles(12);
      return SUCCESS;
    ////////////////////////////////////////////////////////////////////////////
    // 16 bit loads
    // Combined register immediate
    case LD_BC_d16: case LD_DE_d16: case LD_HL_d16: case LD_SP_d16:
      doLoadReg16b(instruction);
      sleepCycles(12);
      return SUCCESS;
    // HL to stack pointer
    case LD_SP_HL:
      doLoadHLSP();
      sleepCycles(8);
      return SUCCESS;
    // SP + 8-bit into HL
    case LD_HL_SP_N:
      doLoadHLSPN();
      sleepCycles(12);
      return SUCCESS;
    // SP to address nn
    case LD_d16p_SP:
      doLoadSP16();
      sleepCycles(20);
      return SUCCESS;
    // Push reg onto stack
    case PUSH_AF: case PUSH_BC: case PUSH_DE: case PUSH_HL:
      doPush(instruction);
      sleepCycles(16);
      return SUCCESS;
    // Pop reg off stack
    case POP_AF: case POP_BC: case POP_DE: case POP_HL:
      doPop(instruction);
      sleepCycles(12);
      return SUCCESS;
    ////////////////////////////////////////////////////////////////////////////
    // Arithmetic instructions
    // ADD A, without carry
    case ADD_AA: case ADD_AB: case ADD_AC: case ADD_AD: case ADD_AE:
    case ADD_AH: case ADD_AL: case ADD_A_IND: case ADD_A_d8:
      add(instruction);
      if (instruction != ADD_A_IND && instruction != ADD_A_d8)
        sleepCycles(4);
      else
        sleepCycles(8);
      return SUCCESS;
    // ADD A, with carry
    case ADC_AA: case ADC_AB: case ADC_AC: case ADC_AD: case ADC_AE: 
    case ADC_AH: case ADC_AL: case ADC_A_IND: case ADC_A_d8:
      adc(instruction);
      if (instruction != ADC_A_IND && instruction != ADC_A_d8)
        sleepCycles(4);
      else
        sleepCycles(8);
      return SUCCESS;
    // SUB A, without carry
    case SUB_AA: case SUB_AB: case SUB_AC: case SUB_AD: case SUB_AE:
    case SUB_AH: case SUB_AL: case SUB_A_IND: case SUB_A_d8:
      sub(instruction);
      if (instruction != SUB_A_IND && instruction != SUB_A_d8)
        sleepCycles(4);
      else
        sleepCycles(8);
      return SUCCESS;
    // SBC: Sub with carry
    case SBC_AA: case SBC_AB: case SBC_AC: case SBC_AD: case SBC_AE:
    case SBC_AH: case SBC_AL: case SBC_A_IND: case SBC_A_d8:
      sbc(instruction);
      if (instruction != SBC_A_IND)
        sleepCycles(4);
      else
        sleepCycles(8);
      return SUCCESS;
    // AND: Logical AND
    case AND_AA: case AND_AB: case AND_AC: case AND_AD: case AND_AE: 
    case AND_AH: case AND_AL: case AND_A_IND: case AND_A_d8:
      land(instruction);
      if (instruction != AND_A_IND && instruction != AND_A_d8)
        sleepCycles(4);
      else
        sleepCycles(8);
      return SUCCESS;
    // OR: logical OR
    case OR_AA: case OR_AB: case OR_AC: case OR_AD: case OR_AE: 
    case OR_AH: case OR_AL: case OR_A_IND: case OR_A_d8:
      lor(instruction);
      if (instruction != OR_A_IND && instruction != OR_A_d8)
        sleepCycles(4);
      else
        sleepCycles(8);
      return SUCCESS;
    // XOR: logical XOR
    case XOR_AA: case XOR_AB: case XOR_AC: case XOR_AD: case XOR_AE: 
    case XOR_AH: case XOR_AL: case XOR_A_IND: case XOR_A_d8:
      lxor(instruction);
      if (instruction != XOR_A_IND && instruction != XOR_A_d8)
        sleepCycles(4);
      else
        sleepCycles(8);
      return SUCCESS;
    // CP: Compare
    case CP_AA: case CP_AB: case CP_AC: case CP_AD: case CP_AE: 
    case CP_AH: case CP_AL: case CP_A_IND: case CP_A_d8:
      cp(instruction);
      if (instruction != CP_A_IND && instruction != CP_A_d8)
        sleepCycles(4);
      else
        sleepCycles(8);
      return SUCCESS;
    // INC: Increment register
    case INC_A: case INC_B: case INC_C: case INC_D: case INC_E: case INC_H:
    case INC_L: case INC_HL_IND:
      inc(instruction);
      if (instruction != INC_HL_IND)
        sleepCycles(8);
      else
        sleepCycles(12);
      return SUCCESS;
    // DEC: Decrement a register
    case DEC_A: case DEC_B: case DEC_C: case DEC_D: case DEC_E: case DEC_H:
    case DEC_L: case DEC_HL_IND:
      dec(instruction);
      if (instruction != DEC_HL_IND)
        sleepCycles(8);
      else
        sleepCycles(12);
      return SUCCESS;
    case RLCA:
      rlc(&a_reg, 0);
      flags &= 0x10;
      sleepCycles(4);
      return SUCCESS;
    case RLA:
      rl(&a_reg, 0);
      flags &= 0x10;
      sleepCycles(4);
      return SUCCESS;
    case RRCA:
      rrc(&a_reg, 0);
      flags &= 0x10;
      sleepCycles(4);
      return SUCCESS;
    case RRA:
      rr(&a_reg, 0);
      flags &= 0x10;
      sleepCycles(4);
      return SUCCESS;
    ////////////////////////////////////////////////////////////////////////////
    // 16-bit ALU
    // 16-bit ADD
    case ADD_BC: case ADD_DE: case ADD_HL: case ADD_SP:
      add16(instruction);
      sleepCycles(8);
      return SUCCESS;
    // Add immediate to SP
    case ADD_SP_d8:
      addSPN();
      sleepCycles(16);
      return SUCCESS;
    // 16-bit incs
    case INC_BC: case INC_DE: case INC_HL: case INC_SP:
      inc16(instruction);
      sleepCycles(8);
      return SUCCESS;
    // 16-bit decs
    case DEC_BC: case DEC_DE: case DEC_HL: case DEC_SP:
      dec16(instruction);
      sleepCycles(8);
      return SUCCESS;
    default:
      printf("ILLEGAL OPCODE, MAY BE BUG IN EMULATOR!\n");
      return -1;
  }
}

int decodeAndExecuteCB(uint8_t instruction)
{
  // Determine registers
  uint8_t lower_nibble = (instruction & 0xf);
  uint8_t* arg = NULL;
  uint8_t bit_no;
  uint16_t addr = 0;
  switch(lower_nibble)
  {
    case 0x0: case 0x8:
      arg = &b_reg;
      break;
    case 0x1: case 0x9:
      arg = &c_reg;
      break;
    case 0x2: case 0xa:
      arg = &d_reg;
      break;
    case 0x3: case 0xb:
      arg = &e_reg;
      break;
    case 0x4: case 0xc:
      arg = &h_reg;
      break;
    case 0x5: case 0xd:
      arg = &l_reg;
      break;
    case 0x6: case 0xe:
      addr = (((uint16_t) h_reg) << 8) | l_reg;
      arg = NULL;
      break;
    case 0x7: case 0xf:
      arg = &a_reg;
      break;
  }
  // Determine Opcode
  if (instruction >= SWAP_LOWER && instruction <= SWAP_UPPER)
  {
    doSwap(arg, addr);
    if (instruction == SWAP_INDIR)
      sleepCycles(SWAP_CYCLES_ADDR);
    else
      sleepCycles(SWAP_CYCLES_REG);
    return SUCCESS;
  }
  else if (instruction >= RLC_LOWER && instruction <= RLC_UPPER)
  {
    rlc(arg, addr);
    if (instruction == RLC_INDIR)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
    return SUCCESS;
  }
  else if (instruction >= RL_LOWER && instruction <= RL_UPPER)
  {
    rl(arg, addr);
    if (instruction == RL_INDIR)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
    return SUCCESS;
  }
  else if (instruction >= RRC_LOWER && instruction <= RRC_UPPER)
  {
    rrc(arg, addr);
    if (instruction == RRC_INDIR)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
    return SUCCESS;
  }
  else if (instruction >= RR_LOWER && instruction <= RR_UPPER)
  {
    rr(arg, addr);
    if (instruction == RR_INDIR)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
    return SUCCESS;
  }
  else if (instruction >= SLA_LOWER && instruction <= SLA_UPPER)
  {
    sla(arg, addr);
    if (instruction == SLA_INDIR)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
    return SUCCESS;
  }
  else if (instruction >= SRA_LOWER && instruction <= SRA_UPPER)
  {
    sra(arg, addr);
    if (instruction == SRA_INDIR)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
    return SUCCESS;
  }
  else if (instruction >= SRL_LOWER && instruction <= SRL_UPPER)
  {
    srl(arg, addr);
    if (instruction == SRL_INDIR)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
    return SUCCESS;
  }
  else if (instruction >= BIT_LOWER && instruction <= BIT_UPPER)
  {
    bit_no = (((instruction - BIT_LOWER)) / 8);
    bit(arg, addr, bit_no);
    if (arg == NULL)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
  }
  else if (instruction >= RES_LOWER && instruction <= RES_UPPER)
  {
    bit_no = (((instruction - RES_LOWER)) / 8);
    res(arg, addr, bit_no);
    if (arg == NULL)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
  }
  else if (instruction >= SET_LOWER && instruction <= SET_UPPER)
  {
    bit_no = (((instruction - SET_LOWER)) / 8);
    set(arg, addr, bit_no);
    if (arg == NULL)
      sleepCycles(CB_CYCLES_ADDR);
    else
      sleepCycles(CB_CYCLES_REG);
  }
  return -1;
}
