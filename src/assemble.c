#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

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

uint32_t getMemAddress(map_t *symbolTable, char *remainingString) {
	int currMap = 0;
	while (true) {
		map = *(symbolTable + currMap);
    if (map.label == remainingString) {
			return map.memAddress;
		}
		currMap += sizeof(map_t);
	}
}

uint32_t parseDataProcessing(map_t *symbolTable, char *remainingString, instrName_t name) {
	return 0;
}

uint32_t parseSDT(map_t *symbolTable, char *remainingString, instrName_t name) {
	return 0;
}

uint32_t parseMultiply(map_t *symbolTable, char *remainingString, instrName_t name) {
	// mul r2, r1, r0 = 0x910002e0
	uint32_t code = 0xe0;
	char* registers;
	int num;
	if (name == MUL) {
		code += (0x0 << 12);
	}
	else {
		code += (0x2 << 12);
	}

	// code = 0x20e0 or 0x00e0

	//Rd
	registers = strtok(remainingString, " ");
	num = registers[1] - '0';
	code += (num << 8);
	// code = 0x2De0 or 0x0De0

	//Rm
	registers = strtok(NULL, " ");
	num = registers[1] - '0';
	code += (((0x9 << 4) + num) << 24);
	//code = 0x9M002De0 or 0x9M000De0

	//Rs
	registers = strtok(NULL, " ");
	num = registers[1] - '0';
	code += (num << 16);
	//code = 0x9M0S2De0 or 0x9M0S0De0

	if (name == MLA) {
		//Rn
		registers = strtok(NULL, " ");
		num = registers[1] - '0';
		code += (num << 20);
		//code = 0x9MNS2De0 or 0x9MNS0De0
	}

	return code;
}

uint32_t parseBranch(map_t *symbolTable, char *remainingString, instrName_t name, int currAddress) {
  int cond;
	switch(name) {
		case(BEQ) :
		  cond = 0;
		case(BNE) :
		  cond = 1;
		case(BGE) :
		  cond = 10;
		case(BLT) :
		  cond = 11;
		case(BGT) :
		  cond = 12;
		case(BLE) :
		  cond = 13;
	  case(B) :
		  cond = 14;
	}
  uint32_t destAddress = getMemAddress(symbolTable, remainingString);
	uint32_t offset      = destAddress - ((uint32_t) (currAddress) * 4);
	return (cond << 28) | (10 << 24) | offset;
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

		if (feof(fptr)) {
			break;
		}
	}

	fclose(fptr);

	return numOfInstr;
	//returns TOTAL no. of instructions excluding labels
}

instrName_t toInstrName(char* instrString) {
	if (strcmp(instrString, "add ") == 0) {
		return ADD;
	}
	else if (strcmp(instrString, "sub ") == 0) {
		return SUB;
	}
	else if (strcmp(instrString, "rsb ") == 0) {
		return RSB;
	}
	else if (strcmp(instrString, "and ") == 0) {
		return AND;
	}
	else if (strcmp(instrString, "eor ") == 0) {
		return EOR;
	}
	else if (strcmp(instrString, "orr ") == 0) {
		return ORR;
	}
	else if (strcmp(instrString, "mov ") == 0) {
		return MOV;
	}
	else if (strcmp(instrString, "tst ") == 0) {
		return TST;
	}
	else if (strcmp(instrString, "teq ") == 0) {
		return TEQ;
	}
	else if (strcmp(instrString, "cmp ") == 0) {
		return CMP;
	}
	else if (strcmp(instrString, "mul ") == 0) {
		return MUL;
	}
	else if (strcmp(instrString, "mla ") == 0) {
		return MLA;
	}
	else if (strcmp(instrString, "ldr ") == 0) {
		return LDR;
	}
	else if (strcmp(instrString, "str ") == 0) {
		return STR;
	}
	else if (strcmp(instrString, "beq ") == 0) {
		return BEQ;
	}
	else if (strcmp(instrString, "bne ") == 0) {
		return BNE;
	}
	else if (strcmp(instrString, "bge ") == 0) {
		return BGE;
	}
	else if (strcmp(instrString, "blt ") == 0) {
		return BLT;
	}
	else if (strcmp(instrString, "bgt ") == 0) {
		return BGT;
	}
	else if (strcmp(instrString, "ble ") == 0) {
		return BLE;
	}
	else if (strcmp(instrString, "b ") == 0) {
		return B;
	}
	else if (strcmp(instrString, "lsl ") == 0) {
		return LSL;
	}
	else {
		return ANDEQ;
	}

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
		 strcpy(instrString, instrStringBuffer);

		 char *passedString = buffer[4]; //bc each instruName is 3 chars + 1 space, not sure about the /0 char
		 char *remainingString = malloc((sizeof(char) * 4) + 1);
		 strcpy(remainingString, passedString);
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
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BEQ, instrNum * 4);
			   break;
			 case BNE :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BNE, instrNum * 4);
			   break;
			 case BGE :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BGE, instrNum * 4);
			   break;
			 case BLT :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BLT, instrNum * 4);
			   break;
			 case BGT :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BGT, instrNum * 4);
			   break;
			 case BLE :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, BLE, instrNum * 4);
			   break;
			 case B :
			   binaryInstructions[instrNum] = parseBranch(symbolTable, remainingString, B, instrNum * 4);
				 break;
	 	}
	 }
	 instrNum++;
	 if (feof(fptr)) {
		 break;
	 }
	 //free(instrString);
	 //I think we free the remainingString after the parse helper function executions
	 fclose(fptr);
 }
}


void writeBinary(char* fileName, uint32_t *binaryInstructions, int numOfInstructions) {
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
