#include<stdio.h>
#include<stdlib.h>
#include"cpu.h"

int main(void) {
  CPU *cpu = (CPU*)calloc(1,sizeof(CPU));
  int i = 0;
  int j = 0;
  u32 entry_point = 0xc4;

  u32 instruction[] = {
#include "program.hex"
  };
  u32 inst_count = sizeof(instruction)/sizeof(u32);
  // 命令の書き込み
  for(j = 0; j < inst_count; j++) {
    for(i = 0; i < 4; i++) {
      cpu->mem[i + j*4] = (u8)(instruction[j] >> (i*8));
    }
  }

  // stackの初期化
  cpu->reg[2] = 1024;

  //printreg(cpu);

  for(cpu->pc = entry_point; cpu->pc < MEM_SIZE; ) {
    //getchar();
    //puts("-------------------------");
    fetch(cpu);

    decode(cpu);
    //print_inst(cpu->idex_reg.inst);

    execute(cpu);

    mem_access(cpu);

    writeback(cpu);

    //printreg(cpu);

  }
  return 0;
}
