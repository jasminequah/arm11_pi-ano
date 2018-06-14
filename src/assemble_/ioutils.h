#ifndef IOUTILS_H
#define IOUTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "definitions.h"
#include "parse.h"

int firstPass(char* fileName, map_t *symbolTable);
void secondPass(char *fileName, state_t* state);
void writeBinary(char* fileName, state_t *state);

#endif
