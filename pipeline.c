#include<stdlib.h>
#include<stdio.h>
#include"cpu.h"

u32 sign_extend(u32 imm, u8 len) {
  u32 mask = 0xFFFFFFFF << (len-1);
  u32 res = 0;
  if(imm & (0x01 << (len-1))) {
    res = imm | mask;
  } else {
    res = imm & ~mask;
  }
  return res;
}

void fetch(CPU *cpu) {
  Inst *inst = (Inst*)calloc(1, sizeof(Inst));
  inst->inst = 0;
  u32 i = 0;
  for(i = 0; i < 4; i++) {
    inst->inst |= memread(cpu, cpu->pc) << (i*8);
    cpu->pc++;
  }
  cpu->ifid_reg.inst = inst;
}

// ld, st, add, sub, mul, div, nop
void decode(CPU *cpu) {
  Inst *inst = cpu->ifid_reg.inst;
  u32 ins = inst->inst;
  inst->opecode = OPECODE(ins);
  switch(inst->opecode) {
    case OPE_R: {
      inst->rd = RD(ins);
      inst->rs1 = cpu->reg[RS1(ins)];
      inst->rs2 = cpu->reg[RS2(ins)];
      inst->funct3 = FUNCT3(ins);
      inst->funct7 = FUNCT7(ins);
      inst->type = R_TYPE;
      break;
    }
    case OPE_I_JA:
    case OPE_I_LD:
    case OPE_I_AL: {
      inst->rd = RD(ins);
      inst->rs1 = cpu->reg[RS1(ins)];
      inst->funct3 = FUNCT3(ins);
      inst->imm = IMM_I(ins);
      inst->type = I_TYPE;
      break;
    }
    case OPE_S: {
      inst->imm = IMM_S(ins);
      inst->funct3 = FUNCT3(ins);
      inst->rs1 = cpu->reg[RS1(ins)];
      inst->rs2 = cpu->reg[RS2(ins)];
      inst->type = S_TYPE;
      break;
    }
    case OPE_J: {
      inst->imm = IMM_J(ins);
      inst->rd = RD(ins);
      break;
    }
    case OPE_B: {
      inst->imm = IMM_B(ins);
      inst->rs2 = cpu->reg[RS2(ins)];
      inst->rs1 = cpu->reg[RS1(ins)];
      inst->funct3 = FUNCT3(ins);
      break;
    }
    case OPE_U_LUI: {
      inst->imm = IMM_U(ins);
      inst->rd = RD(ins);
      break;
    }
    case OPE_U_AUI: {
      inst->imm = IMM_U(ins);
      inst->rd = RD(ins);
      break;
    }
    default: {
      fprintf(stderr, "invalid instruction@%08x:%08x\n", cpu->pc, ins);
      exit(1);
      break;
    }
  }
  cpu->idex_reg.inst = inst;
}

