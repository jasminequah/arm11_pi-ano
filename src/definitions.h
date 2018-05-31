#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>

#define BITS_IN_WORD 32
#define MEMORY_SIZE  65536
#define NUM_REGS     17
#define PC_INCREMENT 0x4

/* Specific registers */
#define SP_REG   13
#define LR_REG   14
#define PC_REG   15
#define CPSR_REG 16

/* Masks for instruction code */
#define FIRST_4_BITS    0x0000000F
#define THIRD_4_BITS    0x00000F00
#define FIRST_BYTE      0x000000FF
#define BIT_31          0x80000000
#define BIT_4           0x00000010
#define BIT_0           0x00000001
#define SHIFT_TYPE_MASK 0x00000060
#define SHIFT_AMT_MASK  0x00000F80

/* Masks for CPSR_REG */
#define N_BIT 0x80000000
#define Z_BIT 0x40000000
#define C_BIT 0x20000000
#define V_BIT 0x10000000

/* Instructions */
typedef enum {
  DATA_PROCESSING,
  MULTIPLY,
  SINGLE_DATA_TRANSFER,
  BRANCH
} instr_t;

/* Conditions */
typedef enum {
  EQ = 0,
  NE = 1,
  GE = 10,
  LT = 11,
  GT = 12,
  LE = 13,
  AL = 14,
} cond_t;

/* OpCode for Data Processing */
typedef enum {
  AND = 0,
  EOR = 1,
  SUB = 2,
  RSB = 3,
  ADD = 4,
  TST = 8,
  TEQ = 9,
  CMP = 10,
  ORR = 12,
  MOV = 13,
} opCode_t;

/* ShiftType for Data Processing */
typedef enum {
  LSL,
  LSR,
  ASR,
  ROR
} shiftType_t;

/* components of decoded instruction */
typedef struct arm_decoded {
  cond_t cond;

  int isI;
  int isP;
  int isU;
  int isA;
  int isS;
  int isL;

  int rn;
  int rd;
  int rs;
  int rm;

  int32_t offset;
  opCode_t opCode;
  uint16_t operand2;

} decoded_t;

/* Holds the state of the emulator. */
typedef struct arm_state {
  uint32_t *registers;
  uint8_t *memory;

  decoded_t *decoded;

  int isTerminated;
} state_t;

#endif
