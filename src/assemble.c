#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define MAX_MAPS 100
#define MAX_INSTR_LEN 511
#define ADDR_INC 0x4
#define MAX_SYMBOL_TABLE_SIZE 50

typedef struct map {
	char *label;
	uint16_t memAddress;
} map_t;

typedef enum {
	ADD, SUB, RSB, AND, EOR, ORR, MOV, TST, TEQ, CMP, MUL, MLA, LDR, STR,
	BEQ, BNE, BGE, BLT, BGT, BLE, B, LSL, ANDEQ,
} instrName_t;


uint32_t parseDataProcessing(map_t *symbolTable, char *remainingString, instrName_t name) {
	return 0;
}

uint32_t parseSDT(map_t *symbolTable, char *remainingString, instrName_t name) {
	return 0;
}

uint32_t parseMultiply(map_t *symbolTable, char *remainingString, instrName_t name) {
	return 0;
}

uint32_t parseBranch(map_t *symbolTable, char *remainingString, instrName_t name) {
	return 0;
}

uint32_t parseSpecial(map_t *symbolTable, char *remainingString, instrName_t name) {
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

map_t newMap(char *label, uint16_t memAddress) {
	char *labelOnHeap = malloc(511);
	strcpy(labelOnHeap, label);
	map_t map;
	map.label = labelOnHeap;
	map.memAddress = memAddress;
	return map;
}

int firstPass(char* fileName, map_t *symbolTable) {

	FILE *fptr = fopen(fileName, "r");
	uint16_t memAddress = 0x0; //check if it is 16 bits
	int tableSize= 0;
	int numOfInstr = 0;

	while(1) {
		char buffer[MAX_INSTR_LEN];
		fgets(buffer, MAX_INSTR_LEN, fptr);
		int strLength = strlen(buffer + 1); //for the \0 char
    if (buffer[strLength - 1] == ':') {
			if (tableSize >= MAX_SYMBOL_TABLE_SIZE) {
				printf("exceeded symbolTableSize");
				return 0;
			}
			map_t map = newMap(buffer, memAddress); //buffer includes the ':'
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

instrName_t toInstrName(char* instrString) {
	return ADD;
}


/* pass 2: decode all instructions using opcode mneomonic into binary and
					 replace label references with corresponding address from symbol
					 table.
*/
void secondPass(char *fileName, map_t *symbolTable, uint32_t *binaryInstructions) {
 //keep track of order of the instructions, pass instruNumber into
 //parse so that it knows which index you write to in the binaryInstructions
 FILE *fptr = fopen(fileName, "r");
 int instrNum = 0;

 while(1) {
	 char buffer[MAX_INSTR_LEN];
	 fgets(buffer, MAX_INSTR_LEN, fptr);
	 int strLength = strlen(buffer);
	 if (buffer[strLength - 1] != ':') {

		 char *instrStringBuffer = strtok(buffer, ' '); //check this, maybe use strtol
     char *instrString = malloc((sizeof(char) * 3) + 1) //i think...
		 instrString = strcpy(instrString, instrStringBuffer);
		 char *passedString = buffer[4]; //bc each instruName is 3 chars + 1 space, not sure about the /0 char
		 char *remainingString = malloc((sizeof(char) * 4) + 1);
		 remainingString = strcpy(remainingString, passedString);
		 //the above code stores the remaining string on the heap so it can be
		 //passed to the parse helper functions

		 //TODO: convert instrString to a type instrName_t for the switch statement
     instrName_t instrName = toInstrName(instrString);

		 switch (instrName) {
	   	 case ADD :
			   binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, ADD);
			   break;
	 		 case SUB :
			   binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, SUB);
			   break;
	 		 case RSB :
			   binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, RSB);
			   break;
	 		 case AND :
			   binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, AND);
			   break;
	 		 case EOR :
			   binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, EOR);
			   break;
	 		 case ORR :
			   binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, ORR);
			   break;
	 		 case MOV :
			   binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, MOV);
			   break;
	 		 case TST :
			   binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, TST);
			   break;
	 		 case TEQ :
			   binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, TEQ);
		     break;
	 		 case CMP :
			 	binaryInstructions[instrNum] = parseDataProcessing(symbolTable, remainingString, CMP);
			  break;
			 case MUL :
			   binaryInstructions[instrNum] = parseMultiply(symbolTable, remainingString, MUL);
			   break;
			 case MLA :
			  binaryInstructions[instrNum] = parseMultiply(symbolTable, remainingString, MLA);
				break;
			 case LDR :
			  binaryInstructions[instrNum] = parseSDT(symbolTable, remainingString, LDR);
			  break;
			 case STR :
			  binaryInstructions[instrNum] = parseSDT(symbolTable, remainingString, STR);
				break;
			 case LSL :
			   binaryInstructions[instrNum] = parseSpecial(symbolTable, remainingString, LSL);
			   break;
			 case ANDEQ :
			   binaryInstructions[instrNum] = parseSpecial(symbolTable, remainingString, ANDEQ);
			   break;
       case BEQ :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BEQ);
			   break;
			 case BNE :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BNE);
			   break;
			 case BGE :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BGE);
			   break;
			 case BLT :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BLT);
			   break;
			 case BGT :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BGT);
			   break;
			 case BLE :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BLE);
			   break;
			 case B :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, B);
				 break;
	 	}
	 }
	 instrNum++;
	 if (feop(fptr)) {
		 break;
	 }
	 free(instrString);
	 //I think we free the remainingString after the parse helper function executions
	 fclose(fptr);
 }
}


void writeBinary(char* filename, uint32_t *binaryInstructions, int numOfInstructions) {
	FILE *fptr = fopen(fileName, "w");
	assert(fptr != NULL);

	for (int i = 0; i < numOfInstructions; i++) {

		uint32_t mask = 1 << 31;
		uint32_t bin = binaryInstructions[i];
		fputs("Address : %08x    Binary instruction : ", (i * 0x4));

		for (int i = 0; i < 32; i++) {
			if ((bin & mask) == 0) {
				fputs("0", fptr);
			} else {
				fputs("1", fptr);
			}
			mask = mask >> 1;
		}
		fputs("\n", fptr);
	}
	fclose(fptr);
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
  writeBinary(argv[2], binaryInstructions, numOfInstructions);

  return EXIT_SUCCESS;
}
