#include <stdio.h>
#include "my_assert.h"
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>

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
void assembler (info_prog * prog, char * arg_console);
int size_file (char * arg_console);
int my_strcmp (const char * string1, const char * string2);
int correct_buf (char * buf, int n_elements);
void check_buf (char * buf);
FILE * open_asm_text (void);
str_info * array_space (int n_strings, size_t size);
void filling_struct (str_info * arr_struct_string, char * buf, int n_elem);

int main (int argc, char * argv[])
{
    //проверку на argc
    read_text_prog_ok();
    info_prog prog = {};
    prog.file_with_code = fopen (argv[1], "r");
    assembler (&prog, argv[1]);

    return 0;
}


void read_text_prog_ok (void)
{
    
    FILE * program = fopen ("prog.txt", "r");
    MY_ASSERT (program == nullptr, "Unable to access the file \"prog\"");
    fclose (program);
}

void assembler (info_prog * prog, char * arg_console)
{
    prog -> size_file = size_file(arg_console);

    printf ("size_file = %d\n", prog->size_file);

    prog -> buf = (char *) calloc ((prog->size_file)+1, sizeof(char));
    printf ("prog -> buf = %p\n", prog -> buf);
    fread (prog -> buf, (prog->size_file)+1, sizeof(char), prog->file_with_code);

    
    (prog -> buf)[prog->size_file] = '\0';
    check_buf(prog->buf);

    prog->n_strings = correct_buf (prog -> buf, prog->size_file);
    str_info * array_strings = array_space (prog->n_strings, sizeof(str_info));
    filling_struct (array_strings, prog->buf, prog->size_file);

    FILE * asm_text = open_asm_text();
    printf ("prog->n_strings = %d\n", prog->n_strings);
    for (int i = 0; i < prog->n_strings; i++)
    {
        char cmd[] = "";

        printf ("array_strings[%d].str = %s\n", i, array_strings[i].str);
 
        sscanf (array_strings[i].str, "%s", cmd);
        if (my_strcmp (cmd, "CP") == 0)
        {
            fprintf (asm_text, "%s", cmd);
        }
        else if (my_strcmp(cmd, "push") == 0)
        {
            int val = -1;
            char * str = nullptr;
            sscanf (array_strings[i].str, "%s%d", str, &val);
            fprintf (asm_text, "%d %d", CMP_PUSH, val);
        }
        else if (my_strcmp(cmd, "pop") == 0)
        {
            fprintf (asm_text, "%d", CMP_ADD);
        }
        else if (my_strcmp(cmd, "sub") == 0)
        {
            fprintf (asm_text, "%d", CMP_SUB);
        }
        else if (my_strcmp(cmd, "mul") == 0)
        {
            fprintf (asm_text, "%d", CMP_MUL);
        }
        else if (my_strcmp(cmd, "div") == 0)
        {
            fprintf (asm_text, "%d", CMP_DIV);
        }
        else if (my_strcmp(cmd, "out") == 0)
        {
            fprintf (asm_text, "%d", CMP_OUT);
        }
        else if (my_strcmp(cmd, "hlt") == 0)
        {
            fprintf (asm_text, "%d", CMP_HLT);
        }
        else 
        {
            printf ("Invalid command; number this command -- %d --\n", i);
            continue;
        }
    }
}

int size_file (char * arg_console)
{
    printf ("arg_console = %s\n", arg_console);
    struct stat about_prog = {};
    read_text_prog_ok();
    size_t val = stat (arg_console, &about_prog);
    printf ("val = %zu\n", val);

    return about_prog.st_size;
}

int my_strcmp (const char * string1, const char * string2)
{
	for (int i = 0, j = 0; string1[i] != '\0' && string2[j] != '\0'; i++, j++)
	{
		while (!isalpha(string1[i]) && string1[i] != '\0') //заменить на while 
			i++;
			
		while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;
		
		if (tolower(string1[i]) == tolower(string2[j]))
			continue;
		return (tolower(string1[i]) - tolower(string2[j]));
	}
	return (tolower(string1[0]) - tolower(string2[0]));
}

int correct_buf (char * buf, int n_elements) 
{
	int n_strings = 0;
	
	for (int i = 0; i < n_elements; i++)
	{
		if (buf[i] == '\n')
		{
			buf[i] = '\0';
            // printf ("buf[i+1] = %c\n", buf[i+1]);
            // if (buf[i+2] != ' ')
			n_strings++;
		}
	}
	return n_strings;
}

void check_buf (char * buf)
{
    printf ("buf = %p\n", buf);
    FILE * test = fopen ("test.txt", "w");
    MY_ASSERT (test == nullptr, "Unable to access the file \"test\"");
    // for (int i = 0; i < 50; i++)
    // {
    //     printf ("%c", buf[i]);
    // }
    fprintf (test, "%s", buf);
    fflush(test);
}

FILE * open_asm_text (void)
{
    FILE * asm_text = fopen ("asm.txt", "w");
    MY_ASSERT(asm_text == nullptr, "Unable to access the file \"asm\"");
    return asm_text;
}

str_info * array_space (int n_strings, size_t size)
{
	str_info * array = (str_info *)calloc (n_strings, size);
	MY_ASSERT (array == NULL, "Memory allocation error\n");
	return array;
}

void filling_struct (str_info * arr_struct_string, char * buf, int n_elem)
{
	arr_struct_string[0].str = buf;
	for (int num_sym = 0, j = 0; num_sym < n_elem && *buf != EOF; num_sym++)
	{
		(arr_struct_string[j].length_str)++;
		if (*buf == '\0')
		{
			arr_struct_string[j+1].str = buf + 1;
			j++;
		}
		buf++;
	}
}
