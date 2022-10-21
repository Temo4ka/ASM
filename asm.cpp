#define ASSEMBLER_CP
#include <cstdio>
#include <string.h>
#include "asm.h"
#ifndef functionList
    #include "functionList.h"
#endif

//const char  *SIGNATURE        =        "CP"      ;
const int ASM_VERSION =         3       ;
const int  SIGNATURE  = ('C' << 8) + 'P';

int argDefinition(Line *args, char *cmdArgs, size_t *dataSize, char command);

int headerCtor(Header *header, size_t signature, size_t version, size_t dataSize);

int stackAsmBin(Lines *commandList, Label **labels, size_t *labelsNum, char** outputData) {
    catchNullptr(commandList);
    catchNullptr(   labels  );

         *outputData  = (char *) calloc(commandList -> numberOfLines, 2 * sizeof(int) + 3);
    char *currentElem =                *outputData + sizeof(Header)                       ;
    size_t dataSize   =                        sizeof(Header)                             ;
    int      err      =                              0                                    ;

    for (size_t currentCommand = 0; currentCommand < commandList -> numberOfLines; ++currentCommand) {
        if (commandList -> array[currentCommand].line[0] == '\0')
            continue;

        while (commandList -> array[currentCommand].line[0] == '/' &&
               commandList -> array[currentCommand].line[1] == '/' && currentCommand < commandList -> numberOfLines)
            ++currentCommand;

        char  *curLine = commandList -> array[currentCommand].line  ;
        size_t curLen  = commandList -> array[currentCommand].length;

        if (curLine[curLen - 1]  == ':') {
            curLine[curLen - 1]  = '\0';

            err = addLabel(labels, curLine, curLen, labelsNum, dataSize);
            if (err) return err;

            continue;
        }

#define DEF_CMD(name, num, arg, ...)                                                                      \
    if (!strcmpi(commandList -> array[currentCommand].line, #name)) {                                     \
        if (arg > 0) {                                                                                    \
            currentCommand++;                                                                             \
                                                                                                          \
            size_t sizeOfCmd = 0;                                                                         \
                                                                                                          \
            argDefinition(&(commandList -> array[currentCommand]), currentElem, &sizeOfCmd, CMD_##name);  \
                                                                                                          \
            dataSize  += sizeOfCmd;                                                                       \
            currentElem += sizeOfCmd;                                                                     \
        } else  {                                                                                         \
            *currentElem = CMD_##name;                                                                    \
            ++currentElem;                                                                                \
            ++dataSize;                                                                                   \
        }                                                                                                 \
    } else                                                                                                \

    // def_cmd
    // def_jmp

#define DEF_CMD_JUMP(name, num, oper, ...)                                                                \
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
                                                                                                          \
            currentElem +=        sizeof(int)        ;                                                    \
              dataSize  += sizeof(int) + sizeof(char);                                                    \
        } else                                                                                            \

#include "cmd.h"

#undef DEF_CMD_JUMP
#undef DEF_CMD

        /* else */
        if (*currentElem != '\0')
            return UndefinedCmd;
    }

    Header binHeader  ={};
    if (headerCtor(&binHeader, SIGNATURE, ASM_VERSION, dataSize)) return EXIT_FAILURE;
    *((Header *) *outputData) = binHeader;

    return OK;
}

int argDefinition(Line *args, char *cmdArgs, size_t *dataSize, char command) {
    catchNullptr(args);
    catchNullptr(cmdArgs);

    char         line[20] =      ""     ;
    unsigned char cmd     =    command  ;
    char         RegArg   =       0     ;
    int          NumArg   =       0     ;
                *dataSize = sizeof(char);

    strcpy(line, args -> line);

    char *currentPart = strtok(line, "]+ ");
    while (currentPart != nullptr) {
        if (*currentPart == '[') {
            cmd |= TypeRAM;
            currentPart++;
        }
        if (strlen(currentPart) == 3 && currentPart[0] == 'r' && currentPart[2] == 'x') {
            cmd |= TypeReg;
            RegArg = (char) (currentPart[1] - 'a' + 1);
            *dataSize += sizeof(char);
        } else {
            cmd |= TypeNum;
            NumArg = strtol(currentPart, &currentPart, 10);
            *dataSize += sizeof(int);
        }
        currentPart = strtok(nullptr, "]+ ");
    }

    *cmdArgs = cmd;
    cmdArgs++;
    if (cmd & TypeReg) {
        *cmdArgs = RegArg;
        cmdArgs++;
    }
    if (cmd & TypeNum)
        *((int *) cmdArgs) = NumArg;

    return OK;

}

int headerCtor(Header *header, size_t signature, size_t version, size_t dataSize) {
    catchNullptr(header);

    header -> signature = signature;
    header ->  version  = version;
    header -> dataSize  = dataSize;

    return OK;
}

int headerDtor(Header *header) {
    catchNullptr(header);

    header -> signature = -1;
    header ->  version  = -1;
    header -> dataSize  = -1;

    return OK;
}

int myWrite(char *outputData, const char *fileName) {
    catchNullptr(outputData);
    catchNullptr( fileName  );

    FILE *stream = fopen(fileName, "wb");
    catchNullptr(stream);

    Header header = *((Header *) outputData);
    if (fwrite(outputData, sizeof(char), header.dataSize, stream) != header.dataSize)
        return EXIT_FAILURE;

    fclose(stream);

    return OK;
}

/*
int stackAsmTex(Lines *commandList, FILE *outStream) {
    catchNullptr(commandList);
    catchNullptr( outStream );

    // Make full...! and verbose!
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
*/