#include "utilities.h"
#include "definitions.h"
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

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
  decoded_t *decoded = state->pipeline->decoded;
  state->memory[memLoc + 3] = state->registers[decoded->rd] >> 24;
  state->memory[memLoc + 2] = (state->registers[decoded->rd] << 8) >> 24;
  state->memory[memLoc + 1] = (state->registers[decoded->rd] << 16) >> 24;
  state->memory[memLoc]     = (state->registers[decoded->rd] << 24) >> 24;
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
  if (state->pipeline->decoded->cond == 14) {
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

state_t *newState(void) {
  state_t *state = malloc(sizeof(state_t));
  if (state == NULL) {
    printf("could not allocate space for state.\n");
    return NULL;
  }

  state->pipeline = malloc(sizeof(pipeline_t));
  if (state->pipeline == NULL) {
    printf("could not allocate space for pipeline struct.\n");
    return NULL;
  }

  state->pipeline->decoded = malloc(sizeof(decoded_t));
  if (state->pipeline->decoded == NULL) {
    printf("could not allocate space for decoded struct");
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

  state->isFetched    = 0;
  state->isTerminated = 0;

  return state;
}
