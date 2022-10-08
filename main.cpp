#include <iostream>
#include <cstring>

#include "functionList.h"
#ifndef catchNullptr
    #include "asm.h"
#endif

const char *INPUT_FILE_NAME  = "commandtext.txt";
const char *OUTPUT_FILE_NAME = "commandnumb.txt";
const char *OUTBIN_FILE_NAME =  "bincommand.bin";

int main(int argc, char *argv[]) {
    char *  fileIn   = nullptr;
    char *fileTexOut = nullptr;
    char *fileBinOut = nullptr;

    int err = 0;

    switch (argc) {
        case 4:   fileIn    =           argv[3]        ;
                fileTexOut  =           argv[2]        ;
                fileBinOut  =           argv[2]        ;
                  break;

        case 3:   fileIn    =           argv[2]        ;
                fileTexOut  =           argv[1]        ;
                fileBinOut  = (char *) OUTBIN_FILE_NAME;
                  break;

        case 2:   fileIn    =           argv[1]        ;
                fileTexOut  = (char *) OUTPUT_FILE_NAME;
                fileBinOut  = (char *) OUTBIN_FILE_NAME;
                  break;
        default:   fileIn   = (char *)  INPUT_FILE_NAME;
                 fileTexOut = (char *) OUTPUT_FILE_NAME;
                 fileBinOut = (char *) OUTBIN_FILE_NAME;
                  break;
    }

    Text commandFile = {};
    err = TEXTConstructor(&commandFile, fileIn);
    if (err)
        return err;

    FILE *numbCommandTexFile = fopen(fileTexOut,  "w");
    FILE *numbCommandBinFile = fopen(fileBinOut, "wb");

    catchNullptr(numbCommandTexFile);

    Lines commandList = {};
    err = getArrayOfStrings(&commandList, &commandFile);
    if (err)
        return err;

    err = stackAsmTex(&commandList, numbCommandTexFile);
    if (err)
        return err;

    err = stackAsmBin(&commandList, numbCommandBinFile);
    if (err)
        return err;

    err = textDestructor(&commandFile);
    if (err)
        return err;
    fclose(numbCommandTexFile);
    fclose(numbCommandBinFile);
    return 0;
}
