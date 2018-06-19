#include <stdio.h>
#include "definitions.h"

/*
*  reads input binary file and stores instructions in memory.
*/

void printState(state_t *state) {
  printf("Registers:\n");
  for (int i = 0; i < 13; i++) {
    printf("$%-2d : %10d (0x%08x)\n", i, state->registers[i], state->registers[i]);
  }
  printf("PC  : %10d (0x%08x)\n", state->registers[PC_REG], state->registers[PC_REG]);
  printf("CPSR: %10d (0x%08x)\n", state->registers[CPSR_REG], state->registers[CPSR_REG]);

  printf("Non-zero memory:\n");
  uint16_t i = 0;
  while (i < MEMORY_SIZE / 4) {
    uint32_t nextMemoryWord = (state->memory[i] << 24) |
                              (state->memory[i + 1] << 16) |
                              (state->memory[i + 2] << 8) |
                              (state-> memory[i + 3]);
    if (nextMemoryWord != 0) {
      printf("0x%08x: 0x%08x\n", i, nextMemoryWord);
    }
    i += 4;
  }
}

void printDecoded(decoded_t *decoded) {
  printf("============= Decoded Instruction =============\n");
  printf("Condition code: %d \n", decoded->cond);
  printf("Single bit values: \n");
  printf("I: %u \n", decoded->isI);
  printf("P: %u \n", decoded->isP);
  printf("U: %u \n", decoded->isU);
  printf("A: %u \n", decoded->isA);
  printf("S: %u \n", decoded->isS);
  printf("L: %u \n", decoded->isL);
  printf("Registers: \n");
  printf("Rn : %u \n", decoded->rn);
  printf("Rd : %u \n", decoded->rd);
  printf("Rs : %u \n", decoded->rs);
  printf("Rm : %u \n\n", decoded->rm);
  printf("Offset : %u \n\n", decoded->offset);
  printf("Opcode : %u \n", decoded->opCode);
  printf("Operand2 : %u \n", decoded->operand2);
  printf("================================================\n");
}

void readBinary(state_t *state, char* fileName) {

  FILE *fptr;
  fptr = fopen(fileName, "rb");

  fseek(fptr, 0, SEEK_END);
  int fileLen = ftell(fptr);
  fseek(fptr, 0, SEEK_SET);

  fread(state->memory, fileLen, 1, fptr);

  fclose(fptr);
}
