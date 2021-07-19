#include<stdio.h>
#include"cpu.h"

u32 ext(u32 imm, u8 len) {
  u32 mask = 0xFFFFFFFF << (len-1);
  u32 res = 0;
  if(imm & (0x01 << (len-1))) {
    res = imm | mask;
  } else {
    res = imm & ~mask;
  }
  return res;
}

void printreg(CPU *cpu) {
  int i = 0;
  printf("pc:%08x\n", cpu->pc);
  for(i = 0; i < 32; i++) {
    printf("x%d:%08x, %d\n", i, cpu->reg[i], cpu->reg[i]);
  }
}

void print_inst(Inst *inst) {
  printf("opecode: %02x\n", inst->opecode);
  printf("rd     : %02x, %u\n", inst->rd, inst->rd);
  printf("rs1    : %08x, %u\n", inst->rs1, inst->rs1);
  printf("rs2    : %08x, %u\n", inst->rs2, inst->rs2);
  printf("imm    : %04x, %u\n", inst->imm, inst->imm);
  printf("immext : %08x, %d\n", ext(inst->imm, 12), ext(inst->imm, 12));
  printf("result : %08x, %u\n", inst->result, inst->result);
}
