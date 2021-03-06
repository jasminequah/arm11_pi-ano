#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>

#define BITS_IN_WORD 32
#define MEMORY_SIZE  65536
#define NUM_REGS     17
#define PC_INCREMENT 0x4
#define PIPELINE_OFFSET 0x8

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

/*Shift amounts for the registers*/
#define RN_SHIFT 12
#define RD_SHIFT 16
#define RM_SHIFT 28
#define OFFSET_SHIFT 20
#define RS_SHIFT 20
#define PC_SHIFT 28

/*Bit masks for I,S,A,P,etc*/
#define IS_I 0x02000000u
#define IS_S 0x00100000u
#define IS_P 0x01000000u
#define IS_U 0x00800000u
#define IS_L 0x00100000u
#define IS_A 0x00200000u

/* GPIO pins */
#define PIN_OFF      0x20200028u
#define PIN_ON       0x2020001cu
#define PIN_0_TO_9   0x20200000u
#define PIN_10_TO_19 0x20200004u
#define PIN_20_TO_29 0x20200008u

/*Function pointers */
typedef void (*func)();

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

typedef struct arm_pipeline {
  decoded_t *decoded;
  uint32_t fetched;
} pipeline_t;

/* Holds the state of the emulator. */
typedef struct arm_state {
  uint32_t *registers;
  uint8_t *memory;

  pipeline_t *pipeline;

  instr_t instruction;
  uint32_t bitInstruction;

  int isFetched;
  int isDecoded;
  int isTerminated;
} state_t;

#endif
