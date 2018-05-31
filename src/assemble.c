#include <stdlib.h>
#include <stdint.h>

typedef struct Map {
	char label[];
	uint16_t memAddress;
}map;

typedef map* symbolTable[];

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
