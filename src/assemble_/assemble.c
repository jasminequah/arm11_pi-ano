#include "definitions.h"
#include "utilities.h"
#include "ioutils.h"
#include "parse.h"

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
