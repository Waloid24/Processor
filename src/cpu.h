#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <math.h>
#include "stack.h"
#include "asm.h"

void cpu (char * nameFile, code_t fileInfo, int numTags);

#endif
