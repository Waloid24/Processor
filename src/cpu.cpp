#include "cpu.h"

//умножать на маску 11111, которая соответствует первым пяти битам, а по ней, где правильно выставятся биты, смотреть на номер команды

#define JUMP_FORM(sign)                             \
    i++;                                            \
    int n1 = stack_pop (&stk, logfile);             \
    int n2 = stack_pop (&stk, logfile);             \
    if (n1 sign n2)                                 \
    {                                               \
        int ptrToJmp = code[i];                     \
        i = ((size_t) ptrToJmp) - 1;                \
        stack_push (&stk, n2, logfile);             \
        stack_push (&stk, n1, logfile);             \
    }                                               \
    else                                            \
    {                                               \
        stack_push (&stk, n2, logfile);             \
        stack_push (&stk, n1, logfile);             \
    }                   

const int NUM_REGISTERS = 4;
const int SIGNATURE_SIZE = 4;

int RAM = 7; //0000 | 0001  -> 1000 | 0000
int REG = 6; //0000 | 0001  -> 0100 | 0000
int NUM = 5; //0000 | 0001  -> 0010 | 0000

static int getNum (void);
static int * createArrRegs (size_t numRegs);
static int * createArrCode (size_t nStrs);
static int checkBit(const int value, const int position);

FILE * codeFile (char * nameFile)
{
    MY_ASSERT (nameFile == nullptr, "Unable to access the file");
    FILE * binFile = fopen (nameFile, "rb");
    MY_ASSERT (binFile == nullptr, "Unable to open binary file");

    return binFile;
}

size_t readNumStrs (FILE * binFile)
{
    MY_ASSERT (binFile == nullptr, "There is no access to binary file");

    fseek (binFile, SIGNATURE_SIZE*sizeof(char), SEEK_SET);

    int nStrs = -1;
    fread (&nStrs, sizeof(int), 1, binFile);
    printf ("in getCode: nStrs = %d\n", nStrs);
    MY_ASSERT (nStrs < 0, "The file is empty");

    return (size_t)nStrs;
}

