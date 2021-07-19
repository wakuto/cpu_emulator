.section .text;
.globl _start
_start:
  addi x3, x0, 10
.loop:
  addi x3, x3, -1
  bne x3, x0, .loop
  addi x3, x0, 334
.end:
  j .end
