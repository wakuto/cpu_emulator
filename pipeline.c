#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
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

u32 right_shift(u32 imm, u32 shamt, u32 is_althmetic) {
  u32 i = 0;
  if(!is_althmetic) { // logical shift
    for(i = 0; i < shamt; i++) {
      imm = (imm >> 1) & 0x7FFFFFFF;  // 右へシフトしてmsbを0にセット
    }
  } else {
    u32 sign = imm & 0x80000000; // msbを取り出す
    for(i = 0; i < shamt; i++) {
      imm = ((imm >> 1) & 0x7FFFFFFF) | sign;
    }
  }
  return imm;
}

pthread_mutex_t ifid_reg_mutex;
pthread_mutex_t idex_reg_mutex;
pthread_mutex_t exmem_reg_mutex;
pthread_mutex_t memwb_reg_mutex;

void mutex_init(void) {
  pthread_mutex_init(&ifid_reg_mutex, NULL);
  pthread_mutex_init(&idex_reg_mutex, NULL);
  pthread_mutex_init(&exmem_reg_mutex, NULL);
  pthread_mutex_init(&memwb_reg_mutex, NULL);
}

void fetch(CPU *cpu) {
  Inst *inst = (Inst*)calloc(1, sizeof(Inst));
  inst->inst = 0;
  u32 i = 0;
  for(i = 0; i < 4; i++) {
    inst->inst |= memread(cpu, cpu->pc+i) << (i*8);
  }
  pthread_mutex_lock(&ifid_reg_mutex);
  cpu->ifid_reg.inst = inst;
  pthread_mutex_unlock(&ifid_reg_mutex);
}

