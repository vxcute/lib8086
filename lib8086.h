#ifndef _LIB8086_H
#define _LIB8086_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define x8086_OPCODE_TABLE_SIZE 0xFF

#define ArrayCount(a) (sizeof(a) / sizeof(a[0]))

#define x8086_MOD(b) ((b) >> 6)
#define x8086_REG(b) (((b) >> 3) & 7)
#define x8086_RM(b) ((b)&7)

#define x8086_REG2(b) ((b)&7)

#define x8086_D_MASK 2
#define x8086_WIDE_MASK 1
#define x8086_WIDE_MASK2 1 << 3

#define x8086_DIRECT_ADDRESS 6

typedef unsigned char u8;
typedef unsigned short u16, x8086DirectAddr, x8086Imm;
typedef short word;

typedef enum {
  RegisterA = 0,
} x8086Register;

typedef enum {
  x8086_MEMORY_MODE = 0,
  x8086_MEMORY_MODE8,
  x8086_MEMORY_MODE16,
  x8086_REGISTER_MODE,
} x8086Mod;

typedef enum {
  x8086_MEM = 0,
  x8086_REG,
  x8086_IMM,
  x8086_DIRECT_ADDR,
} x8086OpKind;

typedef struct {
  bool has_disp;
  u8 base;
  u8 disp_size;
  word disp;
} x8086MemOp;

typedef struct {
  u8 idx;
} x8086RegOp;

typedef struct {
  x8086OpKind kind;

  union {
    x8086RegOp reg;
    x8086MemOp mem;
    x8086Imm imm;
    x8086DirectAddr daddr;
  };

} x8086Op;

typedef struct {
  u8 opcode;
  u8 size;
  bool is_wide;
  const char *mnemonic;
  x8086Op operands[2];
} x8086Instruction;

typedef x8086Instruction (*x8086DecoderFunc)(const u8 *instructions);

typedef struct {
  const char *mnemonic;
  x8086DecoderFunc decoder;
} x8086Opcode;

static const char *x8086GetMemoryOp(const x8086MemOp mem);
static const char *x8086GetRegName(const x8086RegOp reg, bool is_wide);
static x8086MemOp x8086DecodeMemOp(const u8 *instructions);
static x8086RegOp x8086DecodeRegOp(const u8 *instructions, int opi);
void x8086PrintDisasm(const x8086Instruction instr);

x8086Instruction x8086DecodeInstruction(const u8 *instructions);

x8086Instruction x8086_op_rm_r_Decode(const u8 *instructions);
x8086Instruction x8086_op_r_rm_Decode(const u8 *instructions);
x8086Instruction x8086_op_acc_imm_Decode(const u8 *instructions);
x8086Instruction x8086_mov_r_imm_Decode(const u8 *instructions);
x8086Instruction x8086_mov_m_acc_Decode(const u8 *instructions);
x8086Instruction x8086_mov_acc_m_Decode(const u8 *instructions);

static const x8086Opcode x8086_opcode_table[x8086_OPCODE_TABLE_SIZE] = {
    // op_rm_r
    [0x00] = {"ADD", x8086_op_rm_r_Decode},
    [0x01] = {"ADD", x8086_op_rm_r_Decode},
    [0x88] = {"MOV", x8086_op_rm_r_Decode},
    [0x89] = {"MOV", x8086_op_rm_r_Decode},
    [0x38] = {"CMP", x8086_op_rm_r_Decode},
    [0x39] = {"CMP", x8086_op_rm_r_Decode},
    [0x28] = {"SUB", x8086_op_rm_r_Decode},
    [0x29] = {"SUB", x8086_op_rm_r_Decode},
    [0x08] = {"OR", x8086_op_rm_r_Decode},
    [0x09] = {"OR", x8086_op_rm_r_Decode},
    [0x10] = {"ADC", x8086_op_rm_r_Decode},
    [0x11] = {"ADC", x8086_op_rm_r_Decode},
    [0x30] = {"XOR", x8086_op_rm_r_Decode},
    [0x31] = {"XOR", x8086_op_rm_r_Decode},
    [0x20] = {"AND", x8086_op_rm_r_Decode},
    [0x21] = {"AND", x8086_op_rm_r_Decode},
    [0x18] = {"SBB", x8086_op_rm_r_Decode},
    [0x19] = {"SBB", x8086_op_rm_r_Decode},

    // op_r_rm
    [0x02] = {"ADD", x8086_op_r_rm_Decode},
    [0x03] = {"ADD", x8086_op_r_rm_Decode},
    [0x8A] = {"MOV", x8086_op_r_rm_Decode},
    [0x8B] = {"MOV", x8086_op_r_rm_Decode},
    [0x0A] = {"OR", x8086_op_r_rm_Decode},
    [0x0B] = {"OR", x8086_op_r_rm_Decode},
    [0x3A] = {"CMP", x8086_op_r_rm_Decode},
    [0x3B] = {"CMP", x8086_op_r_rm_Decode},
    [0x2A] = {"SUB", x8086_op_r_rm_Decode},
    [0x2B] = {"SUB", x8086_op_r_rm_Decode},
    [0x12] = {"ADC", x8086_op_r_rm_Decode},
    [0x13] = {"ADC", x8086_op_r_rm_Decode},
    [0x32] = {"XOR", x8086_op_r_rm_Decode},
    [0x33] = {"XOR", x8086_op_r_rm_Decode},
    [0x22] = {"AND", x8086_op_r_rm_Decode},
    [0x23] = {"AND", x8086_op_r_rm_Decode},
    [0x1A] = {"SBB", x8086_op_r_rm_Decode},
    [0x1B] = {"SBB", x8086_op_r_rm_Decode},

    // op_acc_imm
    [0x04] = {"ADD", x8086_op_acc_imm_Decode},
    [0x05] = {"ADD", x8086_op_acc_imm_Decode},
    [0x2C] = {"SUB", x8086_op_acc_imm_Decode},
    [0x2D] = {"SUB", x8086_op_acc_imm_Decode},
    [0x3C] = {"CMP", x8086_op_acc_imm_Decode},
    [0x3D] = {"CMP", x8086_op_acc_imm_Decode},

    // mov_r_imm8
    [0xB0] = {"MOV", x8086_mov_r_imm_Decode},
    [0xB1] = {"MOV", x8086_mov_r_imm_Decode},
    [0xB2] = {"MOV", x8086_mov_r_imm_Decode},
    [0xB3] = {"MOV", x8086_mov_r_imm_Decode},
    [0xB4] = {"MOV", x8086_mov_r_imm_Decode},
    [0xB5] = {"MOV", x8086_mov_r_imm_Decode},
    [0xB6] = {"MOV", x8086_mov_r_imm_Decode},
    [0xB7] = {"MOV", x8086_mov_r_imm_Decode},

    // mov_r_imm16
    [0xB8] = {"MOV", x8086_mov_r_imm_Decode},
    [0xB9] = {"MOV", x8086_mov_r_imm_Decode},
    [0xBA] = {"MOV", x8086_mov_r_imm_Decode},
    [0xBB] = {"MOV", x8086_mov_r_imm_Decode},

    [0xA0] = {"MOV", x8086_mov_acc_m_Decode},
    [0xA1] = {"MOV", x8086_mov_acc_m_Decode},

    [0xA2] = {"MOV", x8086_mov_m_acc_Decode},
    [0xA3] = {"MOV", x8086_mov_m_acc_Decode},
};

#endif /* END OF _LIB8086_H  */