#include "readCode.h"

static size_t size (const char * arg_console);
static void writeTextToBuf (code_t * code, FILE * codeFile);
static int correctBuf (char * buf, int nElements);
static char ** arrPtrToStrings (int nStrs, size_t size);
static void fillArr (char ** arrPtrsToStrings, char * buf, int nElem, code_t code);
static void removeSpaces (char * dest, const char * source);
static void skipSpace (char ** strCode, int countLetters);

code_t readCode (const char * nameFile)
{
    MY_ASSERT (nameFile == nullptr, "You did not pass the file name");
    code_t fileInfo = {};
    fileInfo.sizeFile = size (nameFile);

    FILE * codeFile = fopen (nameFile, "r");
    MY_ASSERT (codeFile == nullptr, "Unable to open code file");
    setbuf(codeFile, NULL);
    writeTextToBuf (&fileInfo, codeFile);
    fileInfo.nStrs = correctBuf (fileInfo.buf, fileInfo.sizeFile);
    fileInfo.arrStrs = arrPtrToStrings (fileInfo.nStrs, sizeof(char *));
    fillArr (fileInfo.arrStrs, fileInfo.buf, fileInfo.sizeFile, fileInfo);

    fclose (codeFile);

    return fileInfo;
}

static size_t size (const char * argConsole)
{
    struct stat code = {};
    size_t val = stat (argConsole, &code);

    return code.st_size;
}

static void writeTextToBuf (code_t * code, FILE * codeFile)
{
    MY_ASSERT (code == nullptr, "There is no access to the struct with the necessary information");
    MY_ASSERT (codeFile == nullptr, "There is no access to the code file");

    code->buf = (char *) calloc (code->sizeFile + 1, sizeof(char));
    MY_ASSERT (code->buf == nullptr, "Unable to allocate new memory");

    size_t read_ok = fread (code->buf, sizeof(char), code->sizeFile,  codeFile);
    (code->buf)[code->sizeFile] = '\0';
}

static int correctBuf (char * buf, int nElements) 
{
	int nStrings = 0;

	for (int i = 0; i < nElements; i++) 
	{   
        if (buf[i] == ';')
        {
            while (buf[i] != '\n')
            {
                buf[i] = '\0';
                i++;
            }
            buf[i] = '\0'; 
            nStrings++; 
        }

		if (buf[i] == '\n')
		{
			buf[i] = '\0';
		}   
	}
	return nStrings;
}

static char ** arrPtrToStrings (int nStrs, size_t size)
{
	char ** array = (char **) calloc (nStrs, size);
	MY_ASSERT (array == NULL, "Memory allocation error\n");
	return array;
}

static void fillArr (char ** arrPtrsToStrings, char * buf, int nElem, code_t code) 
{
    skipSpace (&buf, 0);
	arrPtrsToStrings[0] = buf;
	for (int nSym = 0, j = 1; nSym < nElem && j < code.nStrs && *buf != EOF; nSym++)
	{
		if (*buf == '\0')
		{
            while (*buf == '\0')
            {
                buf++;
            }
            skipSpace (&buf, 0);
			arrPtrsToStrings[j] = buf;
			j++;
		}
		buf++;
	}
}

static void skipSpace (char ** strCode, int countLetters)
{
    *strCode = *strCode + countLetters;
	int i = 0;
    for (; **strCode == ' ' || **strCode == '\t' || **strCode == '\0'; i++)
    {
        (*strCode)++;
    }
}