// ld, st, add, sub, mul, div, nop
void decode(CPU *cpu) {
  pthread_mutex_lock(&ifid_reg_mutex);
  Inst *inst = cpu->ifid_reg.inst;
  pthread_mutex_unlock(&ifid_reg_mutex);
  u32 ins = inst->inst;
  inst->opecode = OPECODE(ins);

  switch(inst->opecode) {
    case OPE_R: {
      inst->rd = RD(ins);
      inst->rs1 = cpu->reg[RS1(ins)];
      inst->rs2 = cpu->reg[RS2(ins)];
      inst->funct3 = FUNCT3(ins);
      inst->funct7 = FUNCT7(ins);
      break;
    }
    case OPE_I_JA:
    case OPE_I_LD:
    case OPE_I_AL: {
      inst->rd = RD(ins);
      inst->rs1 = cpu->reg[RS1(ins)];
      inst->funct3 = FUNCT3(ins);
      inst->imm = IMM_I(ins);
      inst->shamt = SHAMT(ins);
      inst->funct7 = FUNCT7(ins);
      break;
    }
    case OPE_S: {
      inst->imm = IMM_S(ins);
      inst->funct3 = FUNCT3(ins);
      inst->rs1 = cpu->reg[RS1(ins)];
      inst->rs2 = cpu->reg[RS2(ins)];
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

  pthread_mutex_lock(&idex_reg_mutex);
  cpu->idex_reg.inst = inst;
  pthread_mutex_unlock(&idex_reg_mutex);
}

void execute(CPU *cpu) {
  pthread_mutex_lock(&idex_reg_mutex);
  Inst *inst = cpu->idex_reg.inst;
  pthread_mutex_unlock(&idex_reg_mutex);
  u32 jump_flag = 0;
  switch(inst->opecode) {
    case OPE_R: {
      switch(inst->funct3) {
        case 0b000: { // ADD SUB
          if(inst->funct7 == 0) { // ADD
            inst->result = inst->rs1 + inst->rs2;
          } else {                // SUB
            inst->result = inst->rs1 - inst->rs2;
          }
          break;
        }
        case 0b001: { // SLL
          inst->result = inst->rs1 << inst->rs2;
          break;
        }
        case 0b010: { // SLT
          inst->result = (i32)inst->rs1 < (i32)inst->rs2 ? 1 : 0;
          break;
        }
        case 0b011: { // SLTU
          inst->result = (u32)inst->rs1 < (u32)inst->rs2 ? 1 : 0;
          break;
        }
        case 0b100: {  // XOR
          inst->result = inst->rs1 ^ inst->rs2;
          break;
        }
        case 0b101: { // srl & sra
          inst->result = right_shift(inst->rs1, inst->rs2, inst->funct7 == 0b0100000);
          break;
        }
        case 0b110: {  // OR
          inst->result = inst->rs1 | inst->rs2;
          break;
        }
        case 0b111: {  // AND
          inst->result = inst->rs1 & inst->rs2;
          break;
        }
      }
      break;
    }
    case OPE_I_AL: {
      u32 sext = sign_extend(inst->imm, 12);
      switch(inst->funct3) {
        case 0b000: {  // ADDI
          inst->result = inst->rs1 + sext;
          break;
        }
        case 0b010: {  // slti
          inst->result = (i32)inst->rs1 < (i32)sext ? 1 : 0;
          break;
        }
        case 0b011: {  // sltiu
          inst->result = (u32)inst->rs1 < (u32)sext ? 1 : 0;
          break;
        }
        case 0b100: {  // XORI
          inst->result = inst->rs1 ^ sext;
          break;
        }
        case 0b110: {  // ORI
          inst->result = inst->rs1 | sext;
          break;
        }
        case 0b111: {  // ANDI
          inst->result = inst->rs1 & sext;
          break;
        }
        case 0b001: { // slli
          inst->result = inst->rs1 << inst->shamt;
          break;
        }
        case 0b101: { // srli & srai
          inst->result = right_shift(inst->rs1, inst->shamt, inst->funct7 == 0b0100000);
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
          inst->result = cpu->reg[inst->rs1] + sign_extend(inst->imm, 12);
          inst->result &= ~0x01;
          jump_flag = 1;
          break;
        }
      }
    }
    case OPE_S: {
      inst->result = inst->rs1 + sign_extend(inst->imm, 12);
      break;
    }
    case OPE_J: {
      inst->result = sign_extend(inst->imm, 20);
      jump_flag = 1;
      break;
    }
    case OPE_B: {
      switch(inst->funct3) {
        case 0x00: {  // beq
          if(inst->rs1 == inst->rs2) {
            inst->result = sign_extend(inst->imm, 12);
            jump_flag = 1;
          } else inst->result = 0;
          break;
        }
        case 0x01: {  // bne
          if(inst->rs1 != inst->rs2) {
            inst->result = sign_extend(inst->imm, 12);
            jump_flag = 1;
          } else inst->result = 0;
          break;
        }
        case 0x04: {  // blt
          if((i32)inst->rs1 < (i32)inst->rs2) {
            inst->result = sign_extend(inst->imm, 12);
            jump_flag = 1;
          } else inst->result = 0;
          break;
        }
        case 0x05: {  // bge
          if((i32)inst->rs1 >= (i32)inst->rs2) {
            inst->result = sign_extend(inst->imm, 12);
            jump_flag = 1;
          } else inst->result = 0;
          break;
        }
        case 0x06: {  // bltu
          if(inst->rs1 < inst->rs2) {
            inst->result = sign_extend(inst->imm, 12);
            jump_flag = 1;
          } else inst->result = 0;
          break;
        }
        case 0x07: {  // bgeu
          if(inst->rs1 >= inst->rs2) {
            inst->result = sign_extend(inst->imm, 12);
            jump_flag = 1;
          } else inst->result = 0;
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
      jump_flag = 1;
      break;
    }
    default: {
      break;
    }
  }
  pthread_mutex_lock(&exmem_reg_mutex);
  cpu->exmem_reg.inst = inst;
  cpu->exmem_reg.jump_flag = jump_flag;
  pthread_mutex_unlock(&exmem_reg_mutex);
}

void mem_access(CPU *cpu) {
  pthread_mutex_lock(&exmem_reg_mutex);
  Inst *inst = cpu->exmem_reg.inst;
  u32 jump_flag = cpu->exmem_reg.jump_flag;
  pthread_mutex_unlock(&exmem_reg_mutex);
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
          inst->result |= memread(cpu, addr+3) << 24;
          inst->result |= memread(cpu, addr+2) << 16;
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
  pthread_mutex_lock(&memwb_reg_mutex);
  cpu->memwb_reg.inst = inst;
  cpu->memwb_reg.jump_flag = jump_flag;
  pthread_mutex_unlock(&memwb_reg_mutex);
}

void writeback(CPU *cpu) {
  pthread_mutex_lock(&memwb_reg_mutex);
  Inst *inst = cpu->memwb_reg.inst;
  u32 jump_flag = cpu->memwb_reg.jump_flag;
  pthread_mutex_unlock(&memwb_reg_mutex);
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
      u32 tmp = cpu->pc + 4;
      cpu->pc = inst->result;
      cpu->reg[inst->rd] = tmp;
      break;
    }
    case OPE_J: {
      cpu->reg[inst->rd] = cpu->pc + 4;
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
      cpu->pc += inst->result;
      break;
    }
  }

  // プログラムカウンタを扱う命令以外 & 条件が成立しなかったジャンプ命令はpc=pc+4にする
  if(!jump_flag) {
    cpu->pc += 4;
  }
  cpu->reg[0] = 0;
  free(inst);
}