void dumpCode (int * code, size_t numElem)
{
    FILE * logfile = fopen ("logCodeCPU.txt", "a");
    MY_ASSERT (code == nullptr, "There is no access to this code");

    fprintf (logfile, "\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    for (size_t i = 0; i < numElem; i++)
    {
        fprintf (logfile, "code[%zu] = %d\n", i, code[i]);
    }

    fprintf (logfile, "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    fclose (logfile);
}

int * getCode (size_t nStrs, FILE * binFile)
{
    int * code = createArrCode (nStrs);
    fread (code, sizeof(int), nStrs * 3, binFile);

    return code;
}

void cpu (int * code, size_t nStrs, size_t numTags)
{
    MY_ASSERT (code == nullptr, "Unable to access to array with code");

    FILE * logfile = open_logfile ("stack_log.txt");
    FILE * logCallStack = open_logfile ("callStack.txt");

    dumpCode (code, nStrs*3);

    // setbuf (logfile, NULL);

    stack_t callStack = {};
    stk_ctor (&callStack, numTags, "callStk");

    int ram[MAX_RAM] = {};

    stack_t stk = {};
    stk_ctor (&stk, nStrs * 3, "cpu");

    int * regs = createArrRegs (NUM_REGISTERS);
    int cmd = -1;
    for (size_t i = 0; i < nStrs * 3; i++)
    {
        cmd = (code[i] & MASK);
        // printf ("code[i] & NUM = %d\n", code[i] & NUM);
        // printf ("code[i] & REG = %d\n", code[i] & REG);
        // printf ("code[i] & RAM = %d\n", code[i] & RAM);

        if (code[i] == CMD_MEOW)
        {
            fprintf (stdout, "meow\n");
        }
        else if (code[i] == CMD_HLT) //hlt
        {
            stack_dtor (&stk);
            stack_dtor (&callStack);
            break;
        }
        else if ((cmd == CMD_PUSH) && 
                (checkBit(code[i], NUM) == 1) && 
                (checkBit(code[i], REG) == 0) && 
                (checkBit(code[i], RAM) == 0)) //push 7 == 0010|0001
        {
            i++;
            stack_push (&stk, code[i], logfile);
            stack_dump (stk, logfile);
            
        }
        else if ((cmd == CMD_POP) && 
                (checkBit(code[i], NUM) == 1) && 
                (checkBit(code[i], REG) == 0) && 
                (checkBit(code[i], RAM) == 0)) //pop 
        {
            i++;
            stack_pop (&stk, logfile);
        }
        else if ((cmd == CMD_PUSH) && 
                (checkBit(code[i], NUM) == 0) && 
                (checkBit(code[i], REG) == 1) && 
                (checkBit(code[i], RAM) == 0)) //push rax
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            stack_push (&stk, regs[nReg], logfile);
        }
        else if ((cmd == CMD_POP) && 
                (checkBit(code[i], NUM) == 0) && 
                (checkBit(code[i], REG) == 1) && 
                (checkBit(code[i], RAM) == 0)) //pop rax
        {     
            i++;
            int nReg = code[i];    
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");    
            regs[nReg] = stack_pop (&stk, logfile);
        }
        else if ((cmd == CMD_PUSH) && 
                (checkBit(code[i], NUM) == 1) && 
                (checkBit(code[i], REG) == 1) && 
                (checkBit(code[i], RAM) == 0)) //push 5 + rax
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            i++;    
            int tmp = regs[nReg] + code[i];
            stack_push (&stk, tmp, logfile);
        }
        else if ((cmd == CMD_PUSH) && 
                (checkBit(code[i], NUM) == 1) && 
                (checkBit(code[i], REG) == 0) && 
                (checkBit(code[i], RAM) == 1)) //push[10]
        {
            i++;
            int ramIndex = code[i];
            MY_ASSERT ((size_t) ramIndex > MAX_RAM-1, "You are out of RAM");
            stack_push (&stk, ram[ramIndex], logfile);
        }
        else if ((cmd == CMD_POP) && 
                (checkBit(code[i], NUM) == 1) && 
                (checkBit(code[i], REG) == 0) && 
                (checkBit(code[i], RAM) == 1)) // pop[10]
        {
            i++;
            int ramIndex = code[i];
            MY_ASSERT ((size_t) ramIndex > MAX_RAM-1, "You are out of RAM");
            ram[ramIndex] = stack_pop (&stk, logfile);
        }
        else if ((cmd == CMD_PUSH) && 
                (checkBit(code[i], NUM) == 0) && 
                (checkBit(code[i], REG) == 1) && 
                (checkBit(code[i], RAM) == 1)) //push[rcx]
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            int ramIndex = regs[nReg];
            MY_ASSERT ((size_t) ramIndex > MAX_RAM-1, "You are out of RAM");
            stack_push (&stk, ram[ramIndex], logfile);
        }
        else if ((cmd == CMD_POP) && 
                (checkBit(code[i], NUM) == 0) && 
                (checkBit(code[i], REG) == 1) && 
                (checkBit(code[i], RAM) == 1)) //pop[rcx]
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            int ramIndex = regs[nReg];
            MY_ASSERT ((size_t) ramIndex > MAX_RAM-1, "You are out of RAM");
            ram[ramIndex] = stack_pop (&stk, logfile);
        }
        else if ((cmd == CMD_PUSH) && 
                (checkBit(code[i], NUM) == 1) && 
                (checkBit(code[i], REG) == 1) && 
                (checkBit(code[i], RAM) == 1))  //push [5 + rcx]
        {
            i++;
            int nReg = code[i];
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            i++;
            int value1 = regs[nReg];
            int value2 = code[i];
            int ramIndex = value1 + value2;
            MY_ASSERT ((size_t) ramIndex > MAX_RAM-1, "You are out of RAM");
            stack_push (&stk, ram[ramIndex], logfile);
        }
        else if ((cmd == CMD_POP) && 
                (checkBit(code[i], NUM) == 1) && 
                (checkBit(code[i], REG) == 1) && 
                (checkBit(code[i], RAM) == 1)) //pop [5 + rcx]
        {
            i++;
            printf ("in cpu: i = %zu\n", i);
            int nReg = code[i];
            printf ("in cpu: nReg = %d\n", nReg);
            MY_ASSERT (nReg > NUM_REGISTERS-1, "You are out of register memory");
            i++;
            int value1 = regs[nReg];
            int value2 = code[i];
            int ramIndex = value1 + value2;
            MY_ASSERT ((size_t) ramIndex > MAX_RAM-1, "You are out of RAM");
            ram[ramIndex] = stack_pop (&stk, logfile);
        }
        
        else if (cmd == CMD_ADD) //add
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 + n2, logfile);
        }
        else if (cmd == CMD_SUB) //sub
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 - n2, logfile);
        }
        else if (cmd == CMD_MUL) //mul
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 * n2, logfile);
        }
        else if (cmd == CMD_DIV) //div
        {
            int n1 = stack_pop (&stk, logfile);
            int n2 = stack_pop (&stk, logfile);
            stack_push (&stk, n1 / n2, logfile);
        }
        else if (cmd == CMD_OUT) //out
        {
            fprintf (stdout, "OUT: %d\n", stack_pop (&stk, logfile));
        }
        else if (cmd == CMD_IN) // in
        {
            int tmp = getNum ();
            stack_push (&stk, tmp, logfile);
            stack_dump (stk, logfile);
        }
        else if (cmd == CMD_JMP) //jmp
        {
            i++;
            int ptrToJmp = code[i];
            i = ((size_t) ptrToJmp)-1;
        }
        else if (cmd == CMD_JA) //ja  
        {
            JUMP_FORM (>)
        }
        else if (cmd == CMD_JB) //jb
        {
            JUMP_FORM(<)
        }
        else if (cmd == CMD_JBE) //jbe
        {
            JUMP_FORM(<=)
        }
        else if (cmd == CMD_JGE) //jge
        {
            JUMP_FORM (>=)
        }
        else if (cmd == CMD_JE) //je
        {
            JUMP_FORM (==)
        }
        else if (cmd == CMD_CALL) //call
        {
            i++;
            int ptrToJmp = code[i];
            stack_push (&callStack, ((int)i) + 1, logCallStack);
            i = (size_t) ptrToJmp-1;
        }
        else if (cmd == CMD_RET) //RET
        {
            i = (size_t) stack_pop (&callStack, logCallStack) - 1;
        }
        else if (cmd == CMD_SQRT)
        {
            int tmp = stack_pop (&stk, logfile);
            int result = (int) sqrt ((double) tmp);
            stack_push (&stk, result, logfile);
        }
        else 
        {
            stack_dtor (&stk);
            stack_dtor (&callStack);
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

static int * createArrCode (size_t nStrs)
{
    int * code = (int *) calloc (nStrs * 3, sizeof (int));
    MY_ASSERT (code == nullptr, "Unable to allocate new memory");

    return code;
}

static int checkBit(const int value, const int position) 
{
    return ((value & (1 << position)) != 0);
}
