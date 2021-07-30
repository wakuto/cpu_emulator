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
    0xfd010113,
    0x02812623,
    0x03010413,
    0x00050793,
    0xfcf40fa3,
    0x001007b7,
    0xfef42623,
    0xfec42783,
    0xfdf44703,
    0x00e78023,
    0x00000013,
    0x02c12403,
    0x03010113,
    0x00008067,
    0xfd010113,
    0x02112623,
    0x02812423,
    0x03010413,
    0xfca42e23,
    0xfe042623,
    0xfe042623,
    0x0280006f,
    0xfec42783,
    0xfdc42703,
    0x00f707b3,
    0x0007c783,
    0x00078513,
    0xf95ff0ef,
    0xfec42783,
    0x00178793,
    0xfef42623,
    0xfec42783,
    0xfdc42703,
    0x00f707b3,
    0x0007c783,
    0xfc0796e3,
    0x00000013,
    0x00000013,
    0x02c12083,
    0x02812403,
    0x03010113,
    0x00008067,
    0xff010113,
    0x00112623,
    0x00812423,
    0x01010413,
    0x0c800513,
    0xf7dff0ef,
    0x0000006f,
    0xfe5ff0ef,
    0x6c6c6548,
    0x6f57206f,
    0x21646c72,
    0x0000000a
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

  for(cpu->pc = 0xc4; cpu->pc < MEM_SIZE; ) {
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
