#include<stdio.h>
#include<stdlib.h>
#include"cpu.h"

int main(void) {
  CPU *cpu = (CPU*)malloc(sizeof(CPU));
  int i = 0;
  int j = 0;
  printf("Hello RISC-V!\n");

  /*
  add x12, x10, x11
  sub x13, x12, x11
  */
  u32 memory[] = {0xB50633, 0x40B606B3};
  u32 inst_count = sizeof(memory)/sizeof(u32);
  // 命令の書き込み
  for(j = 0; j < inst_count; j++) {
    for(i = 0; i < 4; i++) {
      cpu->mem[i + j*4] = (u8)(memory[j] >> (i*8));
      printf("%02x", cpu->mem[i]);
    }
  }
  puts("");
  cpu->pc = 0;
  cpu->reg[10] = 5;
  cpu->reg[11] = 3;
  cpu->reg[12] = 0;

  printreg(cpu);

  for(j = 0; j < inst_count; j++) {
    puts("-------------------------");
    Inst *inst = fetch(cpu);

    decode(cpu, inst);
    print_inst(inst);

    execute(inst);

    mem_access();

    writeback(cpu, inst);

    printreg(cpu);
  }
  return 0;
}
