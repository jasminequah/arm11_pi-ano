#include <stdlib.h>
#include <stdio.h>

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

void execute(int* decodedInst, int instrNumber) {
  //Execute calls different functions depending on which of 4 instructions it is executing
  //If reaches an all 0 instruction, then emulator terminates (halt) and prints registers and memory
// CHECK IF COND IS TRUE, IF IT IS EXECUTE, ELSE IGNORED  

switch (instrNumber) {
    case 0 : executeDataProcessing(int I, toDecimal(&decodedInst[1], 4) , int, S, int* Rn, int* Rd, int Operand2);
            break;
    case 1 : executeMultiply(int A, int S, int Rd, int* Rn, int* Rs, int* Rm);
            break; 
    case 2: executeSDT(int I, int P, int U, int L, int* Rn, int* Rd, int offset);
            break;
    case 3 : executeBranch(int offset);
            break; 
  }
}

int toDecimal(int* binary, int size) {


}



void printRegisters((int*) registers[]) {
  //Prints value of each register to file
  printf("Registers:\n");
  for (int i = 0; i < s; i++) {
    printf("$%d : %d (%p)\n", i, *registers[i], registers[i]);
  }
  printf("PC : %d (%p)\n", *registers[14], registers[14]);
  printf("CPSR : %d (%p)\n", *regiters[15], registers[15]);

}

void printMemory(void) {
  //Prints non-zero memory
  printf("Non-zero memory:\n");

}


int main(int argc, char **argv) {
  //Need to get instruction file from main?
  char instructions[]; //Read in instruction file
  readInstructions(instructions);

  (int*) registers[17]; //17 32-bit registers, store pointers of each register in array
  //Or use C struct?
  char cpsrReg[32];
  registers[15] = &cpsrReg;

  //While still have instructions, pass to fetch function

  printRegisters(registers);
  printMemory();

  return EXIT_SUCCESS;
}
