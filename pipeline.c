#include<stdlib.h>
#include<stdio.h>
#include"cpu.h"

u32 sign_extend(u16 imm, u8 len) {
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
    inst->inst |= cpu->mem[cpu->pc] << (i*8);
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
    case OPE_S: {
      inst->result = inst->rs1 + sign_extend(inst->imm, 12);
      break;
    }
  }
  cpu->exmem_reg.inst = inst;
}

int mem_access(CPU *cpu) {
  Inst *inst = cpu->exmem_reg.inst;
  switch(inst->opecode) {
    case OPE_I_LD: {
      u32 addr = inst->result;
      inst->result = 0;
      switch(inst->funct3) {
        case 0x00: {  // lb
          inst->result |= cpu->mem[addr];
          inst->result = sign_extend(inst->result, 8);
          break;
        }
        case 0x01: {  // lh
          inst->result |= cpu->mem[addr];
          inst->result |= cpu->mem[addr+1] << 8;
          inst->result = sign_extend(inst->result, 16);
          break;
        }
        case 0x02: { // lw
          inst->result |= cpu->mem[addr];
          inst->result |= cpu->mem[addr+1] << 8;
          inst->result |= cpu->mem[addr+2] << 16;
          inst->result |= cpu->mem[addr+3] << 24;
          break;
        }

        case 0x05: {  // lhu
          inst->result |= cpu->mem[addr+1] << 8;
        }
        case 0x04: {  // lbu
          inst->result |= cpu->mem[addr];
          break;
        }
      }
      break;
    }
    case OPE_S: {
      u32 addr = inst->result;
      switch(inst->funct3) {
        case 0x02: {  // sw
          cpu->mem[addr+3] = inst->rs2 >> 24;
          cpu->mem[addr+2] = inst->rs2 >> 16;
        }
        case 0x01: {  // sh
          cpu->mem[addr+1] = inst->rs2 >> 8;
        }
        case 0x00: {  // sb
          cpu->mem[addr] = inst->rs2;
          break;
        }
      }
      break;
    }
  }
  cpu->memwb_reg.inst = inst;
}

int writeback(CPU *cpu) {
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
  }
  free(inst);
}

