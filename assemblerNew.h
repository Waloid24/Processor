#include <stdio.h>
#include "my_assert.h"
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int size_file;
    FILE * file_with_code;
    char * buf;
    int n_strings;
} info_prog;

typedef struct {
	char * str = nullptr;
	int length_str = 0;
} str_info;

typedef enum  {
    CMD_HLT  = 0,
    CMD_PUSH = 1,
    CMD_POP  = 2
    CMD_ADD  = 3,
    CMD_SUB  = 4,
    CMD_MUL  = 5,
    CMD_DIV  = 6,
    CMD_OUT  = 7,
    CMD_JMP  = 8
} COMMANDS;

enum {
    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3
};