#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define BITS_IN_WORD 32
#define MEMORY_SIZE 65536

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
  uint8_t memory[MEMORY_SIZE];

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
  printf("hello");
  decoded_t *decoded = state->decoded;
  uint32_t *registers = state->registers;

  uint32_t operand1 = registers[decoded->rn];
  uint32_t operand2;

  // Operand 2 is an immediate value
  if(decoded->isI) {
    unsigned Imm = decoded->operand2 & 0xFF;
    operand2 = (uint32_t) Imm;
    int shiftAmount = 2 * ((decoded->operand2 & 0xF00) >> 8);
    operand2 = rotateRight(operand2, shiftAmount);
  }
  // Operand 2 is a register
  else {
    operand2 = registers[decoded->rm];


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
        operand2 = logicalLeft(operand2, shiftAmount);
        break;
      case LSR:
        //logical right
        operand2 = logicalRight(operand2, shiftAmount);
        break;
      case ASR:
        //arithmetic right
        operand2 = arithmeticRight(operand2, shiftAmount);
        break;
      case ROR:
        //rotate right
        operand2 = rotateRight(operand2, shiftAmount);
        break;
    }

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
      result = (uint32_t) ((int) operand1 - (int) operand2);
      break;
    case RSB:
      result = (uint32_t) ((int) operand2 - (int) operand1);
      break;
    case ADD:
      result = (uint32_t) ((int) operand1 + (int) operand2);
      break;
    case TST:
      break;
    case TEQ:
      break;
    case CMP:
      break;

    case ORR:
      result = operand1 | operand2;
      break;
    case MOV:
      result = operand2;
      break;
  }
  printf("Rd value before: %u", registers[decoded->rd]);
  registers[decoded->rd] = result;
  printf("Rd value after: %u", registers[decoded->rd]);

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
  decoded_t* decoded = state->decoded;
  uint32_t flags = logicalRight(state->registers[CPSR_REG], 28);
  return (flags == decoded->cond || decoded->cond == 14);
}

/* Executes calls to different functions if condition satisfied
 * Function called depends on which of 4 instructions it is executing
 * If reaches n all 0 instruction, then emulator terminates (halt) and prints registers and memory */

void execute(state_t *state, int instrNumber) {
  //If cond not satisfied may still want to check if all 0 instruction encountered?
  if (checkCond(state, state->decoded->cond)) {
    switch (instrNumber) {
      case 0 : printf("dp\n");
	       executeDataProcessing(state);
	       break;
      /* case 0 : executeDataProcessing(intr[25], toDecimal(&instr[21], 4) ,instr[20],
       toDecimal(&instr[16], 4), toDecimal(&instr[12], 4), &instr[0]); */
       //i put the operand as int* since we need to analyze the sub-bits of it
       //in executeDataProcessing
               break;

      case 1 : printf("mul\n");
	       executeMultiply(state);
               break;
      /* case 1 : executeMultiply(state);
                  break; */

      case 2 : printf("sdt\n");
	       executeSDT(state);
	       break;
      /* case 2: executeSDT(instr[25], instr[24], instr[23], instr[20],
        toDecimal(&instr[16], 4), toDecimal(&instr[12], 4), &instr[0]);
              break; */

      case 3 : printf("branch\n");
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
  return state->memory[state->registers[PC_REG]];
  //uint32_t byte =
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
  } else {
    instr_t instrNum = getInstructionNum(pc);
    decoded_t decoded;
    printf("%d", instrNum);
    switch (instrNum) {

      case(BRANCH) :
        decoded.offset = (pc << 8) >> 8;
        decoded.cond   = (cond_t) ((int) (pc >> 28));
        state->decoded = &decoded;
        execute(state, (int) BRANCH);
        break;

      case(DATA_PROCESSING) :
        printf("I'm in data processing \n");
        decoded.cond = (cond_t)((int)(pc >> 28));
        decoded.isI = pc & 0x02000000u;
        decoded.isS = pc & 0x00100000u;
        decoded.opCode = (opCode_t)((int)(pc << 7) >> 28);
        decoded.operand2 = (uint16_t)pc;
        state->decoded = &decoded;
        execute(state, (int)DATA_PROCESSING);
        break;

      case(SINGLE_DATA_TRANSFER) :
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
        break;

      case(MULTIPLY) :
        decoded.cond     = (cond_t) ((int) (pc >> 28));
        decoded.isA      = pc & 0x00200000u;
        decoded.isS      = pc & 0x00100000u;
        decoded.rd       = (pc << 12) >> 28;
        decoded.rn       = (pc << 16) >> 28;
        decoded.rs       = (pc << 20) >> 28;
        decoded.rm       = (pc << 28) >> 28;
        state->decoded   = &decoded;
        execute(state, (int) MULTIPLY);
        break;
      }
  }
}

/* Prints values of registers and non-zero memory to file */
void printState(state_t *state) {
  printf("Registers:\n");
  for (int i = 0; i < 13; i++) {
    printf("$%d : %d (0x%x)\n", i, state->registers[i], state->registers[i]);
  }
  printf("PC : %d (0x%x)\n", state->registers[PC_REG], state->registers[PC_REG]);
  printf("CPSR : %d (0x%x)\n", state->registers[CPSR_REG], state->registers[CPSR_REG]);

  printf("Non-zero memory:\n");
  uint16_t i = 0;
  while (i < MEMORY_SIZE / 4) {
    if (state->memory[i] == 0) {
      break;
    } else {
      printf("0x%x: 0x%02x%02x%02x%02x\n", i, state->memory[i + 3], state->memory[i + 2], state->memory[i + 1], state->memory[i]);
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


int main(int argc, char* argv[]) { // binary filename as sole argument

  //assert (argc == 2);
  // initialise system to default state
  state_t state = {{0}, {0}, 0, 0};

  readBinary(&state, argv[1]);

  /* Fetch: increments PC and passes state to decode part of pipeline */
  while (!state.isTerminated) {
    //state.registers[PC_REG] += 0x4u;

    if (state.registers[PC_REG] > 0x10000u) {
      fprintf(stderr, "Attempt to execute an undefined with address greater than 65536.\n");
      return EXIT_FAILURE;
    } else {
      decode(&state);
    }
    state.registers[PC_REG] += 0x4u;
  }

  printState(&state);

  return EXIT_SUCCESS;
}
