#ifndef PARSE_H
#define PARSE_H

#include "utilities.h"

uint32_t parseDataProcessing(map_t *symbolTable, char **tokens, instrName_t name, int numTokens);
uint32_t parseSDT(state_t* state, char **tokens, instrName_t name, int numTokens);
uint32_t parseMultiply(map_t *symbolTable, char **tokens, instrName_t name);
uint32_t parseBranch(map_t *symbolTable, char **tokens, instrName_t name, int currAddress);

#endif
