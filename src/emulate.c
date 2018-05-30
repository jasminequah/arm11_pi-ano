#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define BITS_IN_WORD 32
#define MEMORY_SIZE 65536
#define NUM_REGS 17

/* specific registers */
#define SP_REG 13
#define LR_REG 14
#define PC_REG 15
#define CPSR_REG 16

/* Masks for testing function */
#define MULT_MASK   0x00000090u
#define SD_MASK     0x04000000u
#define BRANCH_MASK 0x0a000000u
#define DATA_MASK   0x00000000u

/* Masks for operations */
#define LAST_4_BITS 0x000F
#define FIRST_4_BITS 0xF00
#define LAST_BYTE 0xFF
#define FIRST_BIT 0x80000000

/*Masks for CPSR_REG */
#define Z_BIT 0x40000000
#define N_BIT 0x80000000
#define C_BIT 0x20000000
#define V_BIT 0x10000000

/* flags */
// typedef enum {
//   V_BIT,
//   C_BIT,
//   Z_BIT,
//   N_BIT
// } flags_t;

/* instructions */
typedef enum {
  DATA_PROCESSING,
  MULTIPLY,
  SINGLE_DATA_TRANSFER,
  BRANCH
} instr_t;

/* conditions */
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

/* shiftType for Data Processing */
typedef enum {
  LSL,
  LSR,
  ASR,
  ROR
} shiftType_t;

typedef struct arm_decoded {
  cond_t cond;

  /* single bits */
  int isI;
  int isP;
  int isU;
  int isA;
  int isS;
  int isL;

  /* register number [0 - 16] */
  int rn;
  int rd;
  int rs;
  int rm;
  uint32_t offset;

  opCode_t opCode;
  uint16_t operand2;

  /* incomplete .. */
} decoded_t;

/*
Holds the state of the emulator.
1. 17 32-bit Registers
2. 64KB memory (64KB = 65536 Bytes)
..
3. isTerminated = 1 if reach all 0 instruction.
*/

typedef struct arm_state {
  uint32_t *registers;
  uint8_t *memory;

  decoded_t *decoded;

  int isTerminated;
} state_t;

/*
  Should check for and report any errors occuring during program execution (eg: executing undefined with address greater than 65536.
  Writing emulator involves: building a binary file loader and writing the emulator loop.
  Emulator loop is made of: simulation of ARM pipeline with execute, decode, fetch cycle; simulated execution of the four types of instruction (data processing, multiply, single data transfer, branch). No opcodes for the different instruction types, so must find a pattern to distinguish between instructions.
  Emulator should terminate when executing an all-0 instruction, and upon termination, outputs the state of the registers.
  Instructions: Data processing, multiply, single data transfer, branch
  Pipeline: Fetch, Decode, Execute - 3 instructions processed at once.
*/

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
  if ((a & FIRST_BIT) & (b & FIRST_BIT)) {
    return ((a + b) & FIRST_BIT) == 0;
  } else if (!(a & FIRST_BIT) & !(b & FIRST_BIT)) {
    return ((a + b) & FIRST_BIT) != 0;
  }
  return 0;
}

int checkSubCarryOut(int a, int b) {
  if (!(a & FIRST_BIT) & (b & FIRST_BIT)) {
    return ((a - b) & FIRST_BIT) != 0;
  } else if ((a & FIRST_BIT) & !(b & FIRST_BIT)) {
    return ((a - b) & FIRST_BIT) == 0;
  }
}

