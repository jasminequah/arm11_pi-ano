#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "definitions.h"
#include "utilities.h"
#include "ioutils.h"


void executeDataProcessing(state_t *state) {
  decoded_t *decoded = state->decoded;
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
  decoded_t* decoded  = state->decoded;
  uint32_t* registers = state->registers;
  if (state->decoded->isA) {
    registers[decoded->rd] = registers[decoded->rm] * registers[decoded->rs] + registers[decoded->rn];
  } else {
    registers[decoded->rd] = registers[decoded->rm] * registers[decoded->rs];
  }

  if (state->decoded->isS) {
    if (state->decoded->rd >> (BITS_IN_WORD - 1)) {
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

void checkGPIOPins(uint32_t newBase) {
  if (newBase == PIN_20_TO_29) {
    printf("One GPIO pin from 20 to 29 has been accessed\n");
  } else if (newBase == PIN_10_TO_19) {
    printf("One GPIO pin from 10 to 19 has been accessed\n");
  } else if (newBase == PIN_0_TO_9) {
    printf ("One GPIO pin from 0 to 9 has been accessed\n");
  } else if (newBase == PIN_OFF) {
    printf("PIN OFF\n");
  } else if (newBase == PIN_ON) {
    printf("PIN ON\n");
  } else {
    printf("Error: Out of bounds memory access at address 0x%08x\n", newBase);
  }
  return;
}

void executeSDT(state_t *state) {
  decoded_t* decoded       = state->decoded;
  uint32_t* registers      = state->registers;
  uint8_t* memory          = state->memory;
  uint32_t baseRegContents = registers[decoded->rn];

  if (decoded->rn == PC_REG) {
    baseRegContents += 0x8;
  }

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
      checkGPIOPins(newBase);
      return;
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
  decoded_t* decoded = state->decoded;
  state->registers[PC_REG] = ((int) state->registers[PC_REG]) + decoded->offset + 0x4u;
}

/* Executes calls to different functions if condition satisfied
 * Function called depends on which of 4 instructions it is executing
 * If reaches n all 0 instruction, then emulator terminates (halt) and prints registers and memory */

void execute(state_t *state, instr_t instruction) {
  //printf("Instruction number: %u\n", instruction);
  //If cond not satisfied may still want to check if all 0 instruction encountered?
  if (checkCond(state, state->decoded->cond)) {
    switch (instruction) {
      case DATA_PROCESSING:
	executeDataProcessing(state);
	break;

      case MULTIPLY:
	executeMultiply(state);
        break;

      case SINGLE_DATA_TRANSFER:
	executeSDT(state);
	break;

      case BRANCH:
	executeBranch(state);
        break;
    }

  }
}

void decode(state_t *state) {
  uint32_t pc = getInstruction(state);
  if (!pc) {
    state->isTerminated       = 1;
    state->registers[PC_REG] += 0x4u;
  } else {
    instr_t instrNum = getInstructionNum(pc);
    switch (instrNum) {
      case BRANCH:
        state->decoded->offset   = ((int) (pc << 8)) >> 6;
        state->decoded->cond     = (cond_t) pc >> PC_SHIFT;
        execute(state, BRANCH);
        break;
      case DATA_PROCESSING:
        state->decoded->cond     = (cond_t) pc >> PC_SHIFT;
        state->decoded->isI      = pc & IS_I;
        state->decoded->isS      = pc & IS_S;
        state->decoded->opCode   = (opCode_t) ((pc << 7) >> (BITS_IN_WORD - 4));
        state->decoded->rn       = (pc << RN_SHIFT) >> (BITS_IN_WORD - 4);
        state->decoded->rd       = (pc << RD_SHIFT) >> (BITS_IN_WORD - 4);
        state->decoded->rm       = (pc << RM_SHIFT) >> (BITS_IN_WORD - 4);
        state->decoded->operand2 = (uint16_t)pc;
        execute(state, DATA_PROCESSING);
        break;
      case SINGLE_DATA_TRANSFER:
        state->decoded->cond     = (cond_t) pc >> PC_SHIFT;
        state->decoded->isI      = pc & IS_I;
        state->decoded->isP      = pc & IS_P;
        state->decoded->isU      = pc & IS_U;
        state->decoded->isL      = pc & IS_L;
        state->decoded->rn       = (pc << RN_SHIFT) >> (BITS_IN_WORD - 4);
        state->decoded->rd       = (pc << RD_SHIFT) >> (BITS_IN_WORD - 4);
        state->decoded->offset   = (pc << OFFSET_SHIFT) >> OFFSET_SHIFT;
        execute(state, SINGLE_DATA_TRANSFER);
        break;
      case MULTIPLY:
      /*The positions of rd and rn are interchanged in Multiply,
      hence the shift numbers are interchanged*/
        state->decoded->cond     = (cond_t) pc >> PC_SHIFT;
        state->decoded->isA      = pc & IS_A;
        state->decoded->isS      = pc & IS_S;
        state->decoded->rd       = (pc << RN_SHIFT) >> (BITS_IN_WORD - 4);
        state->decoded->rn       = (pc << RD_SHIFT) >> (BITS_IN_WORD - 4);
        state->decoded->rs       = (pc << RS_SHIFT) >> (BITS_IN_WORD - 4);
        state->decoded->rm       = (pc << RM_SHIFT) >> (BITS_IN_WORD - 4);
        execute(state, MULTIPLY);
        break;
    }
  }
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
    if (state->registers[PC_REG] > MEMORY_SIZE) {
      state->registers[PC_REG] = state->registers[PC_REG] % MEMORY_SIZE;
      fprintf(stderr, "Error: Out of bounds memory access at address 0x%08x\n", state->registers[PC_REG]);
      return EXIT_FAILURE;
    } else {
      decode(state);
    }

    state->registers[PC_REG] += PC_INCREMENT;
  }

  printState(state);

  return EXIT_SUCCESS;
}
