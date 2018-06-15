#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "definitions.h"
#include "utilities.h"
#include "parse.h"

/* First Pass: Makes symbol table-
        1)Associate each label(string) with a memory address(integer)
   Second Pass:
        1) Read OpCode mnemonic
	2) Read operand fields
	3) Replace label with corresponding references
	4) Generate binary encoding of instruction
*/

int firstPass(char* fileName, map_t *symbolTable) {

  FILE *fptr = fopen(fileName, "r");
  uint16_t memAddress = 0x0;
  int tableSize= 0;
  int numOfInstr = 0;

  while(1) {
    char buffer[MAX_INSTR_LEN];
    fscanf(fptr, " %[^\n]", buffer);
    if (feof(fptr)) {
      break;
    }
    int strLength = strlen(buffer) + 1;
    if (buffer[strLength - 2] == ':') {
      if (tableSize >= MAX_SYMBOL_TABLE_SIZE) {
        printf("exceeded symbolTableSize");
        return 0;
      }
      char *label = strtok(buffer, ": ");
      map_t map = newMap(label, memAddress);
      symbolTable[tableSize] = map;
      tableSize++;
    } else {
      numOfInstr++;
      memAddress += ADDR_INC;
    }
  }

  fclose(fptr);

  return numOfInstr;
}

/* 
   pass 2: decode all instructions using opcode mneomonic into binary and 
           replace label references with corresponding address from symbol
           table.
*/

void secondPass(char *fileName, state_t* state) {

  FILE *fptr = fopen(fileName, "r");

  while(1) {
    char buffer[MAX_INSTR_LEN];
    fscanf(fptr, " %[^\n]s", buffer);

    if (feof(fptr)) {
      fclose(fptr);
      break;
    }

    int strLength = strlen(buffer);
    if (buffer[strLength - 1] != ':') {
      char *tokens[6];
      const char delimiter[2] = ", ";

      int num_tokens = 0;
      tokens[0] = strtok(buffer, delimiter);
      while(tokens[num_tokens] != NULL) {
	       num_tokens++;
	       tokens[num_tokens] = strtok(NULL, delimiter);
      }
      instrName_t instrName = toInstrName(tokens[0]);
      parse(state, tokens, instrName, num_tokens);
      state->currAddress = (state->currAddress) + 1;
    }
  }
}

void writeBinary(char* fileName, state_t *state) {
  FILE *fptr = fopen(fileName, "w");
  assert(fptr != NULL);
  uint32_t *binaryInstructions = state->binaryInstructions;

  for (int i = 0; i < (state->numOfInstr + state->numOfConstants); i++) {
    fwrite(&binaryInstructions[i], 4, 1, fptr);
  }
  fclose(fptr);
}
