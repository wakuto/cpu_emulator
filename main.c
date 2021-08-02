#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<signal.h>
#include<string.h>
#include<termios.h>
#include<unistd.h>
#include"cpu.h"

struct termios term_setting;
struct termios old_term_setting;

void exit_handler(int sig) {
  puts("シグナルをハンドルしたよ");
  tcsetattr(STDIN_FILENO, TCSANOW, &old_term_setting);
  exit(1);
}

// signal と termainalの設定
void setup(void) {
  struct sigaction sig;
  memset(&sig, 0, sizeof(sig));
  sig.sa_handler = exit_handler;
  if(sigaction(SIGINT, &sig, NULL) < 0) {
    fprintf(stderr, "signalを設定できませんでした\n");
    exit(1);
  }

  tcgetattr(STDIN_FILENO, &term_setting);
  tcgetattr(STDIN_FILENO, &old_term_setting);
  term_setting.c_lflag &= ~ECHO;
  term_setting.c_lflag &= ~ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &term_setting);
}

int main(void) {
  setup();

  CPU *cpu = (CPU*)calloc(1,sizeof(CPU));
  int i = 0;
  int j = 0;

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
  cpu->reg[2] = MEM_SIZE-4;
  // entry pointの設定
  u32 entry_point = 0x0;
  cpu->pc = entry_point;
  mutex_init();

  /*
  pthread_t fetch_thread;
  pthread_t decode_thread;
  pthread_t execute_thread;
  pthread_t mem_access_thread;
  pthread_t writeback_thread;
  */

  u32 break_flag = 0;
  u32 break_point = 0xffffffff;
  i = 0;
  for(; cpu->pc < MEM_SIZE; ) {
    if(cpu->pc == break_point) break_flag = 1;
    switch(i) {
      case 0: {
        fetch(cpu);
        break;
      }
      case 1: {
        decode(cpu);
        if(break_flag) {
          puts("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
          printreg(cpu);
          print_inst(cpu->idex_reg.inst);
          getchar();
        }
        break;
      }
      case 2: {
        execute(cpu);
        break;
      }
      case 3: {
        mem_access(cpu);
        break;
      }
      case 4: {
        writeback(cpu);
        break;
      }
    }
    i = (i+1) % 5;
  }
  return 0;
}
