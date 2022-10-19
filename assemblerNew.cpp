#include "assemblerNew.h"

FILE * assembler (info_prog * prog, char * arg_console)
{
    FILE * arrayFiles = (FILE *) calloc (2, sizeof (FILE *));
    
    char * arrStrings = createArrStrings(prog, arg_console);
    arrayFiles[0] = createBinFile (arrStrings, prog);
    arrayFiles[1] = createTextFile (prog, );

    return arrayFiles;
}

char * createArrStrings (info_prog * prog, char * arg_console)
{
    const int version = 1;

    prog -> size_file = size_file(arg_console);

    //printf ("size_file = %d\n", prog->size_file);

    writeTextToBuf (prog);

    //check_buf(prog->buf);

    prog->n_strings = correct_buf (prog -> buf, prog->size_file);

    //check_buf(prog->buf);

    char * array_strings = arrayPointerToStrings (prog->n_strings, sizeof(char *));

    filling_struct (array_strings, prog->buf, prog->size_file);
}

int size_file (char * arg_console)
{
    struct stat about_prog = {};
    read_text_prog_ok();
    size_t val = stat (arg_console, &about_prog);

    return about_prog.st_size;
}

void writeTextToBuf (info_prog * prog)
{
    prog -> buf = (char *) calloc ((prog->size_file)+1, sizeof(char));
    
    //printf ("prog -> buf = %p\n", prog -> buf);

    fread (prog -> buf, (prog->size_file)+1, sizeof(char), prog->file_with_code);

    (prog -> buf)[prog->size_file] = '\0';
}

int correct_buf (char * buf, int n_elements)  //заменить в коде всё на маленькие буквы!
{
	int n_strings = 0;

	for (int i = 0; i < n_elements; i++)
	{   
        if (buf[i] == ';')
        {
            printf ("%c\n", buf[i]);
            while (buf[i] != '\n')
            {
                buf[i] = '\0';
                printf ("%c\n", buf[i]); 
                i++;
                continue;
            }
            buf[i] = '\0'; 
            n_strings++; 
        }

		if (buf[i] == '\n')
		{
			buf[i] = '\0';
            // printf ("buf[i+1] = %c\n", buf[i+1]);
            // if (buf[i+2] != ' ')
			n_strings++;
		}
        // printf ("%c", buf[i]);
	}
    printf ("n_strings = %d\n", n_strings);
    // printf ("\n");
    // for (int i = 0; i < n_elements; i++)
	// {
    //     printf ("%c", buf[i]);
	// }
    // printf ("\n");
	return n_strings;
}

char * arrayPointerToStrings (int n_strings, size_t size)
{
	char * array = (char *)calloc (n_strings, size);
	MY_ASSERT (array == NULL, "Memory allocation error\n");
	return array;
}

void filling_struct (char * arrPtrsToStrings, char * buf, int n_elem) 
{
	arrPtrsToStrings[0].str = buf;
	for (int num_sym = 0, j = 0; num_sym < n_elem && *buf != EOF; num_sym++)
	{
		//arr_struct_string
		if (*buf == '\0')
		{
            while (*buf == '\0')
            {
                buf++;
            }
			arrPtrsToStrings[j+1].str = buf;
            printf ("arrPtrsToStrings[%d].str = %s\n", j, arrPtrsToStrings[j].str);
			j++;
		}
		buf++;
	}
}

// FILE * createTextFile (info_prog * prog, char * arg_console)
// {
    
// }

FILE * createBinFile (char * arrStrs, info_prog * prog, )
{
    int * code = calloc (prog->n_strings * 3, sizeof(int)); //возможно, будет мало места.
    #define DEF_CMD(nameCmd, countLetters, numCmd, isArg)\
        if (my_strcmp (cmd, #nameCmd) == 0)\
        {\
            code[ip++] = CMD_##nameCmd;\
            if (isArg)\
            {\
                countArgs++;\
                getArg (&(code[ip++]), array_strings[i].str + countLetters, &ip);\ //ДОПИСАТЬ АРГУМЕНТЫ
            }\
            code[i++] = '\n';\
        }\
        else

    char * cmd = (char *) calloc (STANDART_SIZE, sizeof(char));
    int countArgs = 0;
    for (int i = 0, ip = -1; i < prog->n_strings; i++, ip++)
    {
        sscanf (array_strings[i].str, "%s", cmd);
        
        #include "cmd.h"
        {
            printf ("This command is not defined.\n");
        }
    }
    free (cmd);
    #undef DEF_CMD
}

void getArg (int * code, char * str, int * register, int * ram, int * i)
{
    char * firstBracket = nullptr;
    if ((firstBracket = strchr (str, '[')) != nullptr)
    {
        char * secondBracket = strchr (str, ']');

        //(char * ptrSym = strchr (firstBracket+1, 'r')) != nullptr && ptrSym < secondBracket

        char * reg  = (char *) calloc (3, sizeof(char));
        char * trash = (char *) calloc (3, sizeof(char));
	    int num = -1;

        if (my_strcmp (firstBracket+1, "r") == 0)
        {
            sscanf (firstBracket+1, "%[rabcdx]%[ +]%d", reg, trash, &num); //обработка push [rax + 5]
        }
        else 
        {
            sscanf (firstBracket+1, "%d%[ +]%[rabcdx]", &num, trash, reg); //обработка push [5 + rax] 
        }
        MY_ASSERT (num == -1 && reg == 0, "Your argument in square brackets are incorrect");
        if (num == -1 && reg != 0) //на всякий случай        //ситуация вида push [rax]
        {
            if      (my_strcmp (reg, "rax") == 0) int count_reg = RAX;
            else if (my_strcmp (reg, "rbx") == 0) int count_reg = RBX;
            else if (my_strcmp (reg, "rcx") == 0) int count_reg = RCX;
            else if (my_strcmp (reg, "rdx") == 0) int count_reg = RDX;
            else                                  MY_ASSERT (1, "The case is specified incorrectly");

            int numInRegister = register[count_reg];
            int numInRAM      = ram[numInRegister];

            Push (code, numInRAM)
        }
        else if (reg == 0 && num != -1) //ситуация вида push [5]
        {
            int numInRAM = ram[num];
            Push (code, numInRAM);
        }
        else //ситуация вида push [5 + rax] или push [rax + 5]
        {
            if      (my_strcmp (reg, "rax") == 0) int count_reg = RAX;
            else if (my_strcmp (reg, "rbx") == 0) int count_reg = RBX;
            else if (my_strcmp (reg, "rcx") == 0) int count_reg = RCX;
            else if (my_strcmp (reg, "rdx") == 0) int count_reg = RDX;
            else                                  MY_ASSERT (1, "The case is specified incorrectly");

            int numInRegister = register[count_reg];
            int numInRAM      = ram[numInRegister + num];
            
            Push (code, numInRAM);
        }
    }

    free (reg);
    free (trash);
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

void pushSignature (FILE * dst)
{
    //FILE * bincode = fopen ("BinaryCode.bin", "w");
	MY_ASSERT (dst == nullptr, "null pointer to file");
	int * signature = (int *) calloc (N_COUNT_SIGNATURE, sizeof(int));
	signature[0] = 'ASM';
	signature[1] = version;
	signature[2] = n_strings;
	
	fwrite (signature, sizeof(int), 3, dst);
}

void Push (int * stk, int new_memb)
{
  
}

int Pop (int * stk)
{
    
}

// FILE * open_asm_text (void)
// {
//     FILE * asm_text = fopen ("asm.txt", "w");
//     MY_ASSERT(asm_text == nullptr, "Unable to access the file \"asm\"");
//     return asm_text;
// }