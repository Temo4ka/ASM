#define ASMBLER_CP
#include <cstdio>
#include <string.h>
#include "asm.h"
#ifndef functionList
    #include "functionList.h"
#endif

const char  *SIGNATURE        =        "CP"      ;
const int    ASM_VERSION      =         3        ;

int argDefenition(Line *args, char **cmdArgs, size_t *dataSize, char command);

int stackAsmBin(Lines *commandList, Label **labels, size_t *labelsNum, FILE *outStream) {
    catchNullptr(commandList);
    catchNullptr( outStream );
    catchNullptr(   labels  );

    char *outputData  = (char *) calloc(commandList -> numberOfLines, sizeof(int));
    char *currentElem =                         outputData                        ;
    size_t dataSize   =                      3 * sizeof(int)                      ;
    int      err      =                             0                             ;
    bool  printFlag   =                             1                             ;

     *((int *) currentElem) = 'C' * 256 + 'P';
    currentElem += sizeof(int);
     *currentElem = ASM_VERSION;
    currentElem += sizeof(int);
    char *sizeOfData = currentElem;
    currentElem += sizeof(int);

    size_t previousCmd = 0;

    for (size_t currentCommand = 0; currentCommand < commandList -> numberOfLines; ++currentCommand) {

        if (*(commandList -> array[currentCommand].line) == ':') {
            commandList -> array[currentCommand].line++;
            int pointer = 0;

            err = labelTryFind(labels, commandList->array[currentCommand].line, *labelsNum, &pointer);
            if (err)
                return err;

            if (pointer == -1) {
                err = labelCtor(&((*labels)[*labelsNum]), previousCmd, commandList -> array[currentCommand].line);
                (*labelsNum)++;
                if (err)
                    return err;
            }
            currentCommand++;
        }

        previousCmd = (currentElem - outputData) - 3 * sizeof(int);

#define DEF_CMD(name, num, arg, ...)                                                                      \
    if (!strcmpi(commandList -> array[currentCommand].line, #name)) {                                     \
        if (arg > 0) {                                                                                    \
            currentCommand++;                                                                             \
                                                                                                          \
            size_t sizeOfCmd = 0;                                                                         \
                                                                                                          \
            argDefenition(&(commandList -> array[currentCommand]), &currentElem, &sizeOfCmd, CMD_##name); \
                                                                                                          \
            dataSize  += sizeOfCmd;                                                                       \
            currentElem += sizeOfCmd;                                                                     \
        } else  {                                                                                         \
            *currentElem = CMD_##name;                                                                    \
            ++currentElem;                                                                                \
            ++dataSize;                                                                                   \
        }                                                                                                 \
    } else                                                                                                \

#define DEF_CMD_JUMP(name, num, oper)                                                                     \
        if (!strcmpi(commandList -> array[currentCommand].line, #name)) {                                 \
            *currentElem = CMD_##name;                                                                    \
            ++currentElem;                                                                                \
            ++currentCommand;                                                                             \
                                                                                                          \
            int pointer = 0;                                                                              \
            err = labelTryFind(labels, commandList -> array[currentCommand].line, *labelsNum, &pointer);  \
            if (err)                                                                                      \
                return err;                                                                               \
                                                                                                          \
            *((int *) currentElem) = pointer;                                                             \
            if (pointer == -1)                                                                            \
                printFlag = 0;                                                                            \
                                                                                                          \
            currentElem += sizeof(int);                                                                   \
              dataSize  += sizeof(int) + sizeof(char);                                                    \
        } else                                                                                            \

#define DEF_CMD_REC(name, num, ...)                                                                       \
        if (!strcmpi(commandList -> array[currentCommand].line, #name)) {                                 \
            *currentElem = CMD_##name;                                                                    \
            ++currentElem;                                                                                \
            ++dataSize;                                                                                   \
        }

#include "cmd.h"

#undef DEF_CMD_JUMP

#undef DEF_CMD

        /* else */
        if (*currentElem != '\0')
            return UndefinedCmd;
    }


    *((int *) sizeOfData) = (int) dataSize;

    if (printFlag)
        fwrite(outputData, sizeof(char), dataSize, outStream);
    free(outputData);

    return OK;
}

int argDefenition(Line *args, char **cmdArgs, size_t *sizeData, char command) {
    catchNullptr(args);

    char         line[20] =      ""     ;
    unsigned char cmd     =    command  ;
    char         RegArg   =       0     ;
    int          NumArg   =       0     ;
                *sizeData = sizeof(char);

    strcpy(line, args -> line);

    char *currentPart = strtok(line, "]+ ");
    while (currentPart != nullptr) {
        if (*currentPart == '[') {
            cmd |= TypeRAM;
            currentPart++;
        }
        if (!stricmp("rax", currentPart)) {
            cmd |= TypeReg;
            RegArg = 1;
            *sizeData += sizeof(char);
        } else if (!stricmp("rbx", currentPart)) {
            cmd |= TypeReg;
            RegArg = 2;
            *sizeData += sizeof(char);
        } else if (!stricmp("rcx", currentPart)) {
            cmd |= TypeReg;
            RegArg = 3;
            *sizeData += sizeof(char);
        } else if (!stricmp("rdx", currentPart)) {
            cmd |= TypeReg;
            RegArg = 4;
            *sizeData += sizeof(char);
        } else {
            cmd |= TypeNum;
            NumArg = strtol(currentPart, &currentPart, 10);
            *sizeData += sizeof(int);
        }
        currentPart = strtok(nullptr, "]+ ");
    }

    catchNullptr(*cmdArgs);

    char *curArg = *cmdArgs;
    *curArg = cmd;
    curArg++;
    if (cmd & TypeReg) {
        *curArg = RegArg;
        curArg++;
    }
    if (cmd & TypeNum)
        *((int *) curArg) = NumArg;

    return OK;

}

int stackAsmTex(Lines *commandList, FILE *outStream) {
    catchNullptr(commandList);
    catchNullptr( outStream );

    fprintf(outStream, "%s %d %zu\n", SIGNATURE, ASM_VERSION, commandList -> numberOfLines);
    for (size_t currentCommand = 0; currentCommand < commandList -> numberOfLines; ++currentCommand) {
        if (!strcmpi(commandList -> array[currentCommand].line, "push")) {
            int value = 0;
            ++currentCommand;
            sscanf(commandList -> array[currentCommand].line, "%d", &value);
            fprintf(outStream, "%d %d\n", CMD_PUSH, value);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "in")) {
            int value = 0;
            ++currentCommand;
            sscanf(commandList -> array[currentCommand].line, "%d", &value);
            fprintf(outStream, "%d %d\n", CMD_IN, value);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "pop")) {
            fprintf(outStream, "%d\n", CMD_POP);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "halt")) {
            fprintf(outStream, "%d\n", CMD_HALT);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "add")) {
            fprintf(outStream, "%d\n", CMD_ADD);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "div")) {
            fprintf(outStream, "%d\n", CMD_DIV);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "out")) {
            fprintf(outStream, "%d\n", CMD_OUT);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "product")) {
            fprintf(outStream, "%d\n", CMD_MULT);
        }
    }

    return OK;
}