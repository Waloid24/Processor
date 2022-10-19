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
    N_HLT  = 0,
    N_PUSH = 1,
    N_POP  = 2
    N_ADD  = 3,
    N_SUB  = 4,
    N_MUL  = 5,
    N_DIV  = 6,
    N_OUT  = 7,
    N_JMP  = 8
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
void check_test_file (void);

#endif

// #define LISTING(ptr_mass_str, n_str) \
//     printf (LONG_LINE);\
//     printf ("This listing was called in a file - %s, in a function - %s, and in a line %d\n\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
//     if ((ptr_mass_str)[i])
//         for (int i = 0; i < n_str; i++)
//     printf ("%d, ")