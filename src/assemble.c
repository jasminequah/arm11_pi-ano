#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define MAX_MAPS              10
#define MAX_INSTR_LEN         511
#define ADDR_INC              0x4
#define MAX_SYMBOL_TABLE_SIZE 50
#define BITS_IN_WORD          32

#define S_BIT             0x00080000
#define I_BIT             0x02000000
#define ALWAYS_COND_CODE  0xe0000000
#define MAX_NO_ROTATE_IMM 0xff
#define MAX_ROTATE        0xf

#define PC_REG            15
#define MAX_CONSTANTS     8

typedef struct map {
  char *label;
  uint16_t memAddress;
} map_t;

typedef enum {
  ADD, SUB, RSB, AND, EOR, ORR, MOV, TST, TEQ, CMP, MUL, MLA, LDR, STR,
  BEQ, BNE, BGE, BLT, BGT, BLE, B, LSL, ANDEQ,
} instrName_t;

typedef enum {
  COMP_RESULT, SINGLE_OP_ASS, SET_CPSR,
} dataProcType_t;

typedef struct state {
	uint32_t *binaryInstructions;
	map_t *symbolTable;
	int numOfInstr;
	uint32_t currAddress;
	int numOfConstants; //num of constants stored at end of file, so we know where to write next constant
} state_t;

uint32_t getOperand(char *expression) {
  char *endPtr;
  if (strncmp("0x", expression, 2) == 0) {
    // Numeric constant in hexadecimal
    return strtol(expression + 2 * (sizeof(char)), &endPtr, 16);
  } else {
    // Numeric constant in decimal
    return strtol(expression, &endPtr, 10);
  }
}

uint32_t rotateLeftByTwo(uint32_t n) {
  return (n << 2) | (n >> ((BITS_IN_WORD)- 2));
}

void getImmediate(uint32_t *instrPtr, uint32_t operand2) {
  uint32_t rotateAmount = 0;

  while (operand2 > MAX_NO_ROTATE_IMM) {
    if (rotateAmount > MAX_ROTATE) {
      printf("Error: Immediate value exceeds maximum representable value");
      break;
    }
    rotateAmount++;
    operand2 = rotateLeftByTwo(operand2);
  }

  *instrPtr = *instrPtr | (rotateAmount << 8) | operand2;
}

void setReg(dataProcType_t instrType, char **tokens, uint32_t *instrPtr) {
  uint32_t rd;
  uint32_t rn;
  uint32_t operand2;

  switch(instrType) {
    case COMP_RESULT:
      rd          = atoi(&tokens[1][1]);
      rn          = atoi(&tokens[2][1]);

      if (tokens[3][0] == '#') {
        *instrPtr = *instrPtr | I_BIT;
      }

      operand2    = getOperand(tokens[3] + sizeof(char));
      getImmediate(instrPtr, operand2);

      *instrPtr   = *instrPtr | (rn << 16) | (rd << 12);
      break;
    case SINGLE_OP_ASS:
      rd          = atoi(&tokens[1][1]);

      if (tokens[2][0] == '#') {
        *instrPtr = *instrPtr | I_BIT;
      }

      operand2    = getOperand(tokens[2] + sizeof(char));
      getImmediate(instrPtr, operand2);

      *instrPtr   = *instrPtr | (rd << 12);
      break;
    case SET_CPSR:
      rn          = atoi(&tokens[1][1]);

      if (tokens[2][0] == '#') {
        *instrPtr = *instrPtr | I_BIT;
      }

      operand2     = getOperand(tokens[2] + sizeof(char));
      getImmediate(instrPtr, operand2);

      *instrPtr    = *instrPtr | (rn << 16);
  }
}

uint32_t getMemAddress(map_t *symbolTable, char *label) {
  map_t *currMap = symbolTable;
  while (1) {
    if (strcmp(currMap->label, label) == 0) {
      break;
    }
    currMap++;
  }
  printf("========= exits getMemAddress while =========\n");
  return currMap->memAddress;
}

