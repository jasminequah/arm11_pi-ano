#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define BITS_IN_WORD 32

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
  uint32_t registers[17];
  uint8_t memory[65536];

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

void readInstructions(char instructions[]) {
  //Read instructions from input file into instruction array representing memory

}

//not sure if this is needed. - chris
int binToDecimal(unsigned binary, int size) {
  return 0;
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

void executeDataProcessing(state_t *state) {

  decoded_t *decoded = state->decoded;
  uint32_t *registers = state->registers;

 // uint32_t fstOperand = registers[decoded->rn];

  // Operand 2 is an immediate value
  if(decoded->isI) {
    unsigned Imm = decoded->operand2 & 0xFF;
    uint32_t value = (uint32_t) Imm;
    int shiftAmount = 2 * ((decoded->operand2 & 0xF00) >> 8);
    value = rotateRight(value, shiftAmount);
  }
  // Operand 2 is a register
  else {
    uint32_t value = registers[decoded->rm];


    int bit4 = decoded->operand2 & 0x10;
    int shiftAmount;

    // bit 4 == 1, then shift specified by a register (optional)
    if(bit4) {
      shiftAmount = registers[decoded->rs] & 0xFF;
    }
    // bit 4 == 0, then shift by a constant amount
    else {
      shiftAmount = (decoded->operand2 & 0xF800) >> 7;
    }

    shiftType_t shiftType = (decoded->operand2 & 0x60) >> 5;

    switch(shiftType) {
      case LSL:
        //logical left
        value = logicalLeft(value, shiftAmount);
        break;
      case LSR:
        //logical right
        value = logicalRight(value, shiftAmount);
        break;
      case ASR:
        //arithmetic right
        value = arithmeticRight(value, shiftAmount);
        break;
      case ROR:
        //rotate right
        value = rotateRight(value, shiftAmount);
        break;
    }

  }

  switch(decoded->opCode) {
    case AND:

      break;
    case EOR:

      break;
    case SUB:

      break;
    case RSB:

      break;
    case ADD:

      break;
    case TST:

      break;
    case TEQ:

      break;
    case CMP:

      break;
    case ORR:

      break;
    case MOV:

      break;
  }

  if(decoded->isS) {
    // set CPSR flags
    /*
    1. V unaffected
    2. C set to carry out from any shift operation.
       C set to carry out of bit 31 of the ALU in arithmetic operation.
       C set to 1 if addition produced a carry (unsigned overflow).
       C set to 0 if subtraction produced a borrow, 1 otherwise.
    3. Z is 0 if result is all zeroes.
    4. N is set to logical value of bit 31
    */
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

  uint32_t immOffset = registers[(decoded->offset) & 0x000F]; //= value in Rn (CHECK)
  if (decoded->isI) {
    int bit4 = (decoded->offset) & 0x10;
    int shiftAmount;
    if (bit4) {
      // bit 4 == 1, shift by value in Rs (the last byte)
      shiftAmount = registers[((decoded->offset) & 0xF00) >> 8] & 0xFF; //last byte of Rs?
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
void executeBranch(state_t* state) {
  decoded_t* decoded = state->decoded;
  /* Need to check if will overflow or assume it won't? */
  state->registers[PC_REG] += decoded->offset;
}

/* Restructure to put execute functions in different files for better readability */

/* TODO:
 * Checks whether condition satisfied according to condition code in decoded instruction
 * Shifts CPSR register right so can compare bits 28-31
 * Returns 0 or 1 accordingly */

int checkCond(state_t* state, cond_t cond) {
  decoded_t* decoded = state->decoded;
  uint32_t flags = logicalRight(state->registers[CPSR_REG], 28);
  return flags & decoded->cond;
}

/* Executes calls to different functions if condition satisfied
 * Function called depends on which of 4 instructions it is executing
 * If reaches n all 0 instruction, then emulator terminates (halt) and prints registers and memory */

void execute(state_t* state, int instrNumber) {
  //If cond not satisfied may still want to check if all 0 instruction encountered?
  if (checkCond(state, state->decoded->cond)) {
    switch (instrNumber) {
      case 0 : executeDataProcessing(state);
	       break;
      /* case 0 : executeDataProcessing(intr[25], toDecimal(&instr[21], 4) ,instr[20],
       toDecimal(&instr[16], 4), toDecimal(&instr[12], 4), &instr[0]); */
       //i put the operand as int* since we need to analyze the sub-bits of it
       //in executeDataProcessing
               break;

      case 1 : executeMultiply(state);
               break;
      /* case 1 : executeMultiply(state);
                  break; */

      case 2 : executeSDT(state);
	       break;
      /* case 2: executeSDT(instr[25], instr[24], instr[23], instr[20],
        toDecimal(&instr[16], 4), toDecimal(&instr[12], 4), &instr[0]);
              break; */

      case 3 : executeBranch(state);
               break;
    }
  }

}

/* Decodes instruction and passes relevant arguments to execute
 * execute(state, cond, instr, instrNumber)
 * Initialises required parts of decoded_t struct for that instruction */

void decode(state_t* state) {
  uint32_t pc = state->registers[PC_REG];
  if (!pc) {
    state->isTerminated = 1;
  } else if (pc & BRANCH_MASK) {
    decoded_t decoded;
    decoded.offset = (pc << 8) >> 8;
    decoded.cond   = (cond_t) ((int) (pc >> 28));
    state->decoded = &decoded;
    execute(state, (int) BRANCH);
  } else if (pc & MULT_MASK) {
    decoded_t decoded;
    decoded.cond     = (cond_t) ((int) (pc >> 28));
    decoded.isA      = pc & 0x00200000u;
    decoded.isS      = pc & 0x00100000u;
    decoded.rd       = (pc << 12) >> 28;
    decoded.rn       = (pc << 16) >> 28;
    decoded.rs       = (pc << 20) >> 28;
    decoded.rm       = (pc << 28) >> 28;
    state->decoded   = &decoded;
    execute(state, (int) MULTIPLY);
  } else if (pc & SD_MASK) {
    decoded_t decoded;
    decoded.cond     = (cond_t) ((int) (pc >> 28));
    decoded.isI      = pc & 0x02000000u;
    decoded.isP      = pc & 0x01000000u;
    decoded.isU      = pc & 0x00800000u;
    decoded.isL      = pc & 0x00100000u;
    decoded.rn       = (pc << 12) >> 28;
    decoded.rd       = (pc << 16) >> 28;
    decoded.offset   = (pc << 20) >> 20;
    state->decoded   = &decoded;
    execute(state, (int) SINGLE_DATA_TRANSFER);
  } else if (pc & DATA_MASK) {
    decoded_t decoded;
    decoded.cond     = (cond_t) ((int) (pc >> 28));
    decoded.isI      = pc & 0x02000000u;
    decoded.isS      = pc & 0x00100000u;
    decoded.opCode   = (opCode_t) ((int) (pc << 7) >> 28);
    decoded.operand2 = (uint16_t) pc;
    state->decoded   = &decoded;
    execute(state, (int) DATA_PROCESSING);
  } else {
    fprintf(stderr, "Unrecognised instruction.\n");
    // TODO: Exit failure
    return;
  }
}

/* Prints values of registers and non-zero memory to file */
void printState(state_t *state) {
  printf("Registers:\n");
  for (int i = 0; i < 13; i++) {
    printf("$%d : %d (0x%x)\n", i, state.registers[i], state.registers[i]);
  }
  printf("PC : %d (0x%x)\n", state.registers[PC_REG], state.registers[PC_REG]);
  printf("CPSR : %d (0x%x)\n", state.registers[CPSR_REG], state.registers[CPSR_REG]);

  printf("Non-zero memory:");
  uint8_t *i = 0;
  while (i < sizeof(state.memory) / 4) {
    if (state.memory[i] == 0) {
      break;
    } else {
      for (j = 3; j <= 0; j--) {
        printf("0x%x: 0x%x", i, state.memory[i]);
      }
      printf("0x%x: 0x%x", i, state.memory[i]);
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


int main(int argc, char* argv[]) { // binary filename as sole argument

  assert (argc2 == 2);

  // initialise system to default state
  state_t state = {{0}, {0}, 0, 0};

  readBinary(&state, argv[1]);

  /* Fetch: increments PC and passes state to decode part of pipeline */
  while (!state.isTerminated) {
    state.registers[PC_REG] += 0x4u;

    if (state.registers[PC_REG] > 0x10000u) {
      fprintf(stderr, "Attempt to execute an undefined with address greater than 65536.\n");
      return EXIT_FAILURE;
    } else {
      decode(&state);
    }
  }

  printState(state);

  return EXIT_SUCCESS;
}
