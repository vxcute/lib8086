#include "lib8086.h"

void x8086PrintDisasm(const x8086Instruction instr) {
  printf("%s ", instr.mnemonic);
  int op_count = ArrayCount(instr.operands);

  for (int i = 0; i < op_count; i++) {
    x8086Op op = instr.operands[i];

    switch (op.kind) {
    case x8086_REG: {
      printf("%s", x8086GetRegName(op.reg, instr.is_wide));
    } break;
    case x8086_MEM: {
      x8086MemOp mem = op.mem;

      if (instr.is_wide)
        printf("WORD ");
      else
        printf("BYTE ");

      if (mem.has_disp)
        printf("[%s + 0x%X]", x8086GetMemoryOp(mem), mem.disp);

      else
        printf("[%s]", x8086GetMemoryOp(mem));

    } break;
    case x8086_IMM: {
      printf("0x%X", op.imm);
    } break;
    case x8086_DIRECT_ADDR: {
      printf("[0x%X]", op.daddr);
    } break;
    }

    if (i < op_count - 1) {
      printf(", ");
    }
  }
  printf("\n");
}

const u16 x8086Read16(const u8 *buf, int start) {
  return ((u16)buf[start + 1] << 8) | buf[start];
}

const char *x8086GetRegName(const x8086RegOp reg, bool is_wide) {

  const char *regs[][2] = {
      {"AL", "AX"}, {"CL", "CX"}, {"DL", "DX"}, {"BL", "BX"},
      {"AH", "SP"}, {"CH", "BP"}, {"DH", "SI"}, {"BH", "DI"},
  };

  return regs[reg.idx][is_wide];
}

const char *x8086GetMemoryOp(const x8086MemOp mem) {

  const char *regs[] = {
      "BX + SI", "BX + DI", "BP + SI", "BP + DI", "SI", "DI", "BP", "BX",
  };

  return regs[mem.base];
};

x8086MemOp x8086DecodeMemOp(const u8 *instructions) {
  x8086MemOp mem;
  u8 modrm;
  u8 mod;
  bool is_wide;
  u8 i;

  memset(&mem, 0, sizeof(mem));
  i = 0;
  is_wide = instructions[i] & x8086_WIDE_MASK;
  modrm = instructions[i + 1];
  mod = x8086_MOD(modrm);

  mem.base = x8086_RM(modrm);
  mem.has_disp =
      (mod == x8086_MEMORY_MODE8 || mod == x8086_MEMORY_MODE16) ? true : false;

  if (mem.has_disp) {
    mem.disp = (mod == x8086_MEMORY_MODE16) ? x8086Read16(instructions, i + 2)
                                            : instructions[i + 2];

    mem.disp_size = (mod == x8086_MEMORY_MODE16) ? 2 : 1;
  }

  return mem;
}

x8086RegOp x8086DecodeRegOp(const u8 *instructions, int opi) {
  x8086RegOp reg;
  u8 modrm;
  u8 mod;
  u8 d;
  u8 i;

  memset(&reg, 0, sizeof(reg));
  i = 0;
  modrm = instructions[i + 1];

  reg.idx = opi ? x8086_REG(modrm) : x8086_RM(modrm);
  return reg;
}

x8086Instruction x8086DecodeInstruction(const u8 *instructions) {
  u8 op = instructions[0];
  x8086Instruction instr = x8086_opcode_table[op].decoder(instructions);
  return instr;
}

x8086Instruction x8086_op_rm_r_Decode(const u8 *instructions) {
  x8086Instruction instr;
  u8 mod;
  u8 i;
  u8 instr_ln;
  u8 op;
  u8 rm;

  memset(&instr, 0, sizeof(instr));
  instr_ln = 0;
  i = 0;
  op = instructions[i];
  instr.mnemonic = x8086_opcode_table[op].mnemonic;
  instr.is_wide = instructions[i] & x8086_WIDE_MASK;
  mod = x8086_MOD(instructions[i + 1]);
  rm = x8086_RM(instructions[i + 1]);

  if (mod == x8086_REGISTER_MODE) {
    instr.operands[0].kind = x8086_REG;
    instr.operands[0].reg.idx = x8086_RM(instructions[i + 1]);
  } else {
    if (mod == x8086_MEMORY_MODE && rm == x8086_DIRECT_ADDRESS) {
      instr.operands[0].kind = x8086_DIRECT_ADDR;
      instr.operands[0].daddr = x8086Read16(instructions, i + 2);
      instr_ln += 2;
    } else {
      instr.operands[0].mem = x8086DecodeMemOp(instructions);
      instr_ln += instr.operands[0].mem.disp_size;
    }
  }

  instr.operands[1].kind = x8086_REG;
  instr.operands[1].reg.idx = x8086_REG(instructions[i + 1]);

  instr_ln += 2;
  instr.size = instr_ln;

  return instr;
}

