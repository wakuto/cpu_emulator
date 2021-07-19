#ifndef ASM_HEADER
#define ASM_HEADER
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int bool;

typedef enum {
  TK_INST,
  TK_REG,
  TK_DIGIT,
  TK_EOF
}Token_type;

typedef struct Token Token;

struct Token {
  Token_type type;
  Token *next;
  char *str;
};

typedef struct strarr strarr;
struct strarr {
  char **arr;
  u32 len;
};

enum Inst_type {
  R_TYPE,
  I_TYPE,
  S_TYPE
};

typedef struct {
  u32 inst;
  enum Inst_type type;

  u8 opecode;
  u8 rd;
  u8 funct3;
  u32 rs1;
  u32 rs2;
  u8 funct7;

  u16 imm;

  u32 result;
} Inst;

extern void print_inst(Inst *inst);

#define OPECODE(x)   (x & 0x7F)
#define RD(x)       ((x & (0x1F  << 7 )) >> 7)
#define FUNCT3(x)   ((x & (0x03  << 12)) >> 12)
#define RS1(x)      ((x & (0x1F  << 15)) >> 15)
#define RS2(x)      ((x & (0x1F  << 20)) >> 20)
#define FUNCT7(x)   ((x & (0x7F  << 25)) >> 25)
#define IMM_I(x)    ((x & (0xFFF << 20)) >> 20)
#define IMM_S5(x)   ((x & (0x1F  << 7 )) >> 7)
#define IMM_S12(x)  ((x & (0x7F  << 25)) >> 25)
#define IMM_S(x) (IMM_S5(x) | (IMM_S5(x) << 5))


#define OPE_R 0x33
#define OPE_I 0x13
#endif