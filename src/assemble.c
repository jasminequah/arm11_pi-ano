#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_MAPS 100
#define MAX_INSTR_LEN 511
#define ADDR_INC 0x4

typedef struct map {
	char label[511];
	uint16_t memAddress;
} map_t;

typedef struct instruction {
 	/*instruction, rd, rn, rs, operand2, rm*/
	instrName_t instrName;
	int Rn;
	int Rd;
	in
 } instr_t;

typdef enum {
	ADD,
	SUB,

} instrName_t


//TODO: figure out a way to pass in the string needed (bc its on the stack in secondPass)
uint32_t parseDataProcessing(map_t *symbolTable, remainingString) {
	return 0;
}

uint32_t parseSDT(map_t *symbolTable, remainingString) {
	return 0;
}

uint32_t parseMultiply(map_t *symbolTable, remainingString) {
	return 0;
}

uint32_t parseBranch(map_t *symbolTable, remainingString) {
	return 0;
}


/* First Pass: Makes symbol table-
        1)Associate each label(string) with a memory address(integer)
	 Second Pass:
	      1) Read OpCode mnemonic
				2) Read operand fields
				3) Replace label with corresponding references
				4) Generate binary encoding of instruction
*/

/*sample readfile statement
returns the num of instructios (including labels) and stores instructions into
symbol table */
int firstPass(char* fileName, map_t *symbolTable, int symbolTableSize) {

	FILE *fptr = fopen(fileName, "r");
	uint16_t memAddress = 0x0; //check if it is 16 bits
	int tableSize= 0;
	int numOfInstr = 0;

	while(1) {
		char buffer[MAX_INSTR_LEN];
		fgets(buffer, MAX_INSTR_LEN, fptr);
		int strLength = strlen(buffer);
    if (buffer[strLength - 1] == ':') {
			if (tableSize >= symbolTableSize) {
				printf("exceeded symbolTableSize");
				return 0;
			}
			map_t map = {buffer, memAddress}; //buffer includes the ':'
			symbolTable[tableSize] = map;
			tableSize++;
		} else {
			numOfInstr++;
		}
		memAddress += ADDR_INC;

		if (feop(fptr)) {
			break;
		}
	}

	fclose(fptr);

	return numOfInstr;
	//returns TOTAL no. of instructions excluding labels
}


/* pass 2: decode all instructions using opcode mneomonic into binary and
					 replace label references with corresponding address from symbol
					 table.
*/
void secondPass(char *fileName, map_t *symbolTable, uint32_t *binaryInstructions) {
 //keep track of order of the instructions, pass instruNumber into
 //parse so that it knows which index you write to in the binaryInstructions
 int instrNum = 0;

 while(1) {
	 char buffer[MAX_INSTR_LEN];
	 fgets(buffer, MAX_INSTR_LEN, fptr);
	 int strLength = strlen(buffer);
	 if (buffer[strLength - 1] != ':') {
		 char *instrName = strtok(buffer, ' ');
		 char *remainingInstr = buffer[4]; //bc each instruName is 3 chars + 1 space

		 switch (instrName) {
	     case ADD :
	 		 case SUB :
	 		 case RSB :
	 		 case AND :
	 		 case EOR :
	 		 case ORR :
	 		 case MOV :
	 		 case TST :
	 		 case TEQ :
	 		 case CMP : binaryInstructions[instrNum] = parseDataProcessing(symbolTable, ...)
			            break;

	 	}

	 }
 }
}


void writeBinary(FILE fptr, uint32_t *binaryInstructions) {

}


int main(int argc, char *argv[]) {

	map_t *symbolTable = malloc(sizeof(map_t) * MAX_MAPS);
	int numOfInstructions = firstPass(argv[1], symbolTable);
	if (numOfInstructions == 0) {
		printf("No instructions read");
		return EXIT_FAILURE;
	}
  uint32_t *binaryInstructions = malloc(sizeof(uint32_t) * numOfInstructions);
	secondPass(argv[1], symbolTable, binaryInstructions);
  writeBinary(argv[2], binaryInstructions);

  return EXIT_SUCCESS;
}