// op_r_rm
x8086Instruction x8086_op_r_rm_Decode(const u8 *instructions) {

  x8086Instruction instr;
  u8 mod;
  u8 i;
  u8 instr_ln;
  u8 op;
  u8 rm;

  memset(&instr, 0, sizeof(instr));
  instr_ln = 0;
  i = 0;
  op = instructions[i];
  instr.mnemonic = x8086_opcode_table[op].mnemonic;
  instr.is_wide = instructions[i] & x8086_WIDE_MASK;
  mod = x8086_MOD(instructions[i + 1]);
  rm = x8086_RM(instructions[i + 1]);

  if (mod == x8086_REGISTER_MODE) {
    instr.operands[1].kind = x8086_REG;
    instr.operands[1].reg.idx = x8086_RM(instructions[i + 1]);
  } else {
    if (mod == x8086_MEMORY_MODE && rm == x8086_DIRECT_ADDRESS) {
      instr.operands[1].kind = x8086_DIRECT_ADDR;
      instr.operands[1].daddr = x8086Read16(instructions, i + 2);
      instr_ln += 2;
    } else {
      instr.operands[1].mem = x8086DecodeMemOp(instructions);
      instr_ln += instr.operands[1].mem.disp_size;
    }
  }

  instr.operands[0].kind = x8086_REG;
  instr.operands[0].reg.idx = x8086_REG(instructions[i + 1]);

  instr_ln += 2;
  instr.size = instr_ln;

  return instr;
}

x8086Instruction x8086_mov_r_imm_Decode(const u8 *instructions) {
  x8086Instruction instr;
  u8 mod;
  u8 i;
  u8 instr_ln;
  u8 op;

  memset(&instr, 0, sizeof(instr));
  instr_ln = 0;
  i = 0;
  op = instructions[i];
  instr.mnemonic = x8086_opcode_table[op].mnemonic;
  instr.is_wide = instructions[i] & x8086_WIDE_MASK2;

  instr.operands[0].kind = x8086_REG;
  instr.operands[0].reg.idx = x8086_REG2(instructions[i]);

  instr.operands[1].kind = x8086_IMM;

  instr.operands[1].imm =
      instr.is_wide ? x8086Read16(instructions, i + 1) : instructions[i + 1];

  instr.size = instr.is_wide ? 3 : 2;

  return instr;
}

x8086Instruction x8086_op_acc_imm_Decode(const u8 *instructions) {
  x8086Instruction instr;
  u8 mod;
  u8 i;
  u8 instr_ln;
  u8 op;

  memset(&instr, 0, sizeof(instr));
  instr_ln = 0;
  i = 0;
  op = instructions[i];
  instr.mnemonic = x8086_opcode_table[op].mnemonic;
  instr.is_wide = instructions[i] & x8086_WIDE_MASK;

  instr.operands[0].kind = x8086_REG;
  instr.operands[0].reg.idx = 0;

  instr.operands[1].kind = x8086_IMM;

  instr.operands[1].imm =
      instr.is_wide ? x8086Read16(instructions, i + 1) : instructions[i + 1];

  instr.size = instr.is_wide ? 3 : 2;

  return instr;
}

x8086Instruction x8086_mov_m_acc_Decode(const u8 *instructions) {
  x8086Instruction instr;
  u8 mod;
  u8 i;
  u8 instr_ln;
  u8 op;

  memset(&instr, 0, sizeof(instr));
  instr_ln = 0;
  i = 0;
  op = instructions[i];
  instr.mnemonic = x8086_opcode_table[op].mnemonic;
  instr.is_wide = instructions[i] & x8086_WIDE_MASK;

  instr.operands[0].kind = x8086_DIRECT_ADDR;
  instr.operands[0].daddr = x8086Read16(instructions, i + 1);

  instr.operands[1].kind = x8086_REG;
  instr.operands[1].reg.idx = RegisterA;

  return instr;
}

x8086Instruction x8086_mov_acc_m_Decode(const u8 *instructions) {
  x8086Instruction instr;
  u8 mod;
  u8 i;
  u8 instr_ln;
  u8 op;

  memset(&instr, 0, sizeof(instr));
  instr_ln = 0;
  i = 0;
  op = instructions[i];
  instr.mnemonic = x8086_opcode_table[op].mnemonic;
  instr.is_wide = instructions[i] & x8086_WIDE_MASK;

  instr.operands[0].kind = x8086_REG;
  instr.operands[0].reg.idx = RegisterA;

  instr.operands[1].kind = x8086_DIRECT_ADDR;
  instr.operands[1].daddr = x8086Read16(instructions, i + 1);

  return instr;
}
