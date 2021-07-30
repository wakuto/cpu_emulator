#ifndef CPU_HEADER
#define CPU_HEADER
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed int i32;

enum Inst_type {
  R_TYPE,
  I_TYPE,
  S_TYPE
};

#define MMIO 0x100000

typedef struct {
  u32 inst;
  enum Inst_type type;

  u8 opecode;
  u8 rd;
  u8 funct3;
  u32 rs1;
  u32 rs2;
  u8 funct7;

  u32 imm;

  u32 result;
} Inst;

typedef struct {
  Inst *inst;
} IFID;
typedef struct {
  Inst *inst;
} IDEX;
typedef struct {
  Inst *inst;
} EXMEM;
typedef struct {
  Inst *inst;
} MEMWB;

#define MEM_SIZE 1024

typedef struct {
  u32 reg[32];
  u32 pc;
  u8 mem[MEM_SIZE];

  IFID ifid_reg;
  IDEX idex_reg;
  EXMEM exmem_reg;
  MEMWB memwb_reg;
} CPU;

extern void print_inst(Inst *inst);
extern void printreg(CPU *cpu);


#define OPECODE(x)   (x & 0x7F)
#define RD(x)       ((x & (0x1F  << 7 )) >> 7)
#define FUNCT3(x)   ((x & (0x07  << 12)) >> 12)
#define RS1(x)      ((x & (0x1F  << 15)) >> 15)
#define RS2(x)      ((x & (0x1F  << 20)) >> 20)
// R type
#define FUNCT7(x)   ((x & (0x7F  << 25)) >> 25)
// I type
#define IMM_I(x)    ((x & (0xFFF << 20)) >> 20)
// S type
#define IMM_S4_0(x)   ((x & (0x1F  << 7 )) >> 7)
#define IMM_S11_5(x)  ((x & (0x7F  << 25)) >> 25)
#define IMM_S(x) ((IMM_S11_5(x) << 5) | (IMM_S4_0(x)))
// B type
#define IMM_B12(x)   ((x & (0x01 << 31)) >> 31)
#define IMM_B10_5(x) ((x & (0x3F << 25)) >> 25)
#define IMM_B4_1(x)  ((x & (0x0F << 8 )) >> 8)
#define IMM_B11(x)   ((x & (0x01 << 7 )) >> 7)
#define IMM_B(x) ((IMM_B12(x) << 12) | (IMM_B11(x) << 11) | (IMM_B10_5(x) << 5) | (IMM_B4_1(x) << 1))
// U type
#define IMM_U(x)      ((x & (0xFFFFF << 12)) >> 12)
// J type
#define IMM_J20(x)    ((x & (0x01 << 31)) >> 31)
#define IMM_J10_1(x)  ((x & (0x3FF << 21)) >> 21)
#define IMM_J11(x)    ((x & (0x01 << 20)) >> 20)
#define IMM_J19_12(x) ((x & (0xFF << 12)) >> 12)
#define IMM_J(x) ((IMM_J20(x) << 20) | (IMM_J19_12(x) << 12) | (IMM_J11(x) << 11) | (IMM_J10_1(x) << 1))



// opecode別の分類
#define OPE_I_LD 0x03
#define OPE_I_AL 0x13
#define OPE_S 0x23
#define OPE_R 0x33
#define OPE_I_JA 0x67
#define OPE_J 0x6F
#define OPE_B 0x63
#define OPE_U_LUI 0x37
#define OPE_U_AUI 0x17

extern void fetch(CPU *cpu);
extern void decode(CPU *cpu);
extern void execute(CPU *cpu);
extern int mem_access(CPU *cpu);
extern int writeback(CPU *cpu);

extern void memwrite(CPU *cpu, u32 addr, u8 data);
extern u8 memread(CPU *cpu, u32 addr);
extern u32 sign_extend(u32 imm, u8 len);
#endif
