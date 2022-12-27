#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "readCode.h"
#include "my_string.h"
#include "MY_ASSERT.h"

#define STANDART_SIZE 20
#define LONG_LINE "-----------------------------------------------------------------------------------------------------\n"

typedef enum  {
    CMD_PUSH = 1,
    CMD_POP  = 2,
    CMD_ADD  = 3,
    CMD_SUB  = 4,
    CMD_MUL  = 5,
    CMD_DIV  = 6,
    CMD_OUT  = 7,
    CMD_IN   = 8,
    CMD_JMP  = 9,
    CMD_CALL = 10,
    CMD_RET  = 11,
    CMD_JA   = 12,
    CMD_JB   = 13,
    CMD_JBE  = 14,
    CMD_JGE  = 15,
    CMD_JE   = 16,
    CMD_HLT  = 17,
    CMD_SQRT = 18,
    CMD_MEOW = 19
} COMMANDS;

enum {
    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3
};

typedef int elem_t;

struct tag {
    int * ptr;
    char * nameTag;
};
typedef struct tag tag_t;

struct freeCall {
    int * ptrToArrWithCode;
    char * tag;
    int numFreeCall;
};
typedef struct freeCall freeCall_t;

void createBinFile (char ** arrStrs, code_t * prog, char * nameBinFile, size_t numTags);
void pushSignature (char * nameBinFile, code_t code);
void dumpCode (char ** arrStrs, int * code, size_t numElem);

#endif