#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <math.h>
#include "stack.h"
#include "asm.h"

const int MAX_RAM = 100;
const int MASK = (1<<4) + (1<<3) + (1<<2) + (1<<1) + 1; //0001|1111

void cpu (int * code, int nStrs, int numTags);
int readNumStrs (FILE * binFile);
FILE * codeFile (char * nameFile);
int * getCode (int nStrs, FILE * binFile);

#endif
