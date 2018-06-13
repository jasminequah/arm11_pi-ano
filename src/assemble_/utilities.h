#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "definitions.h"

uint32_t getOperand(char *expression);
uint32_t rotateLeftByTwo(uint32_t n);
void getImmediate(uint32_t *instrPtr, uint32_t operand2);
void getExprBits(uint32_t *instrPtr, char *expression);
void getShiftBits(uint32_t *instrPtr, char **remainingTokens);
void setReg(dataProcType_t instrType, char **tokens, uint32_t *instrPtr, int numTokens);
uint32_t evalExpression(char* expr);
uint32_t getRn(char **tokens);
void parseRm(char **tokens, uint32_t *instr, int numTokens);
uint32_t getMemAddress(map_t *symbolTable, char *label);
map_t newMap(char *label, uint16_t memAddress);
state_t* newState(int numOfInstructions, map_t *symbolTable);


#endif
