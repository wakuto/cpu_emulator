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
    0x00a00193,
    0xfff18193,
    0xfe019ee3,
    0x14e00193,
    0x0000006f
  };
  /*

    //0x00 addi x2, x0, 1
    0b00000000000100000000000100010011,
    //0x04 addi x3, x0, 1
    0b00000000000100000000000110010011,
    //0x08 add x2, x2, x3
    0b00000000001100010000000100110011,
    //0x0c add x3, x2, x3
    0b00000000001100010000000110110011,
    //0x10 jalr x1, 8(x0)
    0b00000000100000000000000011100111
    };*/
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
  /*
  cpu->reg[1] = 1000;
  cpu->reg[2] = 5;
  cpu->reg[11] = 3;
  cpu->reg[12] = 0;
  */

  printreg(cpu);

  for(cpu->pc = 0; cpu->pc < MEM_SIZE; ) {
    puts("-------------------------");
    fetch(cpu);

    decode(cpu);
    print_inst(cpu->idex_reg.inst);

    execute(cpu);

    mem_access(cpu);

    writeback(cpu);

    printreg(cpu);
    getchar();
  }
  return 0;
}
