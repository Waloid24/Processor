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
            if (isArg)\
            {\
                countArgs++;\
                getArg (&(code[ip++]), array_strings[i].str + countLetters, &ip, numCmd);\ //ДОПИСАТЬ АРГУМЕНТЫ
            }\
            else\
            {\
                code[ip++] = CMD_##nameCmd;\
            }\
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

void getArg (int * code, char * str, int * register, int * ram, int * i, int numCmd)
{
    char * firstBracket = nullptr;

    if ((firstBracket = strchr (str, '[')) != nullptr)
    {


        char * secondBracket = strchr (str, ']');

        //(char * ptrSym = strchr (firstBracket+1, 'r')) != nullptr && ptrSym < secondBracket

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

        MY_ASSERT (num == -1 && reg == 0, "Your argument in square brackets are incorrect");

        if (num == -1 && reg != 0) //на всякий случай        //ситуация вида push [rax] и pop [rax]
        {
            if      (my_strcmp (reg, "rax") == 0) int count_reg = RAX;
            else if (my_strcmp (reg, "rbx") == 0) int count_reg = RBX;
            else if (my_strcmp (reg, "rcx") == 0) int count_reg = RCX;
            else if (my_strcmp (reg, "rdx") == 0) int count_reg = RDX;
            else                                  MY_ASSERT (1, "The case is specified incorrectly");


            if (numCmd == CMD_PUSH)
            {
                int numInRegister = register[count_reg];
                int numInRAM      = ram[numInRegister];
                Push (stk, numInRAM); //должны обработать аргумент и положить его и в массив с кодом, и в массив стэка? получается, нужно добавить аргумент?
            }

            if (numCmd == CMD_POP)
            {
                int lastStackNum   = Pop(stk);
                int numInRegister  = register[count_reg];
                ram[numInRegister] = lastStackNum; //проверить, доступен ли вообще такой индекс
            }
        }
    
        else if (reg == 0 && num != -1) //ситуация вида push [5] и pop [5]
        {
            if (numCmd == CMD_PUSH)
            {  
                int numInRAM = ram[num];
                Push (stk, numInRAM);
            }
            
            if (numCmd == CMD_POP)  //if менять на else
            {
                int lastStackNum = Pop(stk);
                ram[num]         = lastStackNum;
            }
        }
        else //ситуация вида push [5 + rax] или pop [5 + rax] или [rax + 5]
        {
            if      (my_strcmp (reg, "rax") == 0) int count_reg = RAX;
            else if (my_strcmp (reg, "rbx") == 0) int count_reg = RBX;
            else if (my_strcmp (reg, "rcx") == 0) int count_reg = RCX;
            else if (my_strcmp (reg, "rdx") == 0) int count_reg = RDX;
            else                                  MY_ASSERT (1, "The case is specified incorrectly");

            if (numCmd == CMD_PUSH)
            {
                int numInRegister = register[count_reg];
                int numInRAM      = ram[numInRegister + num];
                Push (stk, numInRAM);
            }

            if (numCmd == CMD_POP)
            {
                int lastStackNum   = Pop(stk);
                int numInRegister  = register[count_reg];
                ram[numInRegister + num] = lastStackNum;
            }
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