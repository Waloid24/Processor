#include "cpu.h"

#define JUMP_FORM(sign)                             \
    i++;                                            \
    int n1 = stack_pop (&stk, logfile);             \
    int n2 = stack_pop (&stk, logfile);             \
    if (n1 sign n2)                                 \
    {                                               \
        int ptrToJmp = code[i];                  \
        i = ptrToJmp-1;                             \
        stack_push (&stk, n2, logfile);             \
        stack_push (&stk, n1, logfile);             \
    }                                               \
    else                                            \
    {                                               \
        stack_push (&stk, n2, logfile);             \
        stack_push (&stk, n1, logfile);             \
    }                   

const int NUM_REGISTERS = 4;
const int MAX_RAM = 100;

static int getNum (void);
static int * createArrRegs (size_t numRegs);
static int * createArrCode (code_t fileInfo);


FILE * codeFile (char * nameFile)
{
    MY_ASSERT (nameFile == nullptr, "Unable to access the file");
    FILE * binFile = fopen (nameFile, "rb");
    MY_ASSERT (binFile == nullptr, "Unable to open binary file");

    return binFile;
}

void cpu (char * nameFile, code_t fileInfo, int numTags)
{
    FILE * binFile = codeFile (nameFile);
    FILE * logfile = open_logfile ("stack_log.txt");
    FILE * logCallStack = open_logfile ("callStack.txt");

    setbuf (logfile, NULL);
    stack_t stk = {};

    stack_t callStack = {};
    stk_ctor (&callStack, numTags, "callStk");

    int ram[MAX_RAM] = {};

    stk_ctor (&stk, fileInfo.nStrs * 3, "cpu");

    int * code = createArrCode (fileInfo);
    fread (code, sizeof(int), fileInfo.nStrs * 3, binFile);

    int * regs = createArrRegs (NUM_REGISTERS);

    for (int i = 0; i < fileInfo.nStrs * 3; i++)
    {
        if (code[i] == 17) //hlt
        {
            stack_dtor (&stk);
            break;
        }
        else if (code[i] == 33) //push 7
        {
            i++;
            stack_push (&stk, code[i], logfile);
            stack_dump (stk, logfile);
            
        }
        else if (code[i] == 34) //pop 
        {
            i++;
            stack_pop (&stk, logfile);
        }
        else if (code[i] == 65) //push rax
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            stack_push (&stk, regs[nReg], logfile);
        }
        else if (code[i] == 66) //pop rax
        {     
            i++;
            int nReg = code[i];    
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");    
            regs[nReg] = stack_pop (&stk, logfile);
        }
        else if (code[i] == 97) //push 5 + rax
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            i++;    
            int tmp = regs[nReg] + code[i];
            stack_push (&stk, tmp, logfile);
        }
        else if (code[i] == 161) //push[10]
        {
            i++;
            int ramIndex = code[i];
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            stack_push (&stk, ram[ramIndex], logfile);
        }
        else if (code[i] == 162) // pop[10]
        {
            i++;
            int ramIndex = code[i];
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            ram[ramIndex] = stack_pop (&stk, logfile);
        }
        else if (code[i] == 193) //push[rcx]
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            int ramIndex = regs[nReg];
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            stack_push (&stk, ram[ramIndex], logfile);
        }
        else if (code[i] == 194) //pop[rcx]
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            int ramIndex = regs[nReg];
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            ram[ramIndex] = stack_pop (&stk, logfile);
        }
        else if (code[i] == 225)  //push [5 + rcx]
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            i++;
            int value1 = regs[nReg];
            int value2 = code[i];
            int ramIndex = value1 + value2;
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            stack_push (&stk, ram[ramIndex], logfile);
        }
        else if (code[i] == 226) //pop [5 + rcx]
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            i++;
            int value1 = regs[nReg];
            int value2 = code[i];
            int ramIndex = value1 + value2;
            MY_ASSERT (ramIndex > MAX_RAM-1, "You are out of RAM");
            ram[ramIndex] = stack_pop (&stk, logfile);
        }
        
        else if (code[i] == 3) //add
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 + n2, logfile);
        }
        else if (code[i] == 4) //sub
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 - n2, logfile);
        }
        else if (code[i] == 5) //mul
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 * n2, logfile);
        }
        else if (code[i] == 6) //div
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 / n2, logfile);
        }
        else if (code[i] == 7) //out
        {
            fprintf (stdout, "OUT: %d\n", stack_pop (&stk, logfile));
        }
        else if (code[i] == 8) // in
        {
            int tmp = getNum ();
            stack_push (&stk, tmp, logfile);
            stack_dump (stk, logfile);
        }
        else if (code[i] == 9) //jmp
        {
            i++;
            int ptrToJmp = code[i];
            i = ptrToJmp-1;
        }
        else if (code[i] == 12) //ja  
        {
            JUMP_FORM (>)
        }
        else if (code[i] == 13) //jb
        {
            JUMP_FORM(<)
        }
        else if (code[i] == 14) //jbe
        {
            JUMP_FORM(<=)
        }
        else if (code[i] == 15) //jge
        {
            JUMP_FORM (>=)
        }
        else if (code[i] == 16) //je
        {
            JUMP_FORM (==)
        }
        else if (code[i] == 10) //call
        {
            i++;
            int ptrToJmp = code[i];
            stack_push (&callStack, i+1, logCallStack);
            i = ptrToJmp-1;
        }
        else if (code[i] == 11) //RET
        {
            i = stack_pop (&callStack, logCallStack) - 1;
        }
        else if (code[i] == 18)
        {
            int tmp = stack_pop (&stk, logfile);
            int result = (int) sqrt ((double) tmp);
            stack_push (&stk, result, logfile);
        }
        else 
        {
            stack_dtor (&stk);
            break;
        }
    }

    free (regs);
    free (code);
    fclose(logfile);
}

static int getNum (void)
{
    int degreeOfNum = 0;
    int enterSymbols = scanf (" %d", &degreeOfNum);
    while (getchar() != '\n');

    if (enterSymbols == 0)
    {
        printf ("Please, enter a number\n");
        return getNum ();
    }

    return degreeOfNum;
}

static int * createArrRegs (size_t numRegs)
{
    int * regs = (int *) calloc (numRegs, sizeof(int));
    MY_ASSERT (regs == nullptr, "Unable to allocate new memory");

    return regs;
}

static int * createArrCode (code_t fileInfo)
{
    int * code = (int *) calloc (fileInfo.nStrs * 3, sizeof (int));
    MY_ASSERT (code == nullptr, "Unable to allocate new memory");

    return code;
}
