#ifndef READ_CODE_H
#define READ_CODE_H

#include <stdio.h>
#include <sys/stat.h>
#include "MY_ASSERT.h"

struct codeInfo
{
    size_t sizeFile;
    char * buf;
    int nStrs;
    char ** arrStrs;
};
typedef struct codeInfo code_t;

code_t readCode (const char * nameFile);

#endif