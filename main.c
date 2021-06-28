#include<stdio.h>
#include<stdlib.h>
#include"cpu.h"

int main(void) {
  CPU *cpu = (CPU*)malloc(sizeof(CPU));
  Inst *inst = fetch(cpu);
  decode(cpu, inst);
  execute(inst);
  mem_access();
  writeback(cpu, inst);
  free(inst);
  return 0;
}
