#include "assemblerNew.h"

FILE * assembler (info_prog * prog, char * arg_console)
{
    FILE * arrayFiles = (FILE *) calloc (2, sizeof (FILE *));
    
    char * arrStrings = createArrStrings(prog, arg_console);
    arrayFiles[0] = createBinFile (arrStrings, prog);


    //arrayFiles[1] = createTextFile (prog, );

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

    char ** array_strings = arrayPointerToStrings (prog->n_strings, sizeof(char *));

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

char ** arrayPointerToStrings (int n_strings, size_t size)
{
	char ** array = (char **)calloc (n_strings, size);
	MY_ASSERT (array == NULL, "Memory allocation error\n");
	return array;
}

void filling_struct (char ** arrPtrsToStrings, char * buf, int n_elem) 
{
	arrPtrsToStrings[0] = buf;
	for (int num_sym = 0, j = 0; num_sym < n_elem && *buf != EOF; num_sym++)
	{
		//arr_struct_string
		if (*buf == '\0')
		{
            while (*buf == '\0')
            {
                buf++;
            }
			arrPtrsToStrings[j+1] = buf;
            printf ("arrPtrsToStrings[%d].str = %s\n", j, arrPtrsToStrings[j]);
			j++;
		}
		buf++;
	}
}

// FILE * createTextFile (info_prog * prog, char * arg_console)
// {
    
// }

FILE * createBinFile (char ** arrStrs, info_prog * prog, )
{
    char * code = (char *) calloc (prog->n_strings * 3, sizeof(int)); //возможно, будет мало места.
    #define DEF_CMD(nameCmd, countLetters, numCmd, isArg)\
        if (my_strcmp (cmd, #nameCmd) == 0)\
        {\
            // code[ip++] = CMD_##nameCmd;
            // if (isArg)\
            // {\
            //     countArgs++;\
            //     getArg (&(code[ip++]), array_strings[i].str + countLetters, &ip, numCmd);\ //ДОПИСАТЬ АРГУМЕНТЫ
            // }

            if (isArg)
            {
                countArgs++;
                getArg (&(code[ip++]), arrStrs[i], countLetters, numCmd);
            }
            else 
                code[ip++] = CMD_##nameCmd;

        }\
        else

    char * cmd = (char *) calloc (STANDART_SIZE, sizeof(char));
    int countArgs = 0;
    for (int i = 0, ip = -1; i < prog->n_strings; i++)
    {
        sscanf (arrStrs[i], "%s", cmd);

        isMark ();

        #include "cmd.h"
        {
            printf ("This command is not defined.\n");
        }
    }
    free (cmd);
    #undef DEF_CMD
}

void getArg (unsigned char * code, char * str_text_code, int countLetters, int numCmd)
{
	char * firstBracket = nullptr;
    if ((firstBracket = strchr (str_text_code, '[')) != nullptr)
    {
        areSquareBrackets(code, firstBracket, numCmd);
    }
    else
	{
		printf ("There isn't bracket\n");
        otherForm(code, str_text_code, countLetters, numCmd);
	}
		
}

void otherForm (unsigned char * code, char * str_text_code, int countLetters, int numCmd)
{
    printf ("numCmd = %d\n", numCmd);

	char * reg  = (char *) calloc (3, sizeof(char));
    MY_ASSERT (reg == nullptr, "It's impossible to read the argument");

    char * trash = (char *) calloc (3, sizeof(char));
    MY_ASSERT (trash == nullptr, "func GetArgument: it's impossible to read other symbols");

	int num = -1;
    char * ptrToArg = skipSpace (str_text_code, countLetters);

	printf ("*ptrToArg = %c\n", *ptrToArg);

    if (my_strcmp (ptrToArg, "r") == 0)
    {
		printf ("There is register\n");
        sscanf (ptrToArg, "%[rabcdx]%[ +]%d", reg, trash, &num); //обработка push rax + 5
    }
    else 
    {
		printf ("There isn't register\n");
        sscanf (ptrToArg, "%d%[ +]%[rabcdx]", &num, trash, reg); //обработка push 5 + rax 
    }
    
    MY_ASSERT (num == -1 && *reg == 0, "Your argument in square brackets are incorrect");

	printf ("Your reg = %s, trash = %s, num = %d\n", reg, trash, num);

    if (num == -1 && *reg != 0) //на всякий случай        //ситуация вида push rax и pop rax 
    {
		printf ("reg != 0 RRRRRRRRRRRRR\n");
        char count_reg = 0;
        if      (my_strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (my_strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (my_strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (my_strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else                                  MY_ASSERT (1, "The case is specified incorrectly");


        if (numCmd == CMD_PUSH) //0000 | 0000
        {                       //0100 | 0001
            *code = 65;
            code++;
            *code = count_reg;
        }

        else if (numCmd == CMD_POP) //0100 | 0010
        {
            *code = 66;
            code++;
            *code = count_reg;
        }
        else //это случай jmp     //0100 | 1000
        {
            *code = 72;
            code++;
            *code = count_reg;
        }

    }

    else if (*reg == 0 && num != -1) //ситуация вида push 5 и pop 5
    {
		printf ("this if\n");
        if (numCmd == CMD_PUSH) //0010 | 0001
        {  
			printf ("command is push 111\n");
            *code = 33;
            code++;
            *((int *) code) = num;
        }
            
        else if (numCmd == CMD_POP)  //if менять на else //0010 | 0010
        {
			printf ("command is pop 222\n");
            *code = 34;
            code++;
            *((int *) code) = num;
        }
        else //это jmp     //0010 | 1000
		{
			printf ("command is jmp 333\n");
			*code = 40;
            code++;
            *((int *) code) = num;
		}
            
    }
    else //ситуация вида push 5 + rax, или pop 5 + rax, или rax + 5
    {
        char count_reg = 0;
        if      (my_strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (my_strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (my_strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (my_strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else                                  MY_ASSERT (1, "The case is specified incorrectly");

        if (numCmd == CMD_PUSH) //0110 | 0001
        {
            *code = 97;
            code++;
            *((int *) code)  = num;
            code++;
            *((char *) code) = count_reg;
        }

        else if (numCmd == CMD_POP) //0110 | 0010
        {
            *code = 98;
            code++;
            *((int *) code)  = num;
            code++;
            *((char *) code) = count_reg;
        }
        else //0110 | 1000
        {
            *code = 104;
            code++;
            *((int *) code)  = num;
            code++;
            *((char *) code) = count_reg;
        }
    }
    free (reg);
    free (trash);
}

char * skipSpace (char * str_text_code, int countLetters)
{
    char * aftercommand = str_text_code + countLetters;
	int i = 0;
    for (; aftercommand[i] == ' '; i++)
    {
        ;
    }
    return &(aftercommand[i]);
}

void areSquareBrackets (unsigned char * code, char * firstBracket, int numCmd)
{ 
	printf ("Works areSquareBrackets func\n");
	printf ("numCmd = %d\n", numCmd);

    char * reg  = (char *) calloc (3, sizeof(char));
    MY_ASSERT (reg == nullptr, "It's impossible to read the argument");
    char * trash = (char *) calloc (3, sizeof(char));
    MY_ASSERT (trash == nullptr, "func GetArgument: it's impossible to read other symbols");
	int num = -1;

    if (my_strcmp (firstBracket+1, "r") == 0)
    {
        sscanf (firstBracket+1, "%[rabcdx]%[ +]%d", reg, trash, &num); //обработка push [rax + 5]
    }
    else 
    {
        sscanf (firstBracket+1, "%d%[ +]%[rabcdx]", &num, trash, reg); //обработка push [5 + rax] 
    }

    MY_ASSERT (num == -1 && *reg == 0, "Your argument in square brackets are incorrect");

    if (num == -1 && *reg != 0) //на всякий случай        //ситуация вида push [rax] и pop [rax]
    {
        char count_reg = 0;
        if      (my_strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (my_strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (my_strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (my_strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else                                  MY_ASSERT (1, "The case is specified incorrectly");


        if (numCmd == CMD_PUSH) //0000 | 0000
        {                       //1100 | 0001
			printf ("There is push 222\n");
            *code = 193;
            code++;
            *code = count_reg;
        }

        if (numCmd == CMD_POP) //1100 | 0010
        {
			printf ("There is pop 222\n");
            *code = 194;
            code++;
            *code = count_reg;
        }
    }
    
    else if (*reg == 0 && num != -1) //ситуация вида push [5] и pop [5]
    {
        if (numCmd == CMD_PUSH) //1010 | 0001
        {
			printf ("There is push 333\n");

            *code = 161;

            code++;
            *((unsigned int *) code) = num;
        }
            
        if (numCmd == CMD_POP)  //if менять на else //1010 | 0010
        {
			printf ("There is pop 333\n");
            *code = 162;
            code++;
            *((unsigned int *) code) = num;
        }
    }
    else //ситуация вида push [5 + rax] или pop [5 + rax] или [rax + 5]
    {
		char count_reg = 0;
        if      (my_strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (my_strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (my_strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (my_strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else                                  MY_ASSERT (1, "The case is specified incorrectly");

        if (numCmd == CMD_PUSH) //1110 | 0001
        {
			printf ("There is push 444\n");
            *code = 225;
            code++;
            *((unsigned int *) code)  = num;
            code++;
            *((unsigned char *) code) = count_reg;
        }

        if (numCmd == CMD_POP) //1110 | 0010
        {
			printf ("There is pop 444\n");
            *code = 226;
            code++;
            *((unsigned int *) code)  = num;
            code++;
            *((unsigned char *) code) = count_reg;
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

void Push (stack_t * stk, elem_t new_memb)
{
    MY_ASSERT (stk == nullptr, "No stack access");
    //FILE * logfile = fopen ("log.txt", "a");
    
    if (stk->n_memb >= stk->capacity)
    {
        resize (stk, increase);
    }
    stk->data[stk->n_memb] = new_memb;
    stk->n_memb++;
    // ToDo: n_memb++ here ... done

    // hash_sum (stk);
    // STK_OK (stk);
    fclose (logfile);
}

elem_t Pop (stack_t * stk)
{
    stk->n_memb--;

    MY_ASSERT (stk->n_memb < 0, "Error : there are no variables in the stack"); //было бы прикольно как-то, не ломая, обработать

    elem_t pop = (stk->data)[stk->n_memb];
    (stk->data)[stk->n_memb] = POISON;

    if (THRESHOLD_RATIO*(stk->n_memb) <= stk->capacity && (str->capacity/RESIZE) >= (stk -> minCapacity)) 
    {
        resize(stk, reduce);
    }

    return pop;
}

void Ctor (stack_t * stk, size_t capacity)
{
    stk -> n_memb = 0;
    stk -> capacity = capacity;

    char * data = (char *) calloc ((stk->capacity)*sizeof(elem_t), sizeof(char));
    stk -> data = (elem_t *)data;    
} 

void resize (stack_t * stk, int mode)
{
    if (mode == increase)
    {
        stk->capacity *= RESIZE;
        cnr_t temp     = *(stk->ptr_canary_data_second);
        //printf ("in resize before recalloc stk->ptr_canary_data_second = %p\n", stk->ptr_canary_data_second);
        //printf ("The value of stk->ptr_canary_data_second BEFORE resize = %llX\n", *(stk->ptr_canary_data_second));

        char * ptr = (char *) recalloc (stk->ptr_canary_data_first, (stk->capacity)*sizeof(elem_t) + 2*sizeof(hash_t), sizeof(char));

        stk->ptr_canary_data_first     = (hash_t *) ptr;
        stk->data                      = (elem_t *)((char *)stk->ptr_canary_data_first + sizeof (hash_t));
        stk->ptr_canary_data_second    = (cnr_t *)((char *)stk->ptr_canary_data_first + (char)((stk->capacity)*sizeof(elem_t)) + sizeof(hash_t));
        *(stk->ptr_canary_data_second) = temp;

        MY_ASSERT (stk->ptr_canary_data_first == nullptr, "New pointer after resize is nullptr");

        for (int i = 0; i < (stk->capacity - stk->n_memb); i++)
        {
            *((elem_t *)stk->data + (elem_t)i + (elem_t)stk->n_memb) = POISON;
        }

        //stk->ptr_canary_data_first = (cnr_t *) ptr;
        //printf ("in resize after recalloc stk->ptr_canary_data_second = %p\n", stk->ptr_canary_data_second);
        //printf ("The value of stk->ptr_canary_data_second AFTER resize = %llX\n", *(stk->ptr_canary_data_second));
        //check_err (); stk->data != nullptr
    }
    else if (mode == reduce)
    {
        stk->capacity /= RESIZE;
        cnr_t temp = *(stk->ptr_canary_data_second);

        //printf ("in resize before recalloc stk->ptr_canary_data_second = %p\n", stk->ptr_canary_data_second);
        //printf ("The value of stk->ptr_canary_data_second BEFORE resize = %llX\n", *(stk->ptr_canary_data_second));

        char * ptr = (char *) recalloc (stk->ptr_canary_data_first, (stk->capacity)*sizeof(elem_t) + 2*sizeof(hash_t), sizeof(char));

        stk->ptr_canary_data_first     = (hash_t *) ptr;
        stk->data                      = (elem_t *)((char *)stk->ptr_canary_data_first + sizeof (hash_t));
        stk->ptr_canary_data_second    = (cnr_t  *)((char *)stk->ptr_canary_data_first + (char)((stk->capacity)*sizeof(elem_t)) + sizeof(hash_t));
        *(stk->ptr_canary_data_second) = temp;

        MY_ASSERT (stk->ptr_canary_data_first == nullptr, "New pointer after resize is nullptr");

        for (int i = 0; i < (stk->capacity - stk->n_memb); i++)
        {
            *((elem_t *)stk->data + (elem_t)i + (elem_t)stk->n_memb) = POISON;
        }

        //stk->ptr_canary_data_first = (cnr_t *) ptr;
        //printf ("in resize after recalloc stk->ptr_canary_data_second = %p\n", stk->ptr_canary_data_second);
        //printf ("The value of stk->ptr_canary_data_second AFTER resize = %llX\n", *(stk->ptr_canary_data_second));
        //check_err (); stk->data != nullptr
    }
    else 
    {
        printf ("Incorrect operation mode of the function resize\n");
        abort ();
    }
}


// FILE * open_asm_text (void)
// {
//     FILE * asm_text = fopen ("asm.txt", "w");
//     MY_ASSERT(asm_text == nullptr, "Unable to access the file \"asm\"");
//     return asm_text;
// }