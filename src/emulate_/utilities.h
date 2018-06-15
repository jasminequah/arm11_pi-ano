#ifndef UTILITIES_H
#define UTILITIES_H

#include "definitions.h"
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t logicalLeft(uint32_t n, int d);
uint32_t logicalRight(uint32_t n, int d);
uint32_t arithmeticRight(uint32_t n, int d);
uint32_t rotateRight(uint32_t n, int d);

int checkAddCarryOut(uint32_t a, uint32_t b);
int checkSubCarryOut(uint32_t a, uint32_t b);

uint32_t loadMemory(uint8_t *memory, uint32_t memLoc);
void storeMemory(state_t *state, uint32_t memLoc);
uint32_t barrelShifter(uint32_t value, int shiftAmount, int *carryOut, shiftType_t shiftType);
int checkGPIOPins(uint32_t newBase);
int checkCond(state_t *state, cond_t cond);

uint32_t getInstruction(state_t *state);
instr_t getInstructionNum(uint32_t pc);

state_t *newState(void);
void freeState(state_t *state);

#endif
