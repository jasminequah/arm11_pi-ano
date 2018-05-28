#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BITS_IN_WORD 32

/* specific registers */
#define SP_REG 13
#define LR_REG 14
#define PC_REG 15
#define CPSR_REG 16

/* instructions */
typedef enum {
  DATA_PROCESSING,
  MULTIPLY,
  SINGLE_DATA_TRANSFER,
  BRANCH
} instr_t;

/* conditions */
typedef enum {
  EQ = 0;
  NE = 1;
  GE = 10;
  LT = 11;
  GT = 12;
  LE = 13;
  AL = 14;
} cond_t;

/* OpCode for Data Processing */
typedef enum {
  AND = 0;
  EOR = 1;
  SUB = 2;
  RSB = 3;
  ADD = 4;
  TST = 8;
  TEQ = 9;
  CMP = 10;
  ORR = 12;
  MOV = 13;
} opCode_t;

/* shiftType for Data Processing */
typedef enum {
  LSL,
  LSR,
  ASR,
  ROR
} shiftType_t

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
  uint32_t rn;
  uint32_t rd;
  uint32_t rs;
  uint32_t rm;

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

void fetch(int* pcReg, char instructions[]) {
  //Fetch (loads PC), passes to decode
}

int decode(int* decodedInst) {
  //Decode, passes to execute

}

void execute(int* instr, int instrNumber) {
  //Execute calls different functions depending on which of 4 instructions it is executing
  //If reaches an all 0 instruction, then emulator terminates (halt) and prints registers and memory
// CHECK IF COND IS TRUE, IF IT IS EXECUTE, ELSE IGNORED

  switch (instrNumber) {
    case 0 : executeDataProcessing(intr[25], toDecimal(&instr[21], 4) ,instr[20],
     toDecimal(&instr[16], 4), toDecimal(&instr[12], 4), &instr[0]);
     //i put the operand as int* since we need to analyze the sub-bits of it
     //in executeDataProcessing
            break;
    case 1 : executeMultiply(instr[21], instr[20], toDecimal(&instr[16], 4),
    toDecimal(&instr[12], 4), toDecimal(&instr[8], 4), toDecimal(&instr[0], 4));
            break;
    case 2: executeSDT(instr[25], instr[24], instr[23], instr[20],
      toDecimal(&instr[16], 4), toDecimal(&instr[12], 4), &instr[0]);
            break;
    case 3 : executeBranch(&instr[0]);
            break;
  }

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
  return (n >> d) | (n << (BITS_IN_WORD) - rotation);
}

void executeDataProcessing(state_t state) {

  decoded_t decoded = state.decoded;
  uint32_t *registers = state.registers;

  uint32_t fstOperand = registers[decoded.rn];

  // Operand 2 is an immediate value
  if(decoded.isI) {
    unsigned Imm = decoded.operand2 & 0xFF;
    uint32_t value = (uint32_t) Imm;
    int shiftAmount = 2 * ((decoded.operand2 & 0xF00) >> 8);
    value = rotateRight(value, shiftAmount);
  }
  // Operand 2 is a register
  else {
    uint32_t value = registers[decoded.rm];


    int bit4 = decoded.operand2 & 0x10;

    // bit 4 == 1, then shift specified by a register (optional)
    if(bit4) {
      int shiftAmount = registers[decoded.rs] & 0xFF;
    }
    // bit 4 == 0, then shift by a constant amount
    else {
      int shiftAmount = (decoded.operand2 & 0xF800) >> 7;
    }

    shiftType_t shiftType = (decoded.operand2 & 0x60) >> 5;

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

  if(decoded.isS) {
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

void executeMultiply(int A, int S, int Rd, int Rn, int Rs, int Rm) {
	if (A == 1) {
		Rd = (Rm * Rs) + Rn;
	}
	else {
		Rd = Rm * Rs;
	}
	if (S == 1) {
		int N = (Rd >> 31) & 1;
		/*cpsrReg[31] = N*/
		if (Rd == 0) {
			int Z = 1;
			/*cpsrReg[30] = Z*/
		}
	}

}

void executeSDT(int I, int P, int U, int L, int Rn, int Rd, int* offset) {

}

void executeBranch(int* offset) {

}



void printRegisters((int*) registers[]) {
  //Prints value of each register to file
  printf("Registers:\n");
  for (int i = 0; i < s; i++) {
    printf("$%d : %d (%p)\n", i, *registers[i], registers[i]);
  }
  printf("PC : %d (%p)\n", *registers[15], registers[15]);
  printf("CPSR : %d (%p)\n", *registers[16], registers[16]);

}

void printMemory(void) {
  //Prints non-zero memory
  printf("Non-zero memory:\n");

}

// reads input file and puts it somewhere ..
int readBinary() {

}


int main(int argc, char* argv[]) { // binary filename as sole argument

  FILE *fptr;
  fopen(argv[1], "rb"); // open files


  //Need to get instruction file from main?
  char instructions[]; //Read in instruction file
  readInstructions(instructions);

  (int*) registers[17]; //17 32-bit registers, store pointers of each register in array
  //Or use C struct?
  char cpsrReg[32];
  registers[16] = &cpsrReg;

  //While still have instructions, pass to fetch function

  printRegisters(registers);
  printMemory();

  return EXIT_SUCCESS;
}
