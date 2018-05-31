#ifndef IOUTILS_H
#define IOUTILS_H

#include "definitions.h"
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

void printState(state_t *state);
void printDecoded(decoded_t *decoded);
void readBinary(state_t *state, char* fileName);

#endif
