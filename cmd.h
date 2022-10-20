// #define CMD_PUSH (nameCmd, numCmd, isArg)
//     if (my_strcmp(nameCmd, ))

// //DEF_CMD принимает имя аргумента, номер команды, и есть ли аргумент аргумент (isArg)

// //если аргумент есть, то вызывается функция GetArgs, которая обрабатывает аргумент. Должна обрабатывать
// //как случай с метками (:), так и случай обращения к оперативной памяти ([...]). Принимает буфер с текстом,
// //который был прочитан из prog.txt, указатель на буфер, куда будут занесены в числовом представлении 
// //аргументы, прочитанный символ (?).

// //сравниваем прочитанную из буфера команду с командой 

// #define DEF_CMD (name, num, arg, ...)
//     case num:
//         __VA_ARGS__
//         ip++;
//         break;

// #define DEF_CMD (name, num, arg) \
//     CMD_##name = num, //что дальше?

//     enum /*...*/ {
//         #include "cmd.h"
//     }



DEF_CMD (PUSH, 4, CMD_PUSH, YES)
DEF_CMD (POP,  3, CMD_POP,  YES)
DEF_CMD (ADD,  3, CMD_ADD,  NO )
DEF_CMD (SUB,  3, CMD_SUB,  NO )
DEF_CMD (MUL,  3, CMD_MUL,  NO )
DEF_CMD (DIV,  3, CMD_DIV,  NO )
DEF_CMD (OUT,  3, CMD_OUT,  NO )
DEF_CMD (JMP,  3, CMD_JMP,  YES)
DEF_CMD (HLT,  3, CMD_HLT,  NO )


// getArgs (char * buf, char * code, int * ip)
// {

// }

// //code - это указатель на тот массив с текстом
//         //который будет отправлен в бинарный файл
                              
// //ip - это instruction pointer этого файла

// //text[i] - i-ая строка исходного текстового файла с кодом

    //в cpu и assembler дефайны DEF_CMD различаются
    //в одном нам нужно переменное число аргументов, а в 
    //другом случае не нужно, но полноценный DEF_CMD
    //соответствующей команды прописаны в cmd.h, который
    //мы подключаем в файл.

                              