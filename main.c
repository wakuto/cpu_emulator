#include<stdio.h>
#include<stdlib.h>
#include"cpu.h"

int main(void) {
  CPU *cpu = (CPU*)calloc(1,sizeof(CPU));
  int i = 0;
  int j = 0;
  printf("Hello RISC-V!\n");

  /*
  add x12, x10, x11
  sub x13, x12, x11
  addi x13, x0, 64
  sw x12, 0(x13)
  lw x14, 0(x13)
  */
  u32 instruction[] = {
    0xB50633,
    0x40B606B3,
    0b11111111101101101000011010010011,
    0b00000000110001101010000000100011,
    0b00000000000001101010011100000011
    };
  u32 inst_count = sizeof(instruction)/sizeof(u32);
  // 命令の書き込み
  for(j = 0; j < inst_count; j++) {
    for(i = 0; i < 4; i++) {
      cpu->mem[i + j*4] = (u8)(instruction[j] >> (i*8));
      printf("%02x", cpu->mem[i]);
    }
  }
  puts("");
  cpu->pc = 0;
  cpu->reg[2] = 1000;
  cpu->reg[10] = 5;
  cpu->reg[11] = 3;
  cpu->reg[12] = 0;

  printreg(cpu);

  for(j = 0; j < inst_count; j++) {
    puts("-------------------------");
    fetch(cpu);

    decode(cpu);
    print_inst(cpu->idex_reg.inst);

    execute(cpu);

    mem_access(cpu);

    writeback(cpu);

    printreg(cpu);
  }
  return 0;
}
