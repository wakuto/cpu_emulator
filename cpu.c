#include<stdio.h>
#include"cpu.h"

void printreg(CPU *cpu) {
  int i = 0;
  for(i = 0; i < 32; i++) {
    printf("x%d:%08x\n", i, cpu->reg[i]);
  }
}

void print_inst(Inst *inst) {
  printf("opecode: %02x\n", inst->opecode);
  printf("rd     : %02x, %d\n", inst->rd, inst->rd);
  printf("rs1    : %08x, %d\n", inst->rs1, inst->rs1);
  printf("rs2    : %08x, %d\n", inst->rs2, inst->rs2);
  printf("imm    : %04x, %d\n", inst->imm, inst->imm);
  printf("result : %08x, %d\n", inst->result, inst->result);
}
