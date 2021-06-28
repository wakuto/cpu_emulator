#ifndef CPU_HEADER
#define CPU_HEADER
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;


typedef struct {
  u32 reg[32];
  u32 pc;
  u8 mem[1024];
} CPU;

extern void printreg(CPU *cpu);

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

#endif

extern Inst* fetch(CPU *cpu);
extern void decode(CPU *cpu, Inst *inst);
extern void execute(Inst *inst);
extern int mem_access();
extern int writeback(CPU *cpu, Inst *inst);
