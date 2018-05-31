#include "utilities.h"
#include "definitions.h"
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

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

uint32_t logicalLeft(uint32_t n, int d) {
  return (n << d);
}

uint32_t logicalRight(uint32_t n, int d) {
  return (n >> d);
}

uint32_t arithmeticRight(uint32_t n, int d) {
  int temp = (int) n;
  temp >>= d;
  return (uint32_t) temp;
}

uint32_t rotateRight(uint32_t n, int d) {
  return (n >> d) | (n << ((BITS_IN_WORD) - d));
}

int checkAddCarryOut(uint32_t a, uint32_t b) {
  if ((a & BIT_31) & (b & BIT_31)) {
    return ((a + b) & BIT_31) == 0;
  } else if (!(a & BIT_31) & !(b & BIT_31)) {
    return ((a + b) & BIT_31) != 0;
  }
  return 0;
}

int checkSubCarryOut(uint32_t a, uint32_t b) {
  if (b > a) {
    return 0;
  }
  return 1;
}

uint32_t loadMemory(uint8_t *memory, uint32_t memLoc) {
  return (memory[memLoc + 3] << 24) |
         (memory[memLoc + 2] << 16) |
         (memory[memLoc + 1] << 8)  |
         (memory[memLoc]);
}

void storeMemory(state_t *state, uint32_t memLoc) {
  state->memory[memLoc + 3] = state->registers[state->decoded->rd] >> 24;
  state->memory[memLoc + 2] = (state->registers[state->decoded->rd] << 8) >> 24;
  state->memory[memLoc + 1] = (state->registers[state->decoded->rd] << 16) >> 24;
  state->memory[memLoc]     = (state->registers[state->decoded->rd] << 24) >> 24;
}

uint32_t barrelShifter(uint32_t value, int shiftAmount, int *carryOut, shiftType_t shiftType) {
  uint32_t result = 0;
  switch(shiftType) {
    case LSL:
      result    = logicalLeft(value, shiftAmount);
      *carryOut = (value >> (BITS_IN_WORD - shiftAmount - 1)) & BIT_0;
      break;
    case LSR:
      result    = logicalRight(value, shiftAmount);
      *carryOut = (value >> (shiftAmount - 1)) & BIT_0;
      break;
    case ASR:
      result    = arithmeticRight(value, shiftAmount);
      *carryOut = (value >> (shiftAmount - 1)) & BIT_0;
      break;
    case ROR:
      result    = rotateRight(value, shiftAmount);
      *carryOut = (value >> (shiftAmount - 1)) & BIT_0;
      break;
    default:
      printf("not valid barrel shifter operation");
      return 0;
  }
  return result;
}

/* Checks whether condition satisfied according to condition code in decoded instruction
 * Shifts CPSR register right so can compare bits 28-31
 * Returns 0 or 1 accordingly */

int checkCond(state_t *state, cond_t cond) {
  if (state->decoded->cond == 14) {
    return 1;

  } else {
    uint32_t flags = (state->registers[CPSR_REG]) >> 28;
    int N = flags & 8;
    int Z = flags & 4;
    int V = flags & 1;

    switch(cond) {
      case AL:
        return 1;
      case EQ:
        return Z;
      case NE:
        return !Z;
      case GE:
        return N == V;
      case LT:
        return N != V;
      case GT:
        return !Z & (N == V);
      case LE:
        return Z | (N != V);
      default:
        return 0;
    }
  }
}

/* Decodes instruction and passes relevant arguments to execute
 * execute(state, cond, instr, instrNumber)
 * Initialises required parts of decoded_t struct for that instruction */

uint32_t getInstruction(state_t *state) {
  uint32_t instruction;
  instruction = (state->memory[state->registers[PC_REG]]) |
                (state->memory[state->registers[PC_REG] + 1] << 8) |
                (state->memory[state->registers[PC_REG] + 2] << 16) |
                (state->memory[state->registers[PC_REG] + 3] << 24);
  return instruction;
}

instr_t getInstructionNum(uint32_t pc) {
  if ((pc << 4) >> 28 == 0xa) {
    return BRANCH;
  } else if ((pc << 4) >> 30 == 1) {
    return SINGLE_DATA_TRANSFER;
  } else if ((pc << 6) >> 31 == 0 && (pc << 24) >> 28 == 9) {
    return MULTIPLY;
  } else {
    return DATA_PROCESSING;
  }
}

/*
*  reads input binary file and stores instructions in memory.
*/
void readBinary(state_t *state, char* fileName) {

  FILE *fptr;
  fptr = fopen(fileName, "rb");

  fseek(fptr, 0, SEEK_END);
  int fileLen = ftell(fptr);
  fseek(fptr, 0, SEEK_SET);

  fread(state->memory, fileLen, 1, fptr);

  fclose(fptr);
}

state_t *newState(void) {
  state_t *state = malloc(sizeof(state_t));
  if (state == NULL) {
    printf("could not allocate space for state.\n");
    return NULL;
  }

  state->decoded = malloc(sizeof(decoded_t));
  if (state->decoded == NULL) {
    printf("could not allocate space for decoded struct.\n");
    return NULL;
  }

  state->registers = calloc(NUM_REGS, sizeof(uint32_t));
  if (state->registers == NULL) {
    printf("could not allocate space for registers.\n");
    return NULL;
  }

  state->memory = calloc(MEMORY_SIZE, sizeof(uint8_t));
  if (state->memory == NULL) {
    printf("could not allocate space for memory.\n");
    return NULL;
  }

  state->isTerminated = 0;

  return state;
}
