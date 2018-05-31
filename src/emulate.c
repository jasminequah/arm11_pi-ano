#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define BITS_IN_WORD 32
#define MEMORY_SIZE  65536
#define NUM_REGS     17
#define PC_INCREMENT 0x4

/* Specific registers */
#define SP_REG   13
#define LR_REG   14
#define PC_REG   15
#define CPSR_REG 16

/* Masks for instruction code */
#define FIRST_4_BITS    0x0000000F
#define THIRD_4_BITS    0x00000F00
#define FIRST_BYTE      0x000000FF
#define BIT_31          0x80000000
#define BIT_4           0x00000010
#define BIT_0           0x00000001
#define SHIFT_TYPE_MASK 0x00000060
#define SHIFT_AMT_MASK  0x00000F80

/* Masks for CPSR_REG */
#define N_BIT 0x80000000
#define Z_BIT 0x40000000
#define C_BIT 0x20000000
#define V_BIT 0x10000000

/* Instructions */
typedef enum {
  DATA_PROCESSING,
  MULTIPLY,
  SINGLE_DATA_TRANSFER,
  BRANCH
} instr_t;

/* Conditions */
typedef enum {
  EQ = 0,
  NE = 1,
  GE = 10,
  LT = 11,
  GT = 12,
  LE = 13,
  AL = 14,
} cond_t;

/* OpCode for Data Processing */
typedef enum {
  AND = 0,
  EOR = 1,
  SUB = 2,
  RSB = 3,
  ADD = 4,
  TST = 8,
  TEQ = 9,
  CMP = 10,
  ORR = 12,
  MOV = 13,
} opCode_t;

/* ShiftType for Data Processing */
typedef enum {
  LSL,
  LSR,
  ASR,
  ROR
} shiftType_t;

/* components of decoded instruction */
typedef struct arm_decoded {
  cond_t cond;

  int isI;
  int isP;
  int isU;
  int isA;
  int isS;
  int isL;

  int rn;
  int rd;
  int rs;
  int rm;

  int32_t offset;
  opCode_t opCode;
  uint16_t operand2;

} decoded_t;

/* Holds the state of the emulator. */
typedef struct arm_state {
  uint32_t *registers;
  uint8_t *memory;

  decoded_t *decoded;

  int isTerminated;
} state_t;

/* Prints values of registers and non-zero memory to file */

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

/* Function to print values stored in decoded_t struct to aid debugging */

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

// Methods for execute:

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
      printf("Error: Out of bounds memory access at address 0x%08x\n", newBase);
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
        state->decoded->cond     = (cond_t) pc >> 28;
        execute(state, BRANCH);
        break;
      case DATA_PROCESSING:
        state->decoded->cond     = (cond_t) pc >> 28;
        state->decoded->isI      = pc & 0x02000000u;
        state->decoded->isS      = pc & 0x00100000u;
        state->decoded->opCode   = (opCode_t) ((pc << 7) >> 28);
        state->decoded->rn       = (pc << 12) >> 28;
        state->decoded->rd       = (pc << 16) >> 28;
        state->decoded->rm       = (pc << 28) >> 28;
        state->decoded->operand2 = (uint16_t)pc;
        execute(state, DATA_PROCESSING);
        break;
      case SINGLE_DATA_TRANSFER:
        state->decoded->cond     = (cond_t) pc >> 28;
        state->decoded->isI      = pc & 0x02000000u;
        state->decoded->isP      = pc & 0x01000000u;
        state->decoded->isU      = pc & 0x00800000u;
        state->decoded->isL      = pc & 0x00100000u;
        state->decoded->rn       = (pc << 12) >> 28;
        state->decoded->rd       = (pc << 16) >> 28;
        state->decoded->offset   = (pc << 20) >> 20;
        execute(state, SINGLE_DATA_TRANSFER);
        break;
      case MULTIPLY:
        state->decoded->cond     = (cond_t) pc >> 28;
        state->decoded->isA      = pc & 0x00200000u;
        state->decoded->isS      = pc & 0x00100000u;
        state->decoded->rd       = (pc << 12) >> 28;
        state->decoded->rn       = (pc << 16) >> 28;
        state->decoded->rs       = (pc << 20) >> 28;
        state->decoded->rm       = (pc << 28) >> 28;
        execute(state, MULTIPLY);
        break;
    }
  }
}

/* Reads input file and puts it somewhere ... */
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