/* Does not support case when operand2 is a shift so sets I bit */
uint32_t parseDataProcessing(map_t *symbolTable, char **tokens, instrName_t name) {
  uint32_t opCode;

  // Set condition to always
  uint32_t instruction = ALWAYS_COND_CODE;

  switch(name) {
    case AND:
      opCode = 0;
      setReg(COMP_RESULT, tokens, &instruction);
      break;
    case EOR:
      opCode = 1;
      setReg(COMP_RESULT, tokens, &instruction);
      break;
    case SUB:
      opCode = 2;
      setReg(COMP_RESULT, tokens, &instruction);
      break;
    case RSB:
      opCode = 3;
      setReg(COMP_RESULT, tokens, &instruction);
      break;
    case ADD:
      opCode = 4;
      setReg(COMP_RESULT, tokens, &instruction);
      break;
    case ORR:
      opCode = 12;
      setReg(COMP_RESULT, tokens, &instruction);
      break;
    case MOV:
      opCode = 13;
      setReg(SINGLE_OP_ASS, tokens, &instruction);
      break;
    case TST:
      opCode = 8;
      setReg(SET_CPSR, tokens, &instruction);
      instruction = instruction | S_BIT;
      break;
    case TEQ:
      opCode = 9;
      setReg(SET_CPSR, tokens, &instruction);
      instruction = instruction | S_BIT;
      break;
    case CMP:
      opCode = 10;
      setReg(SET_CPSR, tokens, &instruction);
      instruction = instruction | S_BIT;
      break;
    default:
      // Should return an error
      opCode = -1;
      break;
  }
  instruction = instruction | (opCode << 21);
  return instruction;
}


uint32_t parseSDT(state_t* state, char **tokens, instrName_t name) {

	uint32_t binInstr = 0x04000000; //return value
	uint32_t cond = 0; //not used, spec doesn't say what to do with it :C
	uint32_t rd = atoi(&tokens[1][1]) << 12;
	uint32_t l;
	uint32_t p;
	uint32_t u = 0; //not used,
	uint32_t i = 0; //not used
	uint32_t rn;
	uint32_t offset;
  uint32_t *binaryInstructions = state->binaryInstructions; //where we store all our binary sollutions


	if (tokens[2][0] == '=') {
		if (strlen(tokens[2]) <= 6) { //if less than 0xFF, treat as mov
			tokens[2][0] = '#'; //changing to mov format so func call will work
			return parseDataProcessing(state->symbolTable, tokens, MOV);
		} else {
			//treat address as numerican constant and return ldr rn, [PC, offset]
			uint32_t constant = atoi(tokens[2]);
      uint32_t newLocation = state->numOfInstr + state->numOfConstants;
			binaryInstructions[newLocation] = constant;
      state->numOfConstants += 1;

			offset = newLocation - state->currAddress;
			rn = PC_REG << 16; //value of PC reg
			p  = 0x01000000;
			l  = 0x00100000;
			return binInstr | l | p | rn | rd | offset;
		}
	}

  //if code drops down here we have the last to bullet point cases to deal with
	if (tokens[2][3] == ']') {
		tokens[2][3] = '\0';
	}
	rn = atoi(&tokens[2][2]) << 16;

	int isPreindexed = tokens[3][strlen(tokens[3]) - 1] == ']' || tokens[2][3] == ']';
  if (isPreindexed) {
		p = 0x01000000;
    if (tokens[3] != NULL) {
			tokens[3][strlen(tokens[3]) - 1] = '\0';
			offset = atoi(&tokens[3][3]);
		} else {
			offset = 0;
		}
	} else {
		p = 0;
		offset = atoi(&tokens[3][3]); //expression in post-index without the '0x' of hex
	}

	if (name == LDR) {
		l = 0x00100000; //check
	} else {
		l = 0;
	}

	binInstr = binInstr | cond | rd | l | p | u | i | rn | offset;
	return binInstr;
}


uint32_t parseMultiply(map_t *symbolTable, char **tokens, instrName_t name) {
	// mul r2, r1, r0 = 0x910002e0
	uint32_t code = 0xe0 << 24;

	char* registers;
	int num;
	if (name == MUL) {
		code += (0x0 << 20);
	}
	else {
		code += (0x2 << 20);
	}

	// code = 0x20e0 or 0x00e0

	//Rd
	registers = tokens[1];
	num = registers[1] - '0';
	if (registers[2] != '\0') {
		num = (num * 10) + (registers[2] - '0');
	}
	code += (num << 16);
	//code = 0x2De0 or 0x0De0

	//Rm
	registers = tokens[2];
	num = registers[1] - '0';
	if (registers[2] != '\0') {
		num = (num * 10) + (registers[2] - '0');
	}
	code += ((0x9 << 4) + num);
	//code = 0x9M002De0 or 0x9M000De0

	//Rs
	registers = tokens[3];
	num = registers[1] - '0';
	if (registers[2] != '\0') {
		num = (num * 10) + (registers[2] - '0');
	}
	code += (num << 8);
	//code = 0x9M0S2De0 or 0x9M0S0De0

	if (name == MLA) {
		//Rn
		registers = tokens[4];
		num = registers[1] - '0';
		if (registers[2] != '\0') {
			num = (num * 10) + (registers[2] - '0');
		}
		code += (num << 12);
		//code = 0x9MNS2De0 or 0x9MNS0De0
        }

	return code;
}

