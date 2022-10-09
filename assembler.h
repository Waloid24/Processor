#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include "my_assert.h"
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>

#define STANDART_SIZE 20
#define LONG_LINE "-----------------------------------------------------------------------------------------------------\n"

typedef int elem_t;

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
    CMP_PUSH,
    CMP_ADD,
    CMP_SUB,
    CMP_MUL,
    CMP_DIV,
    CMP_OUT,
    CMP_HLT
} COMMANDS;

void read_text_prog_ok (void);
FILE * assembler (info_prog * prog, char * arg_console);
int size_file (char * arg_console);
int my_strcmp (const char * string1, const char * string2);
int correct_buf (char * buf, int n_elements);
void check_buf (char * buf);
FILE * open_asm_text (void);
str_info * array_space (int n_strings, size_t size);
void filling_struct (str_info * arr_struct_string, char * buf, int n_elem);

#endif
