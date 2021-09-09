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

void program_writer(char *filename, CPU *cpu) {
  FILE *program = fopen(filename, "rb");
  if(program == NULL) {
    fprintf(stderr, "program.binの取得に失敗しました。\n");
    exit(1);
  }

  u32 i = 0;
  i32 ch;
  while((ch = fgetc(program)) != EOF) {
    cpu->mem[i] = 0xFF & ch;
    i++;
  }
  // 命令の長さが4の倍数でない場合は0でパディング
  if(i % 4 != 0) {
    for(u32 j = 0; j < 4-(i%4); j++) {
      cpu->mem[i] = 0x00;
      i++;
    }
  } 
}

int main(void) {
  setup();

  CPU *cpu = (CPU*)calloc(1,sizeof(CPU));

  // 命令の書き込み
  program_writer("./program.bin", cpu);

  // stackの初期化
  cpu->reg[2] = MEM_SIZE-4;
  // entry pointの設定
  cpu->pc = 0;

  /*
  mutex_init();
  pthread_t fetch_thread;
  pthread_t decode_thread;
  pthread_t execute_thread;
  pthread_t mem_access_thread;
  pthread_t writeback_thread;
  */

  u32 break_flag = 0;
  u32 break_point = 0xffffffff;
  i32 i = 0;
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
