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
    catchNullptr(outStream);

    char *outputData  = (char *) calloc(commandList -> numberOfLines, sizeof(int));
    char *currentElem = outputData;
    size_t dataSize   = 3 * sizeof(int);
    int      err      = 0;
    bool  printFlag   = 1;

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
                (*labelsNum)++;
                err = labelCtor(&((*labels)[*labelsNum]), previousCmd, commandList -> array[currentCommand].line);
                if (err)
                    return err;
            } else
                return LabelSameNameErr;
        }
        previousCmd = dataSize - 3 * sizeof(int);
        if (!strcmpi(commandList -> array[currentCommand].line, "push")) {
            currentCommand++;

            size_t sizeOfCmd = 0;
            argDefenition(&(commandList -> array[currentCommand]), &currentElem, &sizeOfCmd, PUSH);

              dataSize  += sizeOfCmd;
            currentElem += sizeOfCmd;
        }  else if (!strcmpi(commandList -> array[currentCommand].line, "pop")) {
            size_t sizeOfCmd = 0;

            ++currentCommand;

            if (*(commandList -> array[currentCommand].line) == '[' || *(commandList -> array[currentCommand].line) == 'r') {
                argDefenition(&(commandList -> array[currentCommand]), &currentElem, &sizeOfCmd, POP);
                currentElem += sizeOfCmd;
                dataSize  += sizeOfCmd;
            } else {
                --currentCommand;

                *currentElem = POP;
                ++currentElem;
                ++dataSize;
            }
        } else if (!strcmpi(commandList -> array[currentCommand].line, "halt")) {
            *currentElem = HALT;
            ++currentElem;
            ++dataSize;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "add")) {
            *currentElem = ADD;
            ++currentElem;
            ++dataSize;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "div")) {
            *currentElem = DIV;
            ++currentElem;
            ++dataSize;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "out")) {
            *currentElem = OUT;
            ++currentElem;
            ++dataSize;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "product")) {
            *currentElem = PRODUCT;
            ++currentElem;
            ++dataSize;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "jump")) {
            *currentElem = JUMP;
            ++currentElem;
            ++currentCommand;

            int pointer = 0;
            err = labelTryFind(labels, commandList -> array[currentCommand].line, *labelsNum, &pointer);
            if (err)
                return err;

            *((int *) currentElem) = pointer;
            if (pointer == -1)
                printFlag = 0;

            currentElem += sizeof(int);
              dataSize  += sizeof(int);
        }
    }
    *((int *) sizeOfData) = dataSize;

    if (printFlag)
        fwrite(outputData, sizeof(char), dataSize, outStream);

    return OK;
}

int argDefenition(Line *args, char **cmdArgs, size_t *sizeData, char command) {
    catchNullptr(args);

    char   cmd   =    command  ;
    char  RegArg =       0     ;
    int   NumArg =       0     ;
       *sizeData = sizeof(char);

    char *currentPart = strtok(args -> line, "]+ ");
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
    catchNullptr(outStream);

    fprintf(outStream, "%s %d %zu\n", SIGNATURE, ASM_VERSION, commandList -> numberOfLines);
    for (size_t currentCommand = 0; currentCommand < commandList -> numberOfLines; ++currentCommand) {
        if (!strcmpi(commandList -> array[currentCommand].line, "push")) {
            int value = 0;
            ++currentCommand;
            sscanf(commandList -> array[currentCommand].line, "%d", &value);
            fprintf(outStream, "%d %d\n", PUSH, value);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "in")) {
            int value = 0;
            ++currentCommand;
            sscanf(commandList -> array[currentCommand].line, "%d", &value);
            fprintf(outStream, "%d %d\n", IN, value);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "pop")) {
            fprintf(outStream, "%d\n", POP);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "halt")) {
            fprintf(outStream, "%d\n", HALT);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "add")) {
            fprintf(outStream, "%d\n", ADD);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "div")) {
            fprintf(outStream, "%d\n", DIV);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "out")) {
            fprintf(outStream, "%d\n", OUT);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "product")) {
            fprintf(outStream, "%d\n", PRODUCT);
        }
    }

    return OK;
}