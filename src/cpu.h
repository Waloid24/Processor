#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <math.h>
#include "stack.h"

#define DEF_CMD(name, numLetters, numCmd, isArg, ...)    \
    CMD_##name = numCmd,

typedef enum {
    #include "cmd.h"
} COMMANDS;

#undef DEF_CMD

const size_t MAX_RAM = 100;
const int MASK = (1<<4) + (1<<3) + (1<<2) + (1<<1) + 1; //0001|1111

void cpu (int * code, size_t nStrs, size_t numTags);
size_t readNumStrs (FILE * binFile);
FILE * codeFile (char * nameFile);
int * getCode (size_t nStrs, FILE * binFile);
void dumpCode (int * code, size_t numElem);

#endif
