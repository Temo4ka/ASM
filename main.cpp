//!     TEMO4KA's ASSEMBLER,
//!   Made with Love and Pain,
//!  If u want to use labels yo should use -> [labelName]: <- before commands
//!  COMMAND LIST: [Command_Name] [Types of Arguments]
//!  [ Push ] [ [], r..x, number]
//!  [ Pop  ] [ [], r..x]
//!  [ Ja ]   [ label name ]
//!  [ Jae ]  [ label name ]
//!  [ Jb ]   [ label name ]
//!  [ Jbe ]  [ label name ]
//!  [ JL ]   [ label name ]
//!  [ JM ]   [ label name ]
//!  [ Add  ] [ - ]
//!  [ Mult ] [ - ]
//!  [ Sub ]  [ - ]
//!  [ Out ]  [ - ]
//!  [ Halt ] [ - ]
//!  [ Call ] [ label name]
//!  [ Ret ]  [ - ]

#define ASSEMBLER_CP

#include "asm.h"
#include <iostream>
#include <cstring>
#include "defaultFileNames.h"

int main(int argc, char *argv[]) {
    const char *  fileIn   =  INPUT_FILE_NAME;
    const char *fileTexOut = OUTPUT_FILE_NAME;
    const char *fileBinOut = OUTBIN_FILE_NAME;

    int err = 0;

    switch (argc) {
        case 4:
              fileIn    = argv[1];
            fileBinOut  = argv[2];
            fileTexOut  = argv[3];
            break;

        case 3:
              fileIn    = argv[1];
            fileBinOut  = argv[2];
            break;

        case 2:
              fileIn    = argv[1];
            break;

        default:
            break;
    }

    Text commandFile = {};
    err = TEXTConstructor(&commandFile, fileIn);
    if (err) return EXIT_FAILURE;

//    FILE *numbCommandTexFile = fopen(fileTexOut,  "w");
//    catchNullptr(numbCommandTexFile);

    Lines commandList = {};
    err = getArrayOfStrings(&commandList, &commandFile);
    if (err)
        return err;

    Label *labels = (Label *) calloc(MAX_NUM_LABELS, sizeof(Label));
    size_t ReadyLabels = 0;
    if (labels == nullptr) return EXIT_FAILURE;

//    err = stackAsmTex(&commandList, numbCommandTexFile);
//    if (err) return err;

    char *outputData = nullptr;

    err = stackAsmBin(&commandList, &labels, &ReadyLabels, &outputData);
    if (err) return EXIT_FAILURE;

    err = stackAsmBin(&commandList, &labels, &ReadyLabels, &outputData);
    if (err) return EXIT_FAILURE;

    for (size_t current = 0; current < ReadyLabels; ++current)
        if (labelDtor(&labels[current])) return EXIT_FAILURE;
    free(labels);

    err = myWrite(outputData, fileBinOut);
    if (err) return EXIT_FAILURE;

    err = textDestructor(&commandFile);
    if (err) return EXIT_FAILURE;

//    fclose(numbCommandTexFile);
    free(outputData);

    return 0;
}
