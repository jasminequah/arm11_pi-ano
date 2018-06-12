#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define MAX_MAPS              10
#define MAX_INSTR_LEN         511
#define ADDR_INC              0x4
#define MAX_SYMBOL_TABLE_SIZE 50
#define BITS_IN_WORD          32
#define HEX_BASE              16
#define DECIMAL_BASE          10
#define PIPELINE_OFFSET       0x8

#define S_BIT             0x00100000
#define I_BIT             0x02000000
#define ALWAYS_COND_CODE  0xe0000000
#define EQ_COND_CODE      0x00000000
#define MAX_NO_ROTATE_IMM 0xff
#define MAX_ROTATE        0xf

#define PC_REG               15
#define MAX_CONSTANTS        20

#define OPCODE_SHIFT         21
#define ROTATE_SHIFT         8
#define SHIFT_BY_REG_BIT     0x00000010
#define RN_SHIFT             16
#define RD_SHIFT             12
#define RS_SHIFT             8
#define SHIFT_CONSTANT_SHIFT 7
#define SHIFT_TYPE_SHIFT     5
#define LOWER_24_BIT_MASK    0x00ffffff
#define COND_OFFSET          28
#define BRANCH_MASK          0x0a000000

typedef struct map {
  char *label;
  uint16_t memAddress;
} map_t;

typedef enum {
  ADD, SUB, RSB, AND, EOR, ORR, MOV, TST, TEQ, CMP, MUL, MLA, LDR, STR,
  BEQ, BNE, BGE, BLT, BGT, BLE, B, LSL, ANDEQ,
} instrName_t;

typedef enum {
  COMP_RESULT, SINGLE_OP_ASS, SET_CPSR,
} dataProcType_t;

typedef struct state {
	uint32_t *binaryInstructions;
	map_t *symbolTable;
	int numOfInstr;
	uint32_t currAddress;
	int numOfConstants; //num of constants stored at end of file, so we know where to write next constant
} state_t;

#endif
