#include "lib8086.h"
#include <astro/cutils.h>

int main(int argc, char **argv) {
  long readen_bytes = 0;
  u8 *instructions = read_file(argv[1], &readen_bytes);
  int i = 0;

  if (argc < 2) {
    fprintf(stderr, "Usage %s <urasmom>\n", argv[1]);
    return -1;
  }

  printf("ORG 100h\n\n");

  while (i < readen_bytes) {
    u8 op = instructions[i];
    x8086Instruction instr = x8086_opcode_table[op].decoder(instructions + i);
    x8086PrintDisasm(instr);
    i += instr.size;
  }

  return 0;
}