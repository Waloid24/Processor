#include "assembler.h"

int main (int argc, char * argv[])
{
    //проверку на argc
    read_text_prog_ok();
    info_prog prog = {};
    prog.file_with_code = fopen (argv[1], "rb");
    FILE * arrayFiles = assembler (&prog, argv[1]);
    FILE * binFile = arrayFiles[0];
    FILE * textFile = arrayFiles[1];

    
    //cpu (code_file);

    return 0;
}