uint32_t parseBranch(map_t *symbolTable, char **tokens, instrName_t name, int currAddress) {
  uint32_t cond;
    switch(name) {
      case BEQ:
        cond = 0;
        break;
      case BNE:
        cond = 1;
        break;
      case BGE:
        cond = 10;
        break;
      case BLT:
        cond = 11;
        break;
      case BGT:
        cond = 12;
        break;
      case BLE:
        cond = 13;
        break;
      case B:
        cond = 14;
        break;
      default:
        // Should return an error
        cond = -1;
        break;
    }
  uint32_t destAddress = getMemAddress(symbolTable, tokens[0]);
  uint32_t offset      = destAddress - ((uint32_t) (currAddress) * 4);
  return (cond << 28) | (10 << 24) | offset;
}

uint32_t parseSpecial(map_t *symbolTable, char **tokens, instrName_t name) {
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
    fscanf(fptr, " %[^\n]", buffer);
    if (feof(fptr)) {
      break;
    }
    int strLength = strlen(buffer) + 1; //for the \0 char
    if (buffer[strLength - 1] == ':') {
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
    }
    memAddress += ADDR_INC;

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
 //keep track of order of the instructions, pass instruNumber into
 //parse so that it knows which index you write to in the binaryInstructions

  FILE *fptr = fopen(fileName, "r");
  uint32_t *binaryInstructions = state->binaryInstructions;
  map_t *symbolTable = state->symbolTable;
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
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, ADD);
      	  break;
      	case SUB :
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, SUB);
      	  break;
      	case RSB :
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, RSB);
      	  break;
      	case AND :
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, AND);
      	  break;
      	case EOR :
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, EOR);
      	  break;
      	case ORR :
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, ORR);
      	  break;
      	case MOV :
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, MOV);
      	  break;
      	case TST :
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, TST);
      	  break;
      	case TEQ :
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, TEQ);
      	  break;
      	case CMP :
      	  binaryInstructions[currAddress] = parseDataProcessing(symbolTable, tokens, CMP);
      	  break;
      	case MUL :
      	  binaryInstructions[currAddress] = parseMultiply(symbolTable, tokens, MUL);
      	  break;
      	case MLA :
      	  binaryInstructions[currAddress] = parseMultiply(symbolTable, tokens, MLA);
      	  break;
      	case LDR :
      	  binaryInstructions[currAddress] = parseSDT(state, tokens, LDR);
      	  break;
      	case STR :
      	  binaryInstructions[currAddress] = parseSDT(state, tokens, STR);
      	  break;
      	case LSL :
      	  binaryInstructions[currAddress] = parseSpecial(symbolTable, tokens, LSL);
      	  break;
      	case ANDEQ :
      	  binaryInstructions[currAddress] = parseSpecial(symbolTable, tokens, ANDEQ);
      	  break;
      	case BEQ :
      	  binaryInstructions[currAddress] = parseBranch(symbolTable, tokens, BEQ, currAddress * 4);
      	  break;
      	case BNE :
      	  binaryInstructions[currAddress] = parseBranch(symbolTable, tokens, BNE, currAddress * 4);
      	  break;
      	case BGE :
      	  binaryInstructions[currAddress] = parseBranch(symbolTable, tokens, BGE, currAddress * 4);
      	  break;
      	case BLT :
      	  binaryInstructions[currAddress] = parseBranch(symbolTable, tokens, BLT, currAddress * 4);
      	  break;
      	case BGT :
      	  binaryInstructions[currAddress] = parseBranch(symbolTable, tokens, BGT, currAddress * 4);
      	  break;
      	case BLE :
      	  binaryInstructions[currAddress] = parseBranch(symbolTable, tokens, BLE, currAddress * 4);
      	  break;
      	case B :
      	  binaryInstructions[currAddress] = parseBranch(symbolTable, tokens, B, currAddress * 4);
      	  break;
      }
      // instrNum++; remaned instrNum to currAddress coz it makes more sense
      currAddress += 1;
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

state_t* newState(int numOfInstructions, map_t *symbolTable) {
  state_t *newState = malloc(sizeof(state_t));
  newState->numOfInstr = numOfInstructions;
  newState->currAddress = 0;
  newState->numOfConstants = 0;
  newState->symbolTable = symbolTable;

  uint32_t *binaryInstructions = malloc(sizeof(uint32_t) * (numOfInstructions + MAX_CONSTANTS));
  newState->binaryInstructions = binaryInstructions;

  return newState;
}


int main(int argc, char *argv[]) {
  map_t *symbolTable = malloc(sizeof(map_t) * MAX_MAPS);
  int numOfInstructions = firstPass(argv[1], symbolTable);
  if (numOfInstructions == 0) {
    printf("No instructions read");
    return EXIT_FAILURE;
  }

  state_t *state = newState(numOfInstructions, symbolTable);
  secondPass(argv[1], state);
  writeBinary(argv[2], state);
  free(symbolTable);
  free(state);
  return EXIT_SUCCESS;
}