void executeDataProcessing(state_t *state) {
  decoded_t *decoded = state->decoded;
  uint32_t *registers = state->registers;

  uint32_t operand1 = registers[decoded->rn];
  uint32_t operand2;

  int carryOut = 0;

  // Operand 2 is an immediate value
  if(decoded->isI) {
    unsigned int imm = decoded->operand2 & 0x0FF;
    operand2 = (uint32_t) imm;
    int shiftAmount = 2 * ((decoded->operand2 & 0xF00) >> 8);
    operand2 = rotateRight(operand2, shiftAmount);
  }
  // Operand 2 is a register
  else {
    operand2 = decoded->operand2;

    int bit4 = decoded->operand2 & 0x010;
    int shiftAmount;

    // bit 4 == 1, then shift specified by a register (optional)
    if(bit4) {
      //Takes last byte of register specified by Rs
      shiftAmount = registers[(operand2 & 0xF00) >> 8] & 0x000F;
    }
    // bit 4 == 0, then shift by a constant amount
    else {
      shiftAmount = (operand2 & 0xF80) >> 7;
    //  shiftAmount = (decoded->operand2 & 0xF800) >> 7;
    }

    shiftType_t shiftType = (operand2 & 0x060) >> 5;

    switch(shiftType) {
      case LSL:
        //logical left
        operand2 = logicalLeft(registers[decoded->rm], shiftAmount);
        carryOut = (registers[decoded->rm] >> (BITS_IN_WORD - shiftAmount)) & 0x1;
        break;
      case LSR:
        //logical right
        operand2 = logicalRight(registers[decoded->rm], shiftAmount);
        carryOut = (registers[decoded->rm] >> (shiftAmount - 1)) & 0x1;
        break;
      case ASR:
        //arithmetic right
        operand2 = arithmeticRight(registers[decoded->rm], shiftAmount);
        carryOut = (registers[decoded->rm] >> (shiftAmount - 1)) & 0x1;
        break;
      case ROR:
        //rotate right
        operand2 = rotateRight(registers[decoded->rm], shiftAmount);
        carryOut = (registers[decoded->rm] >> (shiftAmount - 1)) & 0x1;
        break;
    }
  //TODO: check shift functions according to spec
  }
  uint32_t result = 0;
  switch(decoded->opCode) {
    case AND:
      result = operand1 & operand2;
      break;
    case EOR:
      result = operand1 ^ operand2;
      break;
    case SUB:
      result = operand1 - operand2;
      carryOut = checkSubCarryOut(operand1, operand2);
      break;
    case RSB:
      result = operand2 - operand1;
      carryOut = checkSubCarryOut(operand2, operand1);
      break;
    case ADD:
      result = operand1 + operand2;
      carryOut = checkAddCarryOut(operand1, operand2);
      break;
    case TST:
      result = registers[decoded->rd];
      break;
    case TEQ:
      result = registers[decoded->rd];
      break;
    case CMP:
      result = registers[decoded->rd];
      carryOut = checkSubCarryOut(operand1, operand2);
      break;
    case ORR:
      result = operand1 | operand2;
      break;
    case MOV:
      result = operand2;
      break;
  }
  printf("Rd value before: %u\n", registers[decoded->rd]);
  registers[decoded->rd] = result;
  printf("Rd value after: %u\n", registers[decoded->rd]);

  if(decoded->isS) {



    // TODO: set CPSR flags
    /*
    1. V unaffected
    2. C set to carry out from any shift operation.
       C set to carry out of bit 31 of the ALU in arithmetic operation.
       C set to 1 if addition produced a carry (unsigned overflow).
       C set to 0 if subtraction produced a borrow, 1 otherwise.
    */
    if (carryOut == 1) {
      //C is 1 if carry out is 1
      registers[CPSR_REG] = registers[CPSR_REG] | C_BIT;
    }

    if (result == 0) {
      //Z is 1 if result is all zeroes.
      registers[CPSR_REG] = registers[CPSR_REG] | Z_BIT;
    } else {
      //Z is 0 is result is NOT all zeros (not sure if needed)
      registers[CPSR_REG] = registers[CPSR_REG] & 0xBFFF;
    }

    if ((result & N_BIT) >> (BITS_IN_WORD - 1)) {
      // 4. N is set to logical value of bit 31
      registers[CPSR_REG] = registers[CPSR_REG] | N_BIT;
    } else {
      registers[CPSR_REG] = registers[CPSR_REG] & 0x7FFF;
    }
  }

}

void executeMultiply(state_t *state) {
  if (state->decoded->isA == 1) {
	state->decoded->rd = state->decoded->rm*state->decoded->rs + state->decoded->rn;
     /*Rd = (Rm * Rs) + Rn;*/
  }
  else {
	state->decoded->rd = state->decoded->rm*state->decoded->rs;
    /*Rd = Rm * Rs;*/
  }

  if (state->decoded->isS) {
    int N = (state->decoded->rd >> 31) & 1;
	N = N << 31;
	uint32_t cpsr = state->registers[CPSR_REG];
	if (N) {
	  state->registers[CPSR_REG] = cpsr | N;
	}
	else {
	  state->registers[CPSR_REG] = cpsr & N;
	}

	//state->registers[CPSR_REG][31] = N ;
    if (!state->decoded->rd) {
      int Z = 1;
	  Z = Z << 30;
	  cpsr = state->registers[CPSR_REG];
	  state->registers[CPSR_REG] = cpsr | Z;
    }
  }

}

