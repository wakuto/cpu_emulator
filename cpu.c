#include<stdio.h>
#include<stdlib.h>
#include"cpu.h"

void printreg(CPU *cpu) {
  int i = 0;
  printf("pc:%08x\n", cpu->pc);
  for(i = 0; i < 16; i++) {
    printf("x%2d:%08x, %8d\t/\tx%2d:%08x, %8d\n", i, cpu->reg[i], cpu->reg[i], i+16, cpu->reg[i+16], cpu->reg[i+16]);
  }
}

void print_inst(Inst *inst) {
  printf("opecode: %02x\n", inst->opecode);
  printf("rd     : %02x, %u\n", inst->rd, inst->rd);
  printf("rs1    : %08x, %u\n", inst->rs1, inst->rs1);
  printf("rs2    : %08x, %u\n", inst->rs2, inst->rs2);
  printf("imm    : %04x, %u\n", inst->imm, inst->imm);
  printf("immext : %08x, %d\n", sign_extend(inst->imm, 12), sign_extend(inst->imm, 12));
  printf("funct3 : %08x, %u\n", inst->funct3, inst->funct3);
  printf("funct7 : %08x, %u\n", inst->funct7, inst->funct7);
  printf("result : %08x, %u\n", inst->result, inst->result);
}

u8 memread(CPU *cpu, u32 addr) {
  // mmio
  if(addr == MMIO) {
  }
  if(addr >= MEM_SIZE) {
    fprintf(stderr, "[%08x]:メモリ範囲外参照です\n", addr);
    exit(1);
  }
  return cpu->mem[addr];
}

void memwrite(CPU *cpu, u32 addr, u8 data) {
  if(addr == MMIO) {
    printf("%c", data);
    return;
  }
  if(addr >= MEM_SIZE) {
    fprintf(stderr, "[%08x]:メモリ範囲外参照です\n", addr);
    exit(1);
  }
  cpu->mem[addr] = data;
}
