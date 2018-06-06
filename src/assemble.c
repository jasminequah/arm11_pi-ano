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
#define HEX_BASE              16
#define DECIMAL_BASE          10

#define S_BIT             0x00080000
#define I_BIT             0x02000000
#define ALWAYS_COND_CODE  0xe0000000
#define EQ_COND_CODE      0x00000000
#define MAX_NO_ROTATE_IMM 0xff
#define MAX_ROTATE        0xf

#define OPCODE_SHIFT         21
#define ROTATE_SHIFT         8
#define SHIFT_BY_REG_BIT     0x00000010
#define RN_SHIFT             16
#define RD_SHIFT             12
#define RS_SHIFT             8
#define SHIFT_CONSTANT_SHIFT 7
#define SHIFT_TYPE_SHIFT     5

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

uint32_t getOperand(char *expression) {
  char *endPtr;
  if (strncmp("0x", expression, 2) == 0) {
    // Numeric constant in hexadecimal
    return strtol(expression + 2 * (sizeof(char)), &endPtr, HEX_BASE);
  } else {
    // Numeric constant in decimal
    return strtol(expression, &endPtr, DECIMAL_BASE);
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
  
  *instrPtr = *instrPtr | (rotateAmount << ROTATE_SHIFT) | operand2;
}

void getExprBits(uint32_t *instrPtr, char *expression) {
  if (expression[0] == '#') {
    *instrPtr = *instrPtr | I_BIT;
  }
  uint32_t operand2 = getOperand(expression + sizeof(char));
  getImmediate(instrPtr, operand2);

}

void getShiftBits(uint32_t *instrPtr, char **remainingTokens) {
  uint32_t rm = atoi(&remainingTokens[0][1]);
  uint32_t shiftType;
  
  if (strcmp(remainingTokens[1], "lsl") == 0) {
    shiftType = 0;
  } else if (strcmp(remainingTokens[1], "lsr") == 0) {
    shiftType = 1;
  } else if (strcmp(remainingTokens[1], "asr") == 0) {
    shiftType = 2;
  } else if (strcmp(remainingTokens[1], "ror") == 0) {
    shiftType = 3;
  } else {
    printf("Error: unrecognised shift type\n");
  }

  if (remainingTokens[2][0] == '#') {
    uint32_t shiftAmount = getOperand(remainingTokens[2] + sizeof(char));
    *instrPtr = *instrPtr | (shiftAmount << SHIFT_CONSTANT_SHIFT);
  } else {
    uint32_t rs = atoi(&remainingTokens[2][1]);
    *instrPtr = *instrPtr | (rs << RS_SHIFT) | SHIFT_BY_REG_BIT;
  }

  *instrPtr = *instrPtr | (shiftType << SHIFT_TYPE_SHIFT) | rm;

}

void setReg(dataProcType_t instrType, char **tokens, uint32_t *instrPtr, int numTokens) {
  uint32_t rd;
  uint32_t rn;
  switch(instrType) {
    case COMP_RESULT:
      rd = atoi(&tokens[1][1]);
      rn = atoi(&tokens[2][1]);
      
      if (numTokens == 4) {
        // Expression
        getExprBits(instrPtr, tokens[3]);
      } else {
        // Shifted register 
        getShiftBits(instrPtr, &tokens[3]);
      }
      
      *instrPtr = *instrPtr | (rn << RN_SHIFT) | (rd << RD_SHIFT);
      break;

    case SINGLE_OP_ASS:
      rd = atoi(&tokens[1][1]);
      if (numTokens == 3) {
        getExprBits(instrPtr, tokens[2]);
      } else {
        getShiftBits(instrPtr, &tokens[2]);
      }

      *instrPtr = *instrPtr | (rd << RD_SHIFT);
      break;

    case SET_CPSR:
      rn = atoi(&tokens[1][1]);
      if (numTokens == 3) {
        getExprBits(instrPtr, tokens[2]);
      } else {
        getShiftBits(instrPtr, &tokens[2]);
      }

      *instrPtr = *instrPtr | (rn << RN_SHIFT);
  }
}

uint32_t parseDataProcessing(map_t *symbolTable, char **tokens, instrName_t name, int numTokens) {
  uint32_t opCode;

  // Set condition to always
  uint32_t instruction = ALWAYS_COND_CODE;

  switch(name) {
    case ANDEQ:
      instruction = EQ_COND_CODE;
    case AND:
      opCode = 0;
      setReg(COMP_RESULT, tokens, &instruction, numTokens);
      break;
    case EOR:
      opCode = 1;
      setReg(COMP_RESULT, tokens, &instruction, numTokens);
      break;
    case SUB:
      opCode = 2;
      setReg(COMP_RESULT, tokens, &instruction, numTokens);
      break;
    case RSB:
      opCode = 3;
      setReg(COMP_RESULT, tokens, &instruction, numTokens);
      break;
    case ADD:
      opCode = 4;
      setReg(COMP_RESULT, tokens, &instruction, numTokens);
      break;
    case ORR:
      opCode = 12;
      setReg(COMP_RESULT, tokens, &instruction, numTokens);
      break;
    case MOV:
      opCode = 13;
      setReg(SINGLE_OP_ASS, tokens, &instruction, numTokens);
      break;
    case TST:
      opCode = 8;
      setReg(SET_CPSR, tokens, &instruction, numTokens);
      instruction = instruction | S_BIT;
      break;
    case TEQ:
      opCode = 9;
      setReg(SET_CPSR, tokens, &instruction, numTokens);
      instruction = instruction | S_BIT;
      break;
    case CMP:
      opCode = 10;
      setReg(SET_CPSR, tokens, &instruction, numTokens);
      instruction = instruction | S_BIT;
      break;
    case LSL: ;
      char *newTokens[6] = {"mov", tokens[1], tokens[1], "lsl", tokens[2]};
      return parseDataProcessing(symbolTable, newTokens, MOV, numTokens + 2);
    default:
      printf("Error: invalid instruction");
      opCode = -1;
      break;
  }
  instruction = instruction | (opCode << OPCODE_SHIFT);
  return instruction;
}



uint32_t parseSDT(map_t *symbolTable, char **tokens, instrName_t name) {
  //
	// uint32_t binInstr = 0x04000000;
	// uint32_t cond;
	// char* rd = tokens[1];
	// uint32_t rdNum = atoi(&rd[1]) << 12;
	// uint32_t l;
	// uint32_t p;
	// uint32_t u;
	// uint32_t i;
	// uint32_t rn;
	// uint32_t offset;
  //
  // if (tokens[3][strlen(tokens[3]) - 1] == ']' || tokens[2][3] == ']') {
	// 	//pre-indexing - p is set
	// 	p = 0x01000000;
	// 	if (tokens[2][3] == ']') {
	// 		tokens[2][3] = '\0';
	// 	}
	// 	rn = atoi(&tokens[2][2]) << 16;
  //
  //   if (tokens[3][strlen(tokens[3]) - 1] == ']') {
	// 		tokens[3][strlen(tokens[3]) - 1] = '\0';
	// 		offset = atoi(&tokens[3][2]);
	// 	} else {
	// 		offset = 0;
	// 	}
  //
	// } else {
	// 	p = 0x00000000;
	// }
  //
	// switch (name) {
	// 	case STR :
	// 		l = 0x00000000;
	// 	case LDR :
	// 	  l = 0x00100000; // check
	// 		if (tokens[2][0] == '=') {
	// 			if (strlen(tokens[2]) <= 6) { //if less than 0xFF, treat as move
	// 				return parseDataProcessing(symbolTable, tokens, MOV);
	// 				i = 0x00000000;
	// 			} else {
	// 				i = 0x02000000;
	// 			}
	// 			//treat address as numerican constant
  //
	// 		}
  //
	// }
  //
	// binInstr = binInstr | cond | rdNum | l | p | u | i | rn | offset;
	// return binInstr;
  return 0;
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

uint32_t getMemAddress(map_t *symbolTable, char *label) {
  map_t *currMap = symbolTable;
  while (1) {
    printf("label in map: %s, label: %s\n", currMap->label, label);
    if (strcmp(currMap->label, label) == 0) {
      break;
    }
    currMap++;
  }
  printf("========= exits getMemAddress while =========\n");
  return currMap->memAddress;
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
        printf("Error: invalid condition\n");
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
void secondPass(char *fileName, map_t *symbolTable, uint32_t *binaryInstructions) {
 //keep track of order of the instructions, pass instruNumber into
 //parse so that it knows which index you write to in the binaryInstructions

  FILE *fptr = fopen(fileName, "r");
  int instrNum = 0;
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
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, ADD, i);
      	  break;
      	case SUB :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, SUB, i);
      	  break;
      	case RSB :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, RSB, i);
      	  break;
      	case AND :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, AND, i);
      	  break;
      	case EOR :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, EOR, i);
      	  break;
      	case ORR :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, ORR, i);
      	  break;
      	case MOV :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, MOV, i);
      	  break;
      	case TST :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, TST, i);
      	  break;
      	case TEQ :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, TEQ, i);
      	  break;
      	case CMP :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, CMP, i);
      	  break;
      	case MUL :
      	  binaryInstructions[instrNum] = parseMultiply(symbolTable, tokens, MUL);
      	  break;
      	case MLA :
      	  binaryInstructions[instrNum] = parseMultiply(symbolTable, tokens, MLA);
      	  break;
      	case LDR :
      	  binaryInstructions[instrNum] = parseSDT(symbolTable, tokens, LDR);
      	  break;
      	case STR :
      	  binaryInstructions[instrNum] = parseSDT(symbolTable, tokens, STR);
      	  break;
      	case LSL :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, LSL, i);
      	  break;
      	case ANDEQ :
      	  binaryInstructions[instrNum] = parseDataProcessing(symbolTable, tokens, ANDEQ, i);
      	  break;
      	case BEQ :
      	  binaryInstructions[instrNum] = parseBranch(symbolTable, tokens, BEQ, instrNum * 4);
      	  break;
      	case BNE :
      	  binaryInstructions[instrNum] = parseBranch(symbolTable, tokens, BNE, instrNum * 4);
      	  break;
      	case BGE :
      	  binaryInstructions[instrNum] = parseBranch(symbolTable, tokens, BGE, instrNum * 4);
      	  break;
      	case BLT :
      	  binaryInstructions[instrNum] = parseBranch(symbolTable, tokens, BLT, instrNum * 4);
      	  break;
      	case BGT :
      	  binaryInstructions[instrNum] = parseBranch(symbolTable, tokens, BGT, instrNum * 4);
      	  break;
      	case BLE :
      	  binaryInstructions[instrNum] = parseBranch(symbolTable, tokens, BLE, instrNum * 4);
      	  break;
      	case B :
      	  binaryInstructions[instrNum] = parseBranch(symbolTable, tokens, B, instrNum * 4);
      	  break;
      }
      instrNum++;
    }
  }
}


void writeBinary(char* fileName, uint32_t *binaryInstructions, int numOfInstructions) {
  FILE *fptr = fopen(fileName, "w");
  assert(fptr != NULL);

  for (int i = 0; i < numOfInstructions; i++) {

    fwrite(&binaryInstructions[i], 4, 1, fptr);
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
  free(symbolTable);
  free(binaryInstructions);
  return EXIT_SUCCESS;
}
