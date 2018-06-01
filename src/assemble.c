#include <stdlib.h>
#include <stdint.h>

typedef struct Map {
	char label[511];
	uint16_t memAddress;
}map;

typedef struct instruction {
	/*instruction, rd, rn, rs, operand2, rm*/
}instr;

typedef map* symbolTable[];

/*sample readfile statement*/

char** readfile(FILE *fp) {
	/*gets file size*/
	int i = 0;
	int size = 0;

	do {
		char c[511];
		fgets(c, 511, fp);
		//fseek(fp, ftell(fp)+1, SEEK_SET);
		size++;
		i++;
	} while (feof(fp) == 0);

	//printf("%d \n", size);

	fseek(fp, 0, SEEK_SET);

	/* 
	Stores instructions in heap
	Assumes instruction size is less tha 511
	*/

	/*Allocates memory*/
	char **memInstr = (char**)malloc(size);
	for (int i = 0; i < size; i++) {
		memInstr[i] = (char*)malloc(511);
	}

	for (int i = 0; i < size; i++) {
		fgets(memInstr[i], 511, fp);
	}

	return memInstr;
}

/* First Pass: Makes symbol table- 
        1)Associate each label(string) with a memory address(integer)
	 Second Pass: 
	      1) Read OpCode mnemonic
				2) Read operand fields
				3) Replace label with corresponding references
				4) Generate binary encoding of instruction
*/
int main(int argc, char *argvsrc[], char *argvdest[]) {
  return EXIT_SUCCESS;
}
