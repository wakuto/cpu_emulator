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
    0xfe010113,
    0x00112e23,
    0x00812c23,
    0x00912a23,
    0x02010413,
    0xfea42623,
    0xfec42783,
    0x00078863,
    0xfec42703,
    0x00100793,
    0x00f71663,
    0xfec42783,
    0x0300006f,
    0xfec42783,
    0xffe78793,
    0x00078513,
    0xfc1ff0ef,
    0x00050493,
    0xfec42783,
    0xfff78793,
    0x00078513,
    0xfadff0ef,
    0x00050793,
    0x00f487b3,
    0x00078513,
    0x01c12083,
    0x01812403,
    0x01412483,
    0x02010113,
    0x00008067,
    0xfe010113,
    0x00112e23,
    0x00812c23,
    0x02010413,
    0x20000793,
    0xfef42623,
    0x00600513,
    0xf6dff0ef,
    0x00050713,
    0xfec42783,
    0x00e7a023,
    0x0000006f,
    0xfd1ff0ef
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
      printf("%02x", cpu->mem[i+j*4]);
    }
    puts("");
  }
  puts("");
  cpu->reg[2] = 1024;
  /*
  cpu->reg[2] = 5;
  cpu->reg[11] = 3;
  cpu->reg[12] = 0;
  */

  printreg(cpu);

  for(cpu->pc = 0xa8; cpu->pc < MEM_SIZE; ) {
    if(cpu->pc >= 0xa0) {
      getchar();
    }
    puts("-------------------------");
    fetch(cpu);

    decode(cpu);
    print_inst(cpu->idex_reg.inst);

    execute(cpu);

    mem_access(cpu);

    writeback(cpu);

    printreg(cpu);

    printf("512:%x, %u\n", cpu->mem[512], cpu->mem[512]);
  }
  return 0;
}