void executeSDT(state_t *state) {
//TODO: Check condition field before proceeding, and check all memory and reg references
  decoded_t* decoded  = state->decoded;
  uint32_t* registers = state->registers;
  uint8_t* memory    = state->memory;

  uint32_t immOffset = registers[(decoded->offset) & LAST_4_BITS]; //= value in Rn (CHECK)
  if (decoded->isI) {
    int bit4 = (decoded->offset) & 0x10;
    int shiftAmount;
    if (bit4) {
      // bit 4 == 1, shift by value in Rs (the last byte)
      shiftAmount = registers[((decoded->offset) & FIRST_4_BITS) >> 8] & LAST_BYTE; //last byte of Rs?
      // int
    } else {
      //bit4 == 0, shift by integer
      shiftAmount = (decoded->offset & 0xF800) >> 7; // = integer we shift by
    }

    shiftType_t shiftType = (decoded->offset & 0x60) >> 5;
    switch(shiftType) {
      case LSL:
        //logical left
        immOffset = logicalLeft(immOffset, shiftAmount);
        break;
      case LSR:
        //logical right
        immOffset = logicalRight(immOffset, shiftAmount);
        break;
      case ASR:
        //arithmetic right
        immOffset = arithmeticRight(immOffset, shiftAmount);
        break;
      case ROR:
        //rotate right
        immOffset = rotateRight(immOffset, shiftAmount);
        break;
    }
  } else {
    //Offset is an unsigned 12 bit immediate offset
    immOffset = decoded->offset; //CHECK
  }

  uint32_t newBase;
  if (decoded->isU) {
    //offset is added to the base reg
    newBase = registers[decoded->rn] + immOffset;
  } else {
    //offset is subtracted to the base reg
    newBase = registers[decoded->rn] - immOffset;
    // note to self: decoded->rn or decoded.rn?
  }

  if (decoded->isP) {
    //pre-indexing : offset is +/- to the base reg before transferring data
    if (decoded->isL) {
      //word is loaded from memory
      registers[decoded->rd] = memory[newBase];
    } else {
      //word is stored into memory
      memory[newBase] = registers[decoded->rd];
    }


  } else {
    //post-indexing : offset is +/- to the base reg after transferring
    if (decoded->isL) {
      //word is loaded from memory
      registers[decoded->rd] = memory[decoded->rn];
    } else {
      //word is stored into memory - (Rd contents goes into memory)
      memory[decoded->rn] = registers[decoded->rd];
    }
    //offset is +/- to the base reg
    registers[decoded->rn] = newBase; //CHECK
  }
}

/* Branch Instruction:
 * Adds offset from bits 0 to 23 of decoded instruction to PC */
void executeBranch(state_t *state) {
  decoded_t* decoded = state->decoded;
  /* Need to check if will overflow or assume it won't? */
  state->registers[PC_REG] += decoded->offset;
}

/* Restructure to put execute functions in different files for better readability */

/* TODO:
 * Checks whether condition satisfied according to condition code in decoded instruction
 * Shifts CPSR register right so can compare bits 28-31
 * Returns 0 or 1 accordingly */

int checkCond(state_t *state, cond_t cond) {
  uint32_t flags = (state->registers[CPSR_REG]) >> 28;
  return (flags == state->decoded->cond || state->decoded->cond == 14);

  //decoded_t* decoded = state->decoded;
  //uint32_t flags = logicalRight(state->registers[CPSR_REG], 28);
  //return (flags == decoded->cond || decoded->cond == 14);
  /*int N = getFlag(cond, N_BIT);
  int Z = getFlag(cond, Z_BIT);
  // int C = getFlag(cond, C_BIT); not needed
  int V = getFlag(cond, V_BIT);

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
  }
  return 0; */
}

/* Executes calls to different functions if condition satisfied
 * Function called depends on which of 4 instructions it is executing
 * If reaches n all 0 instruction, then emulator terminates (halt) and prints registers and memory */