void execute(CPU *cpu) {
  Inst *inst = cpu->idex_reg.inst;
  switch(inst->opecode) {
    case OPE_R: {
      switch(inst->funct3) {
        case 0x00: {  // ADD SUB
          if(inst->funct7 == 0) { // ADD
            inst->result = inst->rs1 + inst->rs2;
          } else {                // SUB
            inst->result = inst->rs1 - inst->rs2;
          }
          break;
        }
      }
      break;
    }
    case OPE_I_AL: {
      switch(inst->funct3) {
        case 0x00: {  // ADDI
          inst->result = inst->rs1 + sign_extend(inst->imm, 12);
          break;
        }
      }
      break;
    }
    case OPE_I_LD: {  // アドレス計算
      inst->result = inst->rs1 + sign_extend(inst->imm, 12);
      break;
    }
    case OPE_I_JA: {  // ジャンプ
      switch(inst->funct3) {
        case 0x00: {  // JALR
          inst->result = cpu->reg[inst->rs1] + sign_extend(inst->imm, 12)-4;
          inst->result &= ~0x01;
          break;
        }
      }
    }
    case OPE_S: {
      inst->result = inst->rs1 + sign_extend(inst->imm, 12);
      break;
    }
    case OPE_J: {
      inst->result = sign_extend(inst->imm, 20)-4;
      break;
    }
    case OPE_B: {
      switch(inst->funct3) {
        case 0x00: {  // beq
          if(inst->rs1 == inst->rs2) inst->result = sign_extend(inst->imm, 12)-4;
          else inst->result = 0;
          break;
        }
        case 0x01: {  // bne
          if(inst->rs1 != inst->rs2) inst->result = sign_extend(inst->imm, 12)-4;
          else inst->result = 0;
          break;
        }
        case 0x04: {  // blt
          if((i32)inst->rs1 < (i32)inst->rs2)
            inst->result = sign_extend(inst->imm, 12)-4;
          else inst->result = 0;
          break;
        }
        case 0x05: {  // bge
          if((i32)inst->rs1 >= (i32)inst->rs2)
            inst->result = sign_extend(inst->imm, 12)-4;
          else inst->result = 0;
          break;
        }
        case 0x06: {  // bltu
          if(inst->rs1 < inst->rs2) inst->result = sign_extend(inst->imm, 12)-4;
          else inst->result = 0;
          break;
        }
        case 0x07: {  // bgeu
          if(inst->rs1 >= inst->rs2) inst->result = sign_extend(inst->imm, 12)-4;
          else inst->result = 0;
          break;
        }
      }
      break;
    }
    case OPE_U_LUI: {
      inst->result = sign_extend(inst->imm, 20) << 12;
      break;
    }
    case OPE_U_AUI: {
      inst->result = sign_extend(inst->imm, 20);
      break;
    }
    default: {
      break;
    }
  }
  cpu->exmem_reg.inst = inst;
}

void mem_access(CPU *cpu) {
  Inst *inst = cpu->exmem_reg.inst;
  switch(inst->opecode) {
    case OPE_I_LD: {
      u32 addr = inst->result;
      inst->result = 0;
      switch(inst->funct3) {
        case 0x00: {  // lb
          inst->result |= memread(cpu, addr);
          inst->result = sign_extend(inst->result, 8);
          break;
        }
        case 0x01: {  // lh
          inst->result |= memread(cpu, addr);
          inst->result |= memread(cpu, addr+1) << 8;
          inst->result = sign_extend(inst->result, 16);
          break;
        }
        case 0x02: { // lw
          inst->result |= memread(cpu, addr);
          inst->result |= memread(cpu, addr+1) << 8;
          inst->result |= memread(cpu, addr+2) << 16;
          inst->result |= memread(cpu, addr+3) << 24;
          break;
        }

        case 0x05: {  // lhu
          inst->result |= memread(cpu, addr+1) << 8;
        }
        case 0x04: {  // lbu
          inst->result |= memread(cpu, addr);
          break;
        }
      }
      break;
    }
    case OPE_S: {
      u32 addr = inst->result;
      switch(inst->funct3) {
        case 0x02: {  // sw
          memwrite(cpu, addr+3, inst->rs2 >> 24);
          memwrite(cpu, addr+2, inst->rs2 >> 16);
        }
        case 0x01: {  // sh
          memwrite(cpu, addr+1, inst->rs2 >> 8);
        }
        case 0x00: {  // sb
          memwrite(cpu, addr, inst->rs2);
          break;
        }
      }
      break;
    }
  }
  cpu->memwb_reg.inst = inst;
}

void writeback(CPU *cpu) {
  Inst *inst = cpu->memwb_reg.inst;
  switch(inst->opecode) {
    case OPE_R: {
      cpu->reg[inst->rd] = inst->result;
      break;
    }
    case OPE_I_LD:
    case OPE_I_AL: {
      cpu->reg[inst->rd] = inst->result;
      break;
    }
    case OPE_I_JA: {  // ジャンプ
      u32 tmp = cpu->pc;
      cpu->pc = inst->result;
      cpu->reg[inst->rd] = tmp;
      break;
    }
    case OPE_J: {
      cpu->reg[inst->rd] = cpu->pc;
      cpu->pc += inst->result;
      break;
    }
    case OPE_B: {
      cpu->pc += inst->result;
      break;
    }
    case OPE_U_LUI: {
      cpu->reg[inst->rd] = inst->result;
      break;
    }
    case OPE_U_AUI: {
      cpu->pc += inst->result - 4;
      break;
    }
  }
  cpu->reg[0] = 0;
  free(inst);
}

