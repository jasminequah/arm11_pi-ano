#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


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
} opcode_t;

typedef struct arm_decoded {
  cond_t cond;

  /* single bits */
  int isI;
  int isP;
  int isU;
  int isA;
  int isS;
  int isL;

  /* registers */
  uint32_t rn;
  uint32_t rd;
  uint32_t rs;
  uint32_t rm;

  opcode_t opcode;
  uint16_t operand2;

  /* incomplete .. */

}

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

int toDecimal(int* binary, int size) {


}


//Methods for execute:
void executeDataProcessing(int I, int opcode ,int S, int Rn, int Rd, int* Operand2) {

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
