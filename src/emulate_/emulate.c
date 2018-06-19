#include <stdlib.h>
#include <stdint.h>
#include "definitions.h"
#include "utilities.h"
#include "ioutils.h"

void executeDataProcessing(state_t *state) {
  decoded_t *decoded = state->pipeline->decoded;
  uint32_t *registers = state->registers;

  uint32_t operand1 = registers[decoded->rn];
  uint32_t operand2;

  int carryOut = 0;
  int *carryOutPtr = &carryOut;

  if(decoded->isI) {
    uint32_t imm = ((uint32_t) decoded->operand2) & FIRST_BYTE;
    int shiftAmount = 2 * ((decoded->operand2 & THIRD_4_BITS) >> 8);
    operand2 = rotateRight(imm, shiftAmount);
  } else {
    int bit4 = (decoded->operand2) & BIT_4;
    int shiftAmount;
    if(bit4) {
      shiftAmount = registers[(decoded->operand2 & THIRD_4_BITS) >> 8] & FIRST_4_BITS;
    }
    else {
      shiftAmount = (decoded->operand2 & SHIFT_AMT_MASK) >> 7;
    }
    shiftType_t shiftType = (decoded->operand2 & SHIFT_TYPE_MASK) >> 5;
    operand2 = barrelShifter(registers[decoded->rm], shiftAmount, carryOutPtr, shiftType);
  }

  uint32_t result = 0;
  switch(decoded->opCode) {
    case AND:
      result = operand1 & operand2;
      registers[decoded->rd] = result;
      break;
    case EOR:
      result = operand1 ^ operand2;
      registers[decoded->rd] = result;
      break;
    case SUB:
      result = operand1 - operand2;
      registers[decoded->rd] = result;
      carryOut = checkSubCarryOut(operand1, operand2);
      break;
    case RSB:
      result = operand2 - operand1;
      registers[decoded->rd] = result;
      carryOut = checkSubCarryOut(operand2, operand1);
      break;
    case ADD:
      result = operand1 + operand2;
      registers[decoded->rd] = result;
      carryOut = checkAddCarryOut(operand1, operand2);
      break;
    case TST:
      result = operand1 & operand2;
      break;
    case TEQ:
      result = operand1 ^ operand2;
      break;
    case CMP:
      result = operand1 - operand2;
      carryOut = checkSubCarryOut(operand1, operand2);
      break;
    case ORR:
      result = operand1 | operand2;
      registers[decoded->rd] = result;
      break;
    case MOV:
      result = operand2;
      registers[decoded->rd] = result;
      break;
  }

  /* Set CPSR flags
   * 1. V unaffected
   * 2. C set to carry out from any shift operation.
   *    C set to carry out of bit 31 of the ALU in arithmetic operation.
   *    C set to 1 if addition produced a carry (unsigned overflow).
   *    C set to 0 if subtraction produced a borrow, 1 otherwise. */

  if(decoded->isS) {
    if (carryOut) {
      registers[CPSR_REG] = registers[CPSR_REG] | C_BIT;
    } else {
      registers[CPSR_REG] = registers[CPSR_REG] & ~C_BIT;
    }

    if (!result) {
      registers[CPSR_REG] = registers[CPSR_REG] | Z_BIT;
    } else {
      registers[CPSR_REG] = registers[CPSR_REG] & ~Z_BIT;
    }

    if ((result & N_BIT) >> (BITS_IN_WORD - 1)) {
      registers[CPSR_REG] = registers[CPSR_REG] | N_BIT;
    } else {
      registers[CPSR_REG] = registers[CPSR_REG] & ~N_BIT;
    }
  }
}

void executeMultiply(state_t *state) {
  decoded_t* decoded  = state->pipeline->decoded;
  uint32_t* registers = state->registers;
  if (decoded->isA) {
    registers[decoded->rd] = registers[decoded->rm] * registers[decoded->rs] + registers[decoded->rn];
  } else {
    registers[decoded->rd] = registers[decoded->rm] * registers[decoded->rs];
  }

  if (decoded->isS) {
    if (decoded->rd >> (BITS_IN_WORD - 1)) {
      state->registers[CPSR_REG] = state->registers[CPSR_REG] | N_BIT;
    } else {
      state->registers[CPSR_REG] = state->registers[CPSR_REG] & ~N_BIT;
    }

    if (!registers[decoded->rd]) {
      state->registers[CPSR_REG] = state->registers[CPSR_REG] | Z_BIT;
    } else {
      state->registers[CPSR_REG] = state->registers[CPSR_REG] & ~Z_BIT;
    }
  }
}


void executeSDT(state_t *state) {
  decoded_t* decoded       = state->pipeline->decoded;
  uint32_t* registers      = state->registers;
  uint8_t* memory          = state->memory;
  uint32_t baseRegContents = registers[decoded->rn];

  uint32_t offset = registers[(decoded->offset) & FIRST_4_BITS];
  if (decoded->isI) {
    int bit4 = (decoded->offset) & BIT_4;
    int shiftAmount;
    if (bit4) {
      shiftAmount = registers[((decoded->offset) & THIRD_4_BITS) >> 8] & FIRST_BYTE;
    } else {
      shiftAmount = (decoded->offset & SHIFT_AMT_MASK) >> 7;
    }
    shiftType_t shiftType = (decoded->offset & SHIFT_TYPE_MASK) >> 5;
    int carryOut = 1;
    int* carryOutPtr = &carryOut;
    offset = barrelShifter(offset, shiftAmount, carryOutPtr, shiftType);
  } else {
    offset = decoded->offset;
  }

  uint32_t newBase;
  if (decoded->isU) {
    newBase = baseRegContents + offset;
  } else {
    newBase = baseRegContents - offset;
  }

  if (decoded->isP) {
    if (newBase + 3 > MEMORY_SIZE) {
      if (!checkGPIOPins(newBase)) {
        return;
      } else {
        if (decoded->isL) {
          registers[decoded->rd] = newBase;
          return;
        } else {
          return;
        }
      }
    }

    if (decoded->isL) {
      registers[decoded->rd] = loadMemory(memory, newBase);
    } else {
      storeMemory(state, newBase);
    }

  } else {
    if (baseRegContents + 3 > MEMORY_SIZE) {
      printf("Error: Out of bounds memory access at address 0x%08x\n", baseRegContents);
      return;
    }
    if (decoded->isL) {
      registers[decoded->rd] = loadMemory(memory, baseRegContents);
    } else {
      storeMemory(state, baseRegContents);
    }
    registers[decoded->rn] = newBase;
  }
}


