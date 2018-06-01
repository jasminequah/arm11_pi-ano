#include <stdlib.h>
#include <stdint.h>

#define MAX_MAPS 100

typedef struct map {
	char label[511];
	uint16_t memAddress;
} map_t;

// typedef struct instruction {
// 	/*instruction, rd, rn, rs, operand2, rm*/
// } instr_t;

typdef enum {
	ADD,
	SUB,

} instr_t



/*sample readfile statement
returns the num of instructios (including labels) and stores instructions into
symbol table */
int readFile(char* fileName, map_t *symbolTable) {
	/*gets file size*/

	FILE *fptr = fopen(fileName, "r");
	// char **memInstr = (char**) malloc(size * sizeof(char));
	// for (int i = 0; i < size; i++) {
	// 	memInstr[i] = (char*) malloc(511);
	// }

	while(1) {
		fgets();
	}

	int size = 0;

	do {
		char c[511];
		fgets(c, 511, fp);
		size++;
	} while (feof(fp) == 0);

	//printf("%d \n", size);

	fseek(fp, 0, SEEK_SET);

	/*
	Stores instructions in heap
	Assumes instruction size is less than 511
	*/

	/*Allocates memory*/
	char **memInstr = (char**) malloc(size * sizeof(char));
	for (int i = 0; i < size; i++) {
		memInstr[i] = (char*) malloc(511);
	}

	for (int i = 0; i < size; i++) {
		fgets(memInstr[i], 511, fp);
	}

	return memInstr;
}

uint32_t parser(instr_t instr) {

	switch (instr) {

	}

}

/* First Pass: Makes symbol table-
        1)Associate each label(string) with a memory address(integer)
	 Second Pass:
	      1) Read OpCode mnemonic
				2) Read operand fields
				3) Replace label with corresponding references
				4) Generate binary encoding of instruction
*/

int main(int argc, char *argv[]) {

	map_t *symbolTable = malloc(sizeof(map_t) * MAX_MAPS);
	int numOfInstructions = readFile(argv[1], symbolTable);
  uint32_t* binaryInstructions = malloc(sizeof(uint32_t) * numOfInstructions);

 //write binaryInstructions into FILE argv[2] (dest)

  return EXIT_SUCCESS;
}