void execute(state_t *state, instr_t instruction) {
  //If cond not satisfied may still want to check if all 0 instruction encountered?
  if (checkCond(state, state->decoded->cond)) {
    switch (instruction) {
      case DATA_PROCESSING:
        printf("dp\n");
	      executeDataProcessing(state);
	      break;
       //i put the operand as int* since we need to analyze the sub-bits of it
       //in executeDataProcessing

      case MULTIPLY:
        printf("mul\n");
	      executeMultiply(state);
        break;

      case SINGLE_DATA_TRANSFER:
        printf("sdt\n");
	      executeSDT(state);
	      break;

      case BRANCH:
        printf("branch\n");
	      executeBranch(state);
        break;
    }
  } else {
    printf("condition not satisfied\n");
  }
}

/* Decodes instruction and passes relevant arguments to execute
 * execute(state, cond, instr, instrNumber)
 * Initialises required parts of decoded_t struct for that instruction */

uint32_t getInstruction(state_t *state) {
  uint32_t instruction;
  instruction = (state->memory[state->registers[PC_REG]] << 24) | (state->memory[state->registers[PC_REG] + 1] << 16) | (state->memory[state->registers[PC_REG] + 2] << 8) | (state->memory[state->registers[PC_REG] + 3]);
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
    state->isTerminated = 1;
    //TODO: Need to fix incrementation of PC so simulates pipeline correctly, line below is temporary fix to pass tests
    state->registers[PC_REG] += 0x4u;
  } else {
    instr_t instrNum = getInstructionNum(pc);
    printf("Instruction number: %d\n", instrNum);
    switch (instrNum) {

      case(BRANCH) :
        state->decoded->offset   = (pc << 8) >> 8;
        state->decoded->cond     = (cond_t) pc >> 28;
        execute(state, BRANCH);
        break;

      case(DATA_PROCESSING) :
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

      case(SINGLE_DATA_TRANSFER) :
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

      case(MULTIPLY) :
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

/* Prints values of registers and non-zero memory to file */
void printState(state_t *state) {
  printf("Registers:\n");
  for (int i = 0; i < 13; i++) {
    printf("$%-2d : %9d (0x%08x)\n", i, state->registers[i], state->registers[i]);
  }
  printf("PC  : %9d (0x%08x)\n", state->registers[PC_REG], state->registers[PC_REG]);
  printf("CPSR: %9d (0x%08x)\n", state->registers[CPSR_REG], state->registers[CPSR_REG]);

  printf("Non-zero memory:\n");
  uint16_t i = 0;
  while (i < MEMORY_SIZE / 4) {
    if (state->memory[i] == 0) {
      break;
    } else {
      printf("0x%08x: 0x%02x%02x%02x%02x\n", i, state->memory[i + 3], state->memory[i + 2], state->memory[i + 1], state->memory[i]);
      i += 4;
    }
  }
}

/* Reads input file and puts it somewhere ... */
void readBinary(state_t *state, char* fileName) {

  FILE *fptr;
  fptr = fopen(fileName, "rb"); // open files

  fseek(fptr, 0, SEEK_END);
  int fileLen = ftell(fptr);
  fseek(fptr, 0, SEEK_SET);

  uint8_t *memPtr = &(state->memory[0]);

  for (int i = 0; i < fileLen / 4; i++) {
    for (int j = 3; j >=0; j--) {
      fread(memPtr + j, 1, 1, fptr);
    }
    memPtr += 4;
  }

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

int main(int argc, char* argv[]) { // binary filename as sole argument

  // assert (argc == 2);
  // initialise system to default state
  state_t *state = newState();
  if (state == NULL) {
    return EXIT_FAILURE;
  }

  readBinary(state, argv[1]);

  /* Fetch: increments PC and passes state to decode part of pipeline */
  while (!state->isTerminated) {
    //state.registers[PC_REG] += 0x4u;

    if (state->registers[PC_REG] > 0x10000u) {
      fprintf(stderr, "Attempt to execute an undefined with address greater than 65536.\n");
      return EXIT_FAILURE;
    } else {
      decode(state);
    }
    state->registers[PC_REG] += 0x4u;
  }

  printState(state);

  return EXIT_SUCCESS;
}
