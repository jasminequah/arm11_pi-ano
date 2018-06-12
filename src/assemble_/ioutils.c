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
    fscanf(fptr, " %[^\n]", buffer);
    if (feof(fptr)) {
      break;
    }
    int strLength = strlen(buffer) + 1; //for the \0 char
    if (buffer[strLength - 2] == ':') {
      if (tableSize >= MAX_SYMBOL_TABLE_SIZE) {
        printf("exceeded symbolTableSize");
        return 0;
      }
      char *label = strtok(buffer, ": ");
      map_t map = newMap(label, memAddress); //buffer includes the ':'
      symbolTable[tableSize] = map;
      tableSize++;
    } else {
      numOfInstr++;
      memAddress += ADDR_INC;
    }
  }

  fclose(fptr);

  return numOfInstr;
  //returns TOTAL no. of instructions excluding labels
}

instrName_t toInstrName(char* instrString) {
  if (strcmp(instrString, "add") == 0) {
    return ADD;
  }
  else if (strcmp(instrString, "sub") == 0) {
    return SUB;
  }
  else if (strcmp(instrString, "rsb") == 0) {
    return RSB;
  }
  else if (strcmp(instrString, "and") == 0) {
    return AND;
  }
  else if (strcmp(instrString, "eor") == 0) {
    return EOR;
  }
  else if (strcmp(instrString, "orr") == 0) {
    return ORR;
  }
  else if (strcmp(instrString, "mov") == 0) {
    return MOV;
  }
  else if (strcmp(instrString, "tst") == 0) {
    return TST;
  }
  else if (strcmp(instrString, "teq") == 0) {
    return TEQ;
  }
  else if (strcmp(instrString, "cmp") == 0) {
    return CMP;
  }
  else if (strcmp(instrString, "mul") == 0) {
    return MUL;
  }
  else if (strcmp(instrString, "mla") == 0) {
    return MLA;
  }
  else if (strcmp(instrString, "ldr") == 0) {
    return LDR;
  }
  else if (strcmp(instrString, "str") == 0) {
    return STR;
  }
  else if (strcmp(instrString, "beq") == 0) {
    return BEQ;
  }
  else if (strcmp(instrString, "bne") == 0) {
    return BNE;
  }
  else if (strcmp(instrString, "bge") == 0) {
    return BGE;
  }
  else if (strcmp(instrString, "blt") == 0) {
    return BLT;
  }
  else if (strcmp(instrString, "bgt") == 0) {
    return BGT;
  }
  else if (strcmp(instrString, "ble") == 0) {
    return BLE;
  }
  else if (strcmp(instrString, "b") == 0) {
    return B;
  }
  else if (strcmp(instrString, "lsl") == 0) {
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
void secondPass(char *fileName, state_t* state) {

  FILE *fptr = fopen(fileName, "r");
  uint32_t currAddress = state->currAddress;
  // int numOfInstr = 0; this is replaced with the states current address which we increment
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

      /* TOKENIZE */
      const char delimiter[2] = ", ";
      tokens[0] = strtok(buffer, delimiter);

      int i = 0;
      while(tokens[i] != NULL) {
	       i++;
	       tokens[i] = strtok(NULL, delimiter);
      }

      instrName_t instrName = toInstrName(tokens[0]);
      switch (instrName) {
        case ADD :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, ADD, i);
      	  break;
      	case SUB :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, SUB, i);
      	  break;
      	case RSB :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, RSB, i);
      	  break;
      	case AND :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, AND, i);
      	  break;
      	case EOR :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, EOR, i);
      	  break;
      	case ORR :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, ORR, i);
      	  break;
      	case MOV :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, MOV, i);
      	  break;
      	case TST :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, TST, i);
      	  break;
      	case TEQ :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, TEQ, i);
      	  break;
      	case CMP :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, CMP, i);
      	  break;
      	case MUL :
      	  state->binaryInstructions[currAddress] = parseMultiply(state->symbolTable, tokens, MUL);
      	  break;
      	case MLA :
      	  state->binaryInstructions[currAddress] = parseMultiply(state->symbolTable, tokens, MLA);
      	  break;
      	case LDR :
      	  state->binaryInstructions[currAddress] = parseSDT(state, tokens, LDR, i);
      	  break;
      	case STR :
      	  state->binaryInstructions[currAddress] = parseSDT(state, tokens, STR, i);
      	  break;
      	case LSL :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, LSL, i);
      	  break;
      	case ANDEQ :
      	  state->binaryInstructions[currAddress] = parseDataProcessing(state->symbolTable, tokens, ANDEQ, i);
      	  break;
      	case BEQ :
      	  state->binaryInstructions[currAddress] = parseBranch(state->symbolTable, tokens, BEQ, currAddress * 4);
      	  break;
      	case BNE :
      	  state->binaryInstructions[currAddress] = parseBranch(state->symbolTable, tokens, BNE, currAddress * 4);
      	  break;
      	case BGE :
      	  state->binaryInstructions[currAddress] = parseBranch(state->symbolTable, tokens, BGE, currAddress * 4);
      	  break;
      	case BLT :
      	  state->binaryInstructions[currAddress] = parseBranch(state->symbolTable, tokens, BLT, currAddress * 4);
      	  break;
      	case BGT :
      	  state->binaryInstructions[currAddress] = parseBranch(state->symbolTable, tokens, BGT, currAddress * 4);
      	  break;
      	case BLE :
      	  state->binaryInstructions[currAddress] = parseBranch(state->symbolTable, tokens, BLE, currAddress * 4);
      	  break;
      	case B :
      	  state->binaryInstructions[currAddress] = parseBranch(state->symbolTable, tokens, B, currAddress * 4);
      	  break;
      }
      state->currAddress = (state->currAddress) + 1;
      currAddress = state->currAddress;
    }
  }
}


void writeBinary(char* fileName, state_t *state) {
  FILE *fptr = fopen(fileName, "w");
  assert(fptr != NULL);
  uint32_t *binaryInstructions = state->binaryInstructions;

  for (int i = 0; i < (state->numOfInstr + state->numOfConstants); i++) {
    fwrite(&binaryInstructions[i], 4, 1, fptr); //check this
  }
  fclose(fptr);
}
