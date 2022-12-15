#include "asm.h"

const int SIGNATURE = 3;
const int VERSION = 1;

const int NUM_TAGS = 20;
const int N_FUNC_WITHOUT_ARG = 40;

const int NO_TAGS = -2;

enum {
    NO = 0,
    YES = 1
};

static void scanTag (char * src, char * dst, int lengthSrc);
static int findFreePlace (tag_t * tags, int sizeArrTags);
static int findTag (tag_t * tags, char * argument);

void createBinFile (char ** arrStrs, code_t * prog, char * nameBinFile)
{
    MY_ASSERT (arrStrs == nullptr, "There is no access to array of strings");
    MY_ASSERT (prog == nullptr, "There is no access to struct with information about code file");

    for (int i = 0; i < prog->nStrs; i++)
    {
        printf ("arrStrs[%d] = %s\n", i, arrStrs[i]);
    }

    FILE * binFile = fopen (nameBinFile, "wb");
    MY_ASSERT (binFile == nullptr, "There is no access to bin file");
    setbuf (binFile, NULL);
    // pushSignature (binFile, *prog);

    tag_t * tags = (tag_t *) calloc (NUM_TAGS, sizeof(tag_t));
    MY_ASSERT (tags == nullptr, "Unable to allocate new memory");

    int * code = (int *) calloc (prog->nStrs * 3, sizeof(int)); //выделяем для int, а указаетль как char
    int * tmp = code;

    int isTag = 0;
    int tagCallOrJmps = 0;
    int tagJmp = 0;

    freeCall_t * tagCallsOrJmps = (freeCall_t *) calloc (N_FUNC_WITHOUT_ARG, sizeof(int));
    MY_ASSERT (tagCallsOrJmps == nullptr, "Unable to allocate new memory");

    #define DEF_CMD(nameCmd, countLetters, numCmd, isArg)                       \
        if (myStrcmp (cmd, #nameCmd) == 0)                                      \
        {                                                                       \
            printf (">in DEF_CMD: cmd = %s, #nameCmd = %s\n",cmd, #nameCmd);    \
            if (isArg)                                                          \
            {                                                                   \
                countArgs++;                                                    \
                getArg (&code, arrStrs[i], countLetters, numCmd, tags);         \
                code++;                                                         \
            }                                                                   \
            else                                                                \
            {                                                                   \
                *code = CMD_##nameCmd;                                          \
                printf (">in DEF_CMD:  *code = %d\n", *code);                   \
                code++;                                                         \
            }                                                                   \
        }                                                                       \
        else 

    #define CALLS_AND_JMPS(nameJmp, length)                                     \
        if (myStrcmp (cmd, #nameJmp) == 0)                                       \
        {                                                                       \
            countArgs++;                                                        \
            *code = CMD_##nameJmp;                                              \
            code++;                                                             \
                                                                                \
            skipSpace (&(arrStrs[i]), length);                                  \
                                                                                \
            int lengthTag = strlen (arrStrs[i]);                                \
                                                                                \
            char * nameTag = (char *) calloc (lengthTag, sizeof(char));         \
            scanTag (arrStrs[i], nameTag, lengthTag);                           \
                                                                                \
            tagCallOrJmps = findTag (tags, nameTag);                            \
            printf (">>> in otherForm: indexTag = %d\n", tagCallOrJmps);        \
            if (tagCallOrJmps == NO_TAGS)                                       \
            {                                                                   \
                tagCallsOrJmps->ptrToArrWithCode = code;                        \
                tagCallsOrJmps->tag = nameTag;                                  \
                code++;                                                         \
                printf ("free CALL: *(tagCallsOrJmps->ptrToArrWithCode) = %d,"  \
                " tagCallsOrJmps->ptrToCallInArrStr = %d\n", *code, i);         \
            }                                                                   \
            else                                                                \
            {                                                                   \
                printf ("filled call\n");                                       \
                *code = tagCallOrJmps;                                          \
                code++;                                                         \
                free (nameTag);                                                 \
            }                                                                   \
        }                                                                       \
        else

    char * cmd = (char *) calloc (STANDART_SIZE, sizeof(char));
    MY_ASSERT (cmd == nullptr, "Unable to allocate new memory");

    int countArgs = 0;
    for (int i = 0, ip = -1; i < prog->nStrs; i++)
    {
        printf ("\n");
        sscanf (arrStrs[i], "%s", cmd);
        printf ("arrStrs[%d] = \033[51;1m %s \033[0m \n", i, arrStrs[i]);

        if (strchr (cmd, ':') != nullptr) 
        {
            int freeTag = findFreePlace (tags, NUM_TAGS);
            MY_ASSERT (freeTag == -1, "There are no free cells in the tag array");

            int lengthSrc = strlen (cmd);
            printf ("in createBinFile in for (\033[31m call \033[0m): %d\n", lengthSrc);

            tags[freeTag].nameTag = (char *) calloc (lengthSrc, sizeof(char));
            MY_ASSERT (tags[freeTag].nameTag == nullptr, "Unable to allocate new memory");
            scanTag (cmd, tags[freeTag].nameTag, lengthSrc);

            printf ("tags[%d].nameTag = %s\n", freeTag, tags[freeTag].nameTag);
            printf ("tags[%d].ptr = %d\n", freeTag, tags[freeTag].ptr);
        }
        else
        #include "jmps.h"
        #include "cmd.h"
        {
            fprintf (stderr, "command undifined is \"%s\" (arrStrs[%d])\n", arrStrs[i], i);
            MY_ASSERT (1, "This command is not defined.\n");
        }
        printf ("\n");
    }
    
        while (tagCallsOrJmps->ptrToArrWithCode != nullptr)
        {
            tagCallOrJmps = findTag (tags, tagCallsOrJmps->tag);
            printf (">>> in otherForm: indexTag = %d\n", tagCallOrJmps);
            MY_ASSERT (tagCallOrJmps == NO_TAGS, "This tag does not exist");
            *(tagCallsOrJmps->ptrToArrWithCode) = tagCallOrJmps;
            tagCallsOrJmps++;
        }
    

    for (int i = 0; i < prog->nStrs * 3; i++)
    {
        printf ("code[%d] = %d\n", i, tmp[i]);
    }

    fwrite (tmp, sizeof(int), prog->nStrs * 3, binFile);


    printf (">> in createBinFile (RESULT): countArgs = %d\n", countArgs);
    fclose (binFile);
    free (cmd);
    #undef DEF_CMD
}

static int findFreePlace (tag_t * tags, int sizeArrTags)
{
    for (int i = 0; i < sizeArrTags; i++)
    {
        if (tags[i].ptr == 0)
        {
            return i;
        }
    }
    return -1;
}

static void scanTag (char * src, char * dst, int lengthSrc)
{
    for (int i = 0; i < lengthSrc; i++)
    {
        if (*src == ':' || *src == ';')
        {
            *dst = '\0';
            break;
        }
        if (isalnum(*src) || ispunct(*src))
        {
            *dst = *src;
            dst++;
            src++;
        }
        else
        {
            MY_ASSERT (1, "Incorrect tag: missing ":" or there are invalid characters");
        }
    }
}

void getArg (int ** code, char * str_text_code, int countLetters, int numCmd, tag_t * tags)
{
	char * firstBracket = nullptr;
    if ((firstBracket = strchr (str_text_code, '[')) != nullptr)
    {
        ram(code, firstBracket, numCmd);
    }
    else
	{
		printf ("There isn't bracket\n");
        otherForm(code, str_text_code, countLetters, numCmd, tags); //есть ли неинициал тэг
	}
}

void otherForm (int ** code, char * strCode, int countLetters, int numCmd, tag_t * tags)
{
    printf ("in otherForm: \033[32;1m numCmd \033[0m = %d\n", numCmd);

	char * reg  = (char *) calloc (3, sizeof(char));
    MY_ASSERT (reg == nullptr, "It's impossible to read the argument");

    char * trash = (char *) calloc (3, sizeof(char));
    MY_ASSERT (trash == nullptr, "func GetArgument: it's impossible to read other symbols");

	int num = -1;
    skipSpace (&strCode, countLetters);
    char * ptrToArg = strCode;

	printf ("in otherForm: \033[32;1m argument \033[0m = %c\n", *ptrToArg);

    if ((strcmp (ptrToArg, "rax") >= 0) || (strcmp (ptrToArg, "rbx") >= 0) ||
        (strcmp (ptrToArg, "rcx") >= 0) || (strcmp (ptrToArg, "rdx") >= 0))
    {
		printf ("yes \033[35;1m REG \033[0m\n");
        // sscanf (ptrToArg, "%[rabcdx]%[ +]%d", reg, trash, &num); //обработка push rax + 5
        sscanf (ptrToArg, "%[rabcdx]", reg);
        ptrToArg = ptrToArg + 3;
        skipSpace (&ptrToArg, 0);
        printf ("*ptrToArg = %c\n", *ptrToArg);
        sscanf (ptrToArg, "%[+]", trash);
        ptrToArg = ptrToArg + 1;
        skipSpace (&ptrToArg, 0);
        sscanf (ptrToArg, "%d", &num);
    }
    else
    {
		printf ("There isn't register\n");
        int nDigits = readNum (ptrToArg, &num); 
        skipSpace (&ptrToArg, nDigits);
        sscanf (ptrToArg, "%[+]", trash);
        skipSpace (&ptrToArg, 1);
        sscanf (ptrToArg, "%[rabcdx]", reg);
        
        
    }
	printf ("Your reg = %s, trash = %s, num = %d\n", reg, trash, num);
    
    MY_ASSERT (numCmd != 9 && num == -1 && *reg == 0, "Your argument in square brackets are incorrect");

    if (num == -1 && *reg != 0)        //ситуация вида push rax и pop rax или jmp
    {
		printf ("reg != 0 RRRRRRRRRRRRR\n");
        char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else    MY_ASSERT (1, "The case is specified incorrectly");


        if (numCmd == CMD_PUSH) //0100 | 0001 push rax
        {                       
            **code = 65;
            (*code)++;
            **code = count_reg;
        }

        else if (numCmd == CMD_POP) //0100 | 0010   pop rax
        {
            **code = 66;
            (*code)++;
            **code = count_reg;
        }
    }

    else if (*reg == 0 && num != -1) //ситуация вида push 5 и pop 5
    {
		printf ("this if\n");
        if (numCmd == CMD_PUSH) //0010 | 0001
        {  
			printf ("command is push 111\n");
            **code = 33;
            (*code)++;
            **code = num;
        }
            
        else if (numCmd == CMD_POP)  //if менять на else //0010 | 0010
        {
			printf ("command is pop 222\n");
            **code = 34;
            (*code)++;
            **code = num;
        }
        else //это jmp     //0010 | 1000
		{
			printf ("command is jmp 333\n");
			**code = 40;
            (*code)++;
            **code = num;
		}
            
    }
    else //ситуация вида push 5 + rax, или pop 5 + rax, или rax + 5
    {
        char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else                                  MY_ASSERT (1, "The case is specified incorrectly");

        if (numCmd == CMD_PUSH) //0110 | 0001    push 5 + rax
        {
            **code = 97;
            (*code)++;
            **code = count_reg;
            (*code)++;
            **code = num;
        }

        else if (numCmd == CMD_POP) //0110 | 0010    pop 5 + rax
        {
            **code = 98;
            (*code)++;
            **code = count_reg;
            (*code)++;
            **code = num;
        }
        else //0110 | 1000                 rax + 5
        {
            **code = 104;
            (*code)++;
            **code = count_reg;
            (*code)++;
            **code = num;
        }
    }
    free (reg);
    free (trash);
}

void ram (int ** code, char * firstBracket, int numCmd)
{ 
	printf ("Works ram func\n");
	printf ("numCmd = %d\n", numCmd);

    char * reg  = (char *) calloc (3, sizeof(char));
    MY_ASSERT (reg == nullptr, "It's impossible to read the argument");
    char * trash = (char *) calloc (8, sizeof(char)); 
    MY_ASSERT (trash == nullptr, "func GetArgument: it's impossible to read other symbols");

	int num = -1;
    firstBracket++; //сдвинулись со скобки
    if (strcmp (firstBracket, "r") >= 0)
    {
        sscanf (firstBracket, "%[rabcdx]", reg);  //обработка push [rax + 5]
        firstBracket = firstBracket + 3;
        skipSpace (&firstBracket, 0);
        sscanf (firstBracket, "%[+]", trash);
        firstBracket = firstBracket + 1;
        skipSpace (&firstBracket, 0);
        sscanf (firstBracket, "%d", &num);
    }
    else 
    {
        int nDigit = readNum (firstBracket, &num);//обработка push [5 + rax]
        firstBracket = firstBracket + nDigit;
        skipSpace (&firstBracket, 0);      
        sscanf (firstBracket, "%[+]", trash);
        firstBracket = firstBracket + 1;
        skipSpace (&firstBracket, 0);
        sscanf (firstBracket, "%[rabcdx]", reg);
    }

    printf ("num = %d, trash = %s, reg = %s\n", num, trash, reg);

    MY_ASSERT (num == -1 && *reg == 0, "Your argument in square brackets are incorrect");

    if (num == -1 && *reg != 0)     //ситуация вида push [rax] и pop [rax]
    {
        char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else                                  MY_ASSERT (1, "The case is specified incorrectly");


        if (numCmd == CMD_PUSH) //0000 | 0000
        {                       //1100 | 0001
			printf ("There is push 222\n");
            **code = 193;
            (*code)++;
            **code = count_reg;
        }

        if (numCmd == CMD_POP) //1100 | 0010
        {
			printf ("There is pop 222\n");
            **code = 194;
            printf ("**code = %d\n", **code);
            (*code)++;
            **code = count_reg;
            printf ("**code = %d\n", **code);
        }
    }
    
    else if (*reg == 0 && num != -1) //ситуация вида push [5] и pop [5]
    {
        if (numCmd == CMD_PUSH) //1010 | 0001      push [5] 
        {
			printf ("There is push 333\n");

            **code = 161;
            (*code)++;
            **code = num;
        }
            
        if (numCmd == CMD_POP)  //if менять на else //1010 | 0010 pop [5]
        {
			printf ("There is pop 333\n");
            **code = 162;
            (*code)++;
            **code = num;
        }
    }
    else //ситуация вида push [5 + rax] или pop [5 + rax] или [rax + 5]
    {
		char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else                                  MY_ASSERT (1, "The case is specified incorrectly");

        if (numCmd == CMD_PUSH) //1110 | 0001  push [5 + rax]
        {
            **code = 225;
            (*code)++;
            **code = count_reg;
            (*code)++;
            **code = num;
        }

        if (numCmd == CMD_POP) //1110 | 0010  pop [5 + rax]
        {
			printf ("There is pop 444\n");
            **code = 226;
            (*code)++;
            **code = count_reg;
            (*code)++;
            **code = num;
        }
    }
    free (reg);
    free (trash);
}

void pushSignature (FILE * dst, code_t codeFile)
{
	MY_ASSERT (dst == nullptr, "null pointer to file");
	int * signature = (int *) calloc (SIGNATURE, sizeof(int));
	signature[0] = 'ASM';
	signature[1] = VERSION;
	signature[2] = codeFile.nStrs;
	
	fwrite (signature, sizeof(int), 3, dst);
}

//---------------------------------string functions--------------------------------------------------
void skipSpace (char ** strCode, int countLetters)
{
    *strCode = *strCode + countLetters;
	int i = 0;
    for (; **strCode == ' ' || **strCode == '\t'; i++)
    {
        (*strCode)++;
    }
}

static int findTag (tag_t * tags, char * argument)
{
    MY_ASSERT (tags == nullptr, "There is no access to array with tags");
    for (int i = 0; i < NUM_TAGS; i++)
    {
        printf (">>>in findTag: argument [%d/%d] = %s\n", i, NUM_TAGS, argument);
        if (tags[i].nameTag != nullptr && myStrcmp (argument, tags[i].nameTag) >= 0) //==?
        {
            printf (">>>hyi Landau\n");
            return tags[i].ptr;
        }
    }
    return NO_TAGS;
}

int readNum (char * src, int * dst)
{
    MY_ASSERT (src == nullptr, "There is no access to source string");
    MY_ASSERT (dst == nullptr, "There is no access to destination string");
    char sym = 0;
    char * tmp = src;

    while (48 <= *tmp && *tmp <= 57)
    {
        tmp++;
        sym++;
    }
    *dst = atoi(src);
    return sym;
}

int myStrcmp (const char * string1, const char * string2)
{
	int i = 0, j = 0;
	for (; string1[i] != '\0' && string2[j] != '\0'; i++, j++)
	{
		while (!isalpha(string1[i]) && string1[i] != '\0')
			i++;
		while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;
		if (tolower(string1[i]) == tolower(string2[j]))
			continue;
		return (tolower(string1[i]) - tolower(string2[j]));
	}

	while (!isalpha(string1[i]) && string1[i] != '\0')
			i++;

	while (!isalpha(string2[j]) && string2[j] != '\0')
			j++;

	return (tolower(string1[i]) - tolower(string2[j]));
}
