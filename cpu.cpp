#include <stdio.h>

typedef struct {
    elem_t * buf;
    int n_elem;
} stack_t;

void cpu (FILE * asm_text)
{
    struct stat info_file = {};
    stack_t stk = {};
    size_t size_file_asm = stat ("asm.txt", &info_file);
    Ctor (&stk, size_file_asm);

    
}

void Ctor (stack_t * stk, size_t size_file_asm)
{
    //check_err (capacity); //capacity <= 0
    stk -> n_elem = 
    char * ptr = (char *) calloc  
    //check_err (ptr);

} 