/* Branch Instruction:
 * Adds offset from bits 0 to 23 of decoded instruction to PC */

void executeBranch(state_t *state) {
  decoded_t* decoded = state->pipeline->decoded;
  state->registers[PC_REG] = ((int) state->registers[PC_REG]) + decoded->offset;
  state->isDecoded = 0;
  state->isFetched = 0;
}

/* Executes calls to different functions if condition satisfied
 * Function called depends on which of 4 instructions it is executing
 * If reaches n all 0 instruction, then emulator terminates (halt) and prints registers and memory */

void execute(state_t *state) {
  if (checkCond(state, state->pipeline->decoded->cond)) {
    func executePointers[] = {executeDataProcessing, executeMultiply, executeSDT, executeBranch};
    executePointers[state->instruction](state);
  }
}

void decodeBranch(pipeline_t *pipeline, uint32_t instruction) {
  pipeline->decoded->offset   = ((int) (instruction << 8)) >> 6;
  pipeline->decoded->cond     = (cond_t) instruction >> PC_SHIFT;
}

void decodeDataProcessing(pipeline_t *pipeline, uint32_t instruction) {
  pipeline->decoded->cond     = (cond_t) instruction >> PC_SHIFT;
  pipeline->decoded->isI      = instruction & IS_I;
  pipeline->decoded->isS      = instruction & IS_S;
  pipeline->decoded->opCode   = (opCode_t) ((instruction << 7) >> (BITS_IN_WORD - 4));
  pipeline->decoded->rn       = (instruction << RN_SHIFT) >> (BITS_IN_WORD - 4);
  pipeline->decoded->rd       = (instruction << RD_SHIFT) >> (BITS_IN_WORD - 4);
  pipeline->decoded->rm       = (instruction << RM_SHIFT) >> (BITS_IN_WORD - 4);
  pipeline->decoded->operand2 = (uint16_t)instruction;
}

void decodeSDT(pipeline_t *pipeline, uint32_t instruction) {
  pipeline->decoded->cond     = (cond_t) instruction >> PC_SHIFT;
  pipeline->decoded->isI      = instruction & IS_I;
  pipeline->decoded->isP      = instruction & IS_P;
  pipeline->decoded->isU      = instruction & IS_U;
  pipeline->decoded->isL      = instruction & IS_L;
  pipeline->decoded->rn       = (instruction << RN_SHIFT) >> (BITS_IN_WORD - 4);
  pipeline->decoded->rd       = (instruction << RD_SHIFT) >> (BITS_IN_WORD - 4);
  pipeline->decoded->offset   = (instruction << OFFSET_SHIFT) >> OFFSET_SHIFT;
}

void decodeMultiply(pipeline_t *pipeline, uint32_t instruction) {
  pipeline->decoded->cond     = (cond_t) instruction >> PC_SHIFT;
  pipeline->decoded->isA      = instruction & IS_A;
  pipeline->decoded->isS      = instruction & IS_S;
  pipeline->decoded->rd       = (instruction << RN_SHIFT) >> (BITS_IN_WORD - 4);
  pipeline->decoded->rn       = (instruction << RD_SHIFT) >> (BITS_IN_WORD - 4);
  pipeline->decoded->rs       = (instruction << RS_SHIFT) >> (BITS_IN_WORD - 4);
  pipeline->decoded->rm       = (instruction << RM_SHIFT) >> (BITS_IN_WORD - 4);
}


void decode(state_t *state) {
  uint32_t instruction = state->pipeline->fetched;
  pipeline_t *pipeline = state->pipeline;
  if (!(instruction)) {
    state->isTerminated       = 1;
  } else {
    instr_t instrNum = getInstructionNum(instruction);
    func decodePointers[] = {decodeDataProcessing, decodeMultiply, decodeSDT, decodeBranch};
    decodePointers[instrNum](pipeline, instruction);
    state->instruction = instrNum;
  }
  state->isDecoded = 1;
}

void fetch(state_t *state) {
  if (state->registers[PC_REG] > MEMORY_SIZE) {
    fprintf(stderr, "Error: Out of bounds memory access at address 0x%08x\n", state->registers[PC_REG]);
  }
  state->pipeline->fetched = getInstruction(state);
  state->isFetched = 1;
}

void incrementPC(state_t *state) {
  state->registers[PC_REG] += PC_INCREMENT;
}


int main(int argc, char* argv[]) {
  assert (argc == 2);

  state_t *state = newState();

  if (state == NULL) {
    return EXIT_FAILURE;
  }

  readBinary(state, argv[1]);

  /* Fetch: increments PC and passes state to decode part of pipeline */
  while (!state->isTerminated) {
    if (state->isDecoded) {
      execute(state);
    }
    if (state->isFetched) {
      decode(state);
    }
    fetch(state);
    incrementPC(state);
  }

  printState(state);

  return EXIT_SUCCESS;
}
