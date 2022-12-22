#include "asm.h"

//TODO: дописать генерацию команд
//очистить память
//вернуть листинг
//добавить все флаги, обработать warnings

int RAM = 1 << 7; //0000 | 0001  -> 1000 | 0000
int REG = 1 << 6; //0000 | 0001  -> 0100 | 0000
int NUM = 1 << 5; //0000 | 0001  -> 0010 | 0000

const int SIGNATURE_SIZE = 4;

int N_FUNC_WITHOUT_ARG = 0;

const int NO_TAGS = -2;

enum {
    NO = 0,
    YES = 1
};

static void scanTag (char * src, char * dst, int lengthSrc);
static int findFreePlace (tag_t * tags, int sizeArrTags);
static long int findTag (tag_t * tags, char * argument, int * startArrWithCode, int numTags);
static void ram (int ** code, char * firstBracket, int numCmd);
static void no_ram (int ** code, char * strCode, int countLetters, int numCmd, tag_t * tags);
static void getArg (int ** code, char * str_text_code, int countLetters, int numCmd, tag_t * tags);

void dumpCode (char ** arrStrs, int * code, int numElem, int nStrs)
{
    MY_ASSERT (code == nullptr, "There is no access to this code");
    MY_ASSERT (arrStrs == nullptr, "There is no access to array with strings");

    FILE * logfile = fopen ("logCode.txt", "a");
    fprintf (logfile, "\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    for (int i = 0; i < numElem; i++)
    {
        fprintf (logfile, "code[%d] = %d\n", i, code[i]);
    }

    fprintf (logfile, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
}

void createBinFile (char ** arrStrs, code_t * prog, char * nameBinFile, int numTags)
{
    MY_ASSERT (arrStrs == nullptr, "There is no access to array of strings");
    MY_ASSERT (prog == nullptr, "There is no access to struct with information about code file");

    FILE * binFile = fopen (nameBinFile, "a+b");
    MY_ASSERT (binFile == nullptr, "There is no access to bin file");
    setbuf (binFile, NULL);

    tag_t * tags = (tag_t *) calloc (numTags, sizeof(tag_t));
    MY_ASSERT (tags == nullptr, "Unable to allocate new memory");

    int * code = (int *) calloc (prog->nStrs * 3, sizeof(int));
    int * tmp = code;

    int isTag = 0;
    long int tagCallOrJmps = 0;
    int tagJmp = 0;

    freeCall_t * freeFunc = (freeCall_t *) calloc (numTags, sizeof(freeCall_t));
    MY_ASSERT (freeFunc == nullptr, "Unable to allocate new memory");
    freeCall_t * saveTagCallsOrJumps = freeFunc;

    #define DEF_CMD(nameCmd, countLetters, numCmd, isArg)                       \
        if (myStrcmp (cmd, #nameCmd) == 0)                                      \
        {                                                                       \
            if (isArg)                                                          \
            {                                                                   \
                countArgs++;                                                    \
                getArg (&code, arrStrs[i], countLetters, numCmd, tags);         \
                code++;                                                         \
            }                                                                   \
            else                                                                \
            {                                                                   \
                *code = CMD_##nameCmd;                                          \
                code++;                                                         \
            }                                                                   \
        }                                                                       \
        else 

    #define CALLS_AND_JMPS(nameJmp, length)                                     \
        if (myStrcmp (cmd, #nameJmp) == 0)                                      \
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
            tagCallOrJmps = findTag (tags, nameTag, tmp, numTags);              \
            if (tagCallOrJmps == NO_TAGS)                                       \
            {                                                                   \
                freeFunc->ptrToArrWithCode = code;                              \
                freeFunc->tag = nameTag;                                        \
                N_FUNC_WITHOUT_ARG++;                                           \
                code++;                                                         \
                freeFunc++;                                                     \
            }                                                                   \
            else                                                                \
            {                                                                   \
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
        sscanf (arrStrs[i], "%s", cmd);

        if (strchr (cmd, ':') != nullptr) 
        {
            int freeTag = findFreePlace (tags, numTags);
            MY_ASSERT (freeTag == -1, "There are no free cells in the tag array");

            int lengthSrc = strlen (cmd);

            tags[freeTag].nameTag = (char *) calloc (lengthSrc, sizeof(char));
            MY_ASSERT (tags[freeTag].nameTag == nullptr, "Unable to allocate new memory");
            scanTag (cmd, tags[freeTag].nameTag, lengthSrc);

            tags[freeTag].ptr = code;
        }
        else
        #include "jmps.h"
        #include "cmd.h"
        {
            fprintf (stderr, "command undifined is \"%s\" (arrStrs[%d])\n", arrStrs[i], i);
            MY_ASSERT (1, "This command is not defined.\n");
        }
    }
    for (int i = 0; i < N_FUNC_WITHOUT_ARG; i++)
    {
        tagCallOrJmps = findTag (tags, saveTagCallsOrJumps->tag, tmp, numTags);
        MY_ASSERT (tagCallOrJmps == NO_TAGS, "This tag does not exist");
        *(saveTagCallsOrJumps->ptrToArrWithCode) = tagCallOrJmps;
        saveTagCallsOrJumps++;
    }

    dumpCode (arrStrs, tmp, prog->nStrs * 3, prog->nStrs);

    fwrite (tmp, sizeof(int), prog->nStrs * 3, binFile);

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
            MY_ASSERT (1, "Incorrect tag: missing \":\" or there are invalid characters");
        }
    }
}

static void getArg (int ** code, char * str_text_code, int countLetters, int numCmd, tag_t * tags)
{
	char * firstBracket = nullptr;
    if ((firstBracket = strchr (str_text_code, '[')) != nullptr)
    {
        ram(code, firstBracket, numCmd);
    }
    else
	{
        no_ram(code, str_text_code, countLetters, numCmd, tags);
	}
}

static void no_ram (int ** code, char * strCode, int countLetters, int numCmd, tag_t * tags)
{
    skipSpace (&strCode, countLetters);
    char * ptrToArg = strCode;

    char * reg  = (char *) calloc (4, sizeof(char));
    MY_ASSERT (reg == nullptr, "It's impossible to read the argument");

    char * ptrToReg = nullptr;

    char * placeOfPlus = nullptr;

    if (strchr(ptrToArg, '-') != nullptr)
    {
        MY_ASSERT (numCmd != CMD_PUSH, "the minus can only be present in the \"push\" command"); 
        **code = **code | NUM;
        **code = **code | CMD_PUSH;
        (*code)++;
        **code = atoi(strCode);
    }
    else if ((ptrToReg = strchr(ptrToArg, 'r')) != nullptr)
    {
        reg = (char *) memcpy (reg, ptrToReg, 3 * sizeof(char));
        *(reg+3) = '\0';

        char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else    MY_ASSERT (1, "The case is specified incorrectly");

        if ((placeOfPlus = strchr(ptrToArg, '+')) != nullptr)
        {
            placeOfPlus++;
            if (numCmd == CMD_PUSH) //0110 | 0001
            {
                **code = **code | REG;
                **code = **code | NUM;
                **code = **code | CMD_PUSH;
                (*code)++;
            }
            else if (numCmd == CMD_POP) //0110 | 0010
            {
                **code = **code | REG;
                **code = **code | NUM;
                **code = **code | CMD_POP;
                (*code)++;
            }   
            else
            {
                MY_ASSERT (1, "Incorrect expression with \"+\"");
            }

            skipSpace(&placeOfPlus, 0);
            int num = 0;
            int nSymbols = readNum (ptrToArg, &num);
            if (nSymbols == 0)
            {
                readNum (placeOfPlus, &num);
                **code = count_reg;
                (*code)++;
                **code = num;
            }
            else
            {
                **code = count_reg;
                (*code)++;
                **code = num;
            }
        }
        else
        {
            if (numCmd == CMD_PUSH)
            {
                **code = **code | REG;
                **code = **code | CMD_PUSH;
                (*code)++;
                **code = count_reg;
            }
            else if (numCmd == CMD_POP)
            {
                **code = **code | REG;
                **code = **code | CMD_POP;
                (*code)++;
                **code = count_reg;
            }
            else
            {
                MY_ASSERT (1, "Incorrect expression with a case without \"+\"");
            }
        }
    }
    else if (numCmd == CMD_PUSH)
    {
        int num = 0;
        int nSymbols = readNum (ptrToArg, &num);
        MY_ASSERT (nSymbols == 0, "Push without a numeric argument\n");
        **code = **code | NUM;
        **code = **code | CMD_PUSH;
        (*code)++;
        **code = num;
    }
    else if (numCmd == CMD_POP)
    {
        int num = 0;
        int nSymbols = readNum (ptrToArg, &num);
        MY_ASSERT (nSymbols != 0, "Pop with a numeric argument\n");
        **code = **code | NUM;
        **code = **code | CMD_POP;
        (*code)++;
        **code = num;
    }
    else
    {
        MY_ASSERT (1, "Incorrect push or pop");
    }
    free (reg);
}

static void ram (int ** code, char * firstBracket, int numCmd)
{
    char * reg  = (char *) calloc (3, sizeof(char));
    MY_ASSERT (reg == nullptr, "It's impossible to read the argument");
    char * trash = (char *) calloc (8, sizeof(char)); 
    MY_ASSERT (trash == nullptr, "func GetArgument: it's impossible to read other symbols");

	int num = -1;
    firstBracket++;
    if (strcmp (firstBracket, "r") >= 0)
    {
        sscanf (firstBracket, "%[rabcdx]", reg);
        firstBracket = firstBracket + 3;
        skipSpace (&firstBracket, 0);
        sscanf (firstBracket, "%[+]", trash);
        firstBracket = firstBracket + 1;
        skipSpace (&firstBracket, 0);
        sscanf (firstBracket, "%d", &num);
    }
    else 
    {
        int nDigit = readNum (firstBracket, &num);
        firstBracket = firstBracket + nDigit;
        skipSpace (&firstBracket, 0);      
        sscanf (firstBracket, "%[+]", trash);
        firstBracket = firstBracket + 1;
        skipSpace (&firstBracket, 0);
        sscanf (firstBracket, "%[rabcdx]", reg);
    }

    MY_ASSERT (num == -1 && *reg == 0, "Your argument in square brackets are incorrect");

    if (num == -1 && *reg != 0)
    {
        char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else    MY_ASSERT (1, "The case is specified incorrectly");


        if (numCmd == CMD_PUSH) 
        {
            **code = **code | RAM;
            **code = **code | REG;
            **code = **code | CMD_PUSH;
            (*code)++;
            **code = count_reg;
        }

        if (numCmd == CMD_POP)
        {
            **code = **code | RAM;
            **code = **code | REG;
            **code = **code | CMD_POP;
            (*code)++;
            **code = count_reg;
        }
    }
    
    else if (*reg == 0 && num != -1) //push [5]
    {
        if (numCmd == CMD_PUSH) 
        {
            **code = **code | RAM;
            **code = **code | NUM;
            **code = **code | CMD_PUSH;
            (*code)++;
            **code = num;
        }
            
        if (numCmd == CMD_POP) 
        {
            **code = **code | RAM;
            **code = **code | NUM;
            **code = **code | CMD_POP;
            (*code)++;
            **code = num;
        }
    }
    else 
    {
		char count_reg = 0;
        if      (strcmp (reg, "rax") == 0) count_reg = (char) RAX;
        else if (strcmp (reg, "rbx") == 0) count_reg = (char) RBX;
        else if (strcmp (reg, "rcx") == 0) count_reg = (char) RCX;
        else if (strcmp (reg, "rdx") == 0) count_reg = (char) RDX;
        else    MY_ASSERT (1, "The case is specified incorrectly");

        if (numCmd == CMD_PUSH)
        {
            **code = **code | REG;
            **code = **code | RAM;
            **code = **code | NUM;
            **code = **code | CMD_PUSH;
            (*code)++;
            **code = count_reg;
            (*code)++;
            **code = num;
        }

        if (numCmd == CMD_POP)
        {
            **code = **code | REG;
            **code = **code | RAM;
            **code = **code | NUM;
            **code = **code | CMD_POP;
            (*code)++;
            **code = count_reg;
            (*code)++;
            **code = num;
        }
    }
    free (reg);
    free (trash);
}

void pushSignature (char * nameBinFile, code_t codeFile)
{
    FILE * dst = fopen (nameBinFile, "w");
	MY_ASSERT (dst == nullptr, "null pointer to file");

    char * nameProg = (char *) calloc (SIGNATURE_SIZE, sizeof(char));
    char * signature = (char *) calloc (1, SIGNATURE_SIZE*sizeof(char) + sizeof(int));
    char * savePtrName = nameProg;
    char * savePtrSignature = signature;
    char version = 1;
	*nameProg = 'A';
    nameProg++;
    *nameProg = 'S';
    nameProg++;
    *nameProg = 'M';
    nameProg++;
    *nameProg = version; 

    memcpy (signature, savePtrName, SIGNATURE_SIZE*sizeof(char));
    signature = signature + SIGNATURE_SIZE*sizeof(char);

    int nStrings = (int) codeFile.nStrs;
    printf (">> pushSignature: nStrings = %d\n", nStrings);
    memcpy (signature, &nStrings, sizeof(int));

	fwrite (savePtrSignature, SIGNATURE_SIZE*sizeof(char) + sizeof(int), 1, dst);

    free (savePtrName);
    free (savePtrSignature);
    fclose (dst); //при новом открытии где будет поток файла?
}

static long int findTag (tag_t * tags, char * argument, int * startTagWithCode, int numTags)
{
    MY_ASSERT (tags == nullptr, "There is no access to array with tags");
    for (int i = 0; i < numTags; i++)
    {
        if (tags[i].nameTag != nullptr && myStrcmp (argument, tags[i].nameTag) == 0)
        {
            return tags[i].ptr - startTagWithCode;
        }
    }
    return NO_TAGS;
}

