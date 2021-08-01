#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"cpu.h"

void printreg(CPU *cpu) {
  int i = 0;
  int j = 0;
  printf("pc:%08x\n", cpu->pc);
  for(i = 0; i < 8; i++) {
    for(j = 0; j < 4; j++) {
      printf("x%2d:%08x, %8d\t", i*4+j, cpu->reg[i*4+j], cpu->reg[i*4+j]);
    }
    printf("\n");
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

pthread_mutex_t memory_mutex;
i32 m_first = 1;

u8 memread(CPU *cpu, u32 addr) {
  if(m_first) {
    pthread_mutex_init(&memory_mutex, NULL);
    m_first = 0;
  }

  if(addr < MEM_SIZE) {
    pthread_mutex_lock(&memory_mutex);
    u8 res = cpu->mem[addr];
    pthread_mutex_unlock(&memory_mutex);
    return res;
  } else if(addr == (UART | 0x004)) {
    // rxdata
    char c = getc(stdin);
    return c;
  } else {
    fprintf(stderr, "[%08x]:memread メモリ範囲外参照です。MEM_SIZE=%08xです。\n", addr, MEM_SIZE);
    printreg(cpu);
    exit(1);
  }
}

void memwrite(CPU *cpu, u32 addr, u8 data) {
  if(m_first) {
    pthread_mutex_init(&memory_mutex, NULL);
    m_first = 0;
  }

  if(addr < MEM_SIZE) {
    // memory access
    pthread_mutex_lock(&memory_mutex);
    cpu->mem[addr] = data;
    pthread_mutex_unlock(&memory_mutex);
  } else if(addr == (UART | 0x000)) {
    // uart accsess
    // txdata
    printf("%c", data);
    fflush(stdout);
  } else {
    fprintf(stderr, "[%08x]:memwrite メモリ範囲外参照です。MEM_SIZE=%08xです。\n", addr, MEM_SIZE);
    printreg(cpu);
    exit(1);
  }
}
