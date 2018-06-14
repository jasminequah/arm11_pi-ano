#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "definitions.h"
#include "utilities.h"
#include "ioutils.h"

uint32_t parseDataProcessing(map_t *symbolTable, char **tokens, instrName_t name, int numTokens) {
  uint32_t opCode;

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
      printf("Error: invalid instruction\n");
      opCode = -1;
      break;
  }
  instruction = instruction | (opCode << OPCODE_SHIFT);
  return instruction;
}

uint32_t parseSDT(state_t* state, char **tokens, instrName_t name, int numTokens) {

	uint32_t binInstr = 0x04000000;
	uint32_t cond = 0xe0000000;
	uint32_t rd = atoi(&tokens[1][1]) << 12;
	uint32_t l;
	uint32_t p;
	uint32_t u = 0;
	uint32_t i = 0;
	uint32_t rn;
	uint32_t offset;
  uint32_t *binaryInstructions = state->binaryInstructions;

	if (tokens[2][0] == '=') {
    u = 0x1 << 23;
    uint32_t expr = strtoul(&tokens[2][3], NULL, 16);

		if (expr <= 0xFF) {
			tokens[2][0] = '#';
      tokens[0]    = "mov";
			return parseDataProcessing(state->symbolTable, tokens, MOV, 3);
		} else {
      uint32_t newLocation            = state->numOfInstr + state->numOfConstants;
			binaryInstructions[newLocation] = expr;
      state->numOfConstants          += 1;
			offset = 0x4 * (newLocation - state->currAddress) - 0x8;
      if (offset < 0) {
        offset = 0;
      }

			rn = PC_REG << 16;
			p  = 0x01000000;
			l  = 0x00100000;
			return binInstr | l | p | rn | rd | offset | cond | u;
		}
	}

  int preindexed = numTokens < 4 || tokens[numTokens - 1][strlen(tokens[numTokens - 1]) - 1] == ']';

  rn = getRn(tokens);

  if (preindexed) {
		p = 0x01000000;
    if (numTokens >= 4) {
      if (tokens[3][0] == 'r' || tokens[3][1] == 'r') {
        parseRm(tokens, &binInstr, numTokens);

      } else {
  			tokens[3][strlen(tokens[3]) - 1] = '\0';
  			offset = evalExpression(&tokens[3][2]);
        if (tokens[3][1] != '-') {
          u      = 0x1 << 23;
          offset = evalExpression(&tokens[3][1]);
        }
      }
		} else {
			offset = 0;
      u      = 0x1 << 23;
		}
	} else {
    p = 0;

    if (tokens[3][0] == 'r' || tokens[3][1] == 'r') {
       parseRm(tokens, &binInstr, numTokens);

    } else {
      offset = evalExpression(&tokens[3][2]);
      if (tokens[3][2] != '-') {
        u      = 0x1 << 23;
        offset = evalExpression(&tokens[3][1]);
      }
    }
	}

	if (name == LDR) {
		l = 0x00100000;
	} else {
		l = 0;
	}

	binInstr = binInstr | cond | rd | l | p | u | i | rn | offset;
	return binInstr;
}

uint32_t parseMultiply(map_t *symbolTable, char **tokens, instrName_t name) {
	uint32_t code = 0xe0 << 24;

	char* registers;
	int num;
	if (name == MUL) {
		code += (0x0 << 20);
	}
	else {
		code += (0x2 << 20);
	}

	registers = tokens[1];
	num = registers[1] - '0';
	if (registers[2] != '\0') {
		num = (num * 10) + (registers[2] - '0');
	}
	code += (num << 16);

	registers = tokens[2];
	num = registers[1] - '0';
	if (registers[2] != '\0') {
		num = (num * 10) + (registers[2] - '0');
	}
	code += ((0x9 << 4) + num);

	registers = tokens[3];
	num = registers[1] - '0';
	if (registers[2] != '\0') {
		num = (num * 10) + (registers[2] - '0');
	}
	code += (num << 8);

	if (name == MLA) {
		registers = tokens[4];
		num = registers[1] - '0';
		if (registers[2] != '\0') {
			num = (num * 10) + (registers[2] - '0');
		}
		code += (num << 12);
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
        printf("Error: invalid condition\n");
        cond = -1;
        break;
    }
  uint32_t destAddress = getMemAddress(symbolTable, tokens[1]);
  int32_t signedOffset = (((int) destAddress - currAddress) - PIPELINE_OFFSET);
  int32_t offset       = (signedOffset >> 2) & LOWER_24_BIT_MASK;
  return (cond << COND_OFFSET) | BRANCH_MASK | offset;
}
