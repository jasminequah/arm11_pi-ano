#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "definitions.h"

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
      printf("Error: Immediate value exceeds maximum representable value\n");
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
  printf("tokens: %s, %s, %s\n", remainingTokens[0], remainingTokens[1], remainingTokens[2]);
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

uint32_t evalExpression(char* expr) {
   if (expr[0] == '0' && expr[1] == 'x') {
		 return strtoul(&expr[2], NULL, 16);
	 } else {
		 return atoi(expr);
	 }
}

uint32_t getRn(char **tokens) {
  if (tokens[2][3] == ']') {
    tokens[2][3] = '\0';
  }
  return atoi(&tokens[2][2]) << 16;
}

void parseRm(char **tokens, uint32_t *instr, int numTokens) {
  uint32_t u = 0;
  uint32_t offset = 0;
  uint32_t i = 0;

  if (tokens[3][0] != '-') {
    u = 0x1 << 23;
  }
  if (numTokens == 6) {
    tokens[numTokens - 1][strlen(tokens[numTokens - 1]) - 1] = '\0';
    getShiftBits(instr, &tokens[3]);
  } else {
    offset = atoi(&tokens[3][1]);
  }
  i = 1 << 25;
  *instr = *instr | u | offset | i;
}

uint32_t getMemAddress(map_t *symbolTable, char *label) {
  map_t *currMap = symbolTable;
  while (1) {
    if (strcmp(currMap->label, label) == 0) {
      break;
    }
    currMap++;
  }
  return currMap->memAddress;
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
