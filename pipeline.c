#include<stdlib.h>
#include<stdio.h>
#include"cpu.h"

Inst* fetch(CPU *cpu) {
  Inst *inst = (Inst*)malloc(sizeof(Inst));
  inst->inst = 0;
  u32 i = 0;
  for(i = 0; i < 4; i++) {
    inst->inst |= cpu->mem[cpu->pc] << (i*8);
    cpu->pc++;
  }
  return inst;
}

// ld, st, add, sub, mul, div, nop
void decode(CPU *cpu, Inst *inst) {
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
  }
}

void execute(Inst *inst) {
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
  }
}

int mem_access() {
}

int writeback(CPU *cpu, Inst *inst) {
  switch(inst->type) {
    case R_TYPE: {
      cpu->reg[inst->rd] = inst->result;
      break;
    }
  }
  free(inst);
}

