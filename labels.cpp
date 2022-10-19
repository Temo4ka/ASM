#define ASSEMBLER_CP

#include "asm.h"

int labelCtor(Label *label, const size_t cmd, const char *name, const size_t len) {
    catchNullptr(label);
    if (label -> status == Constructed) return LabelReConstruction;

    label ->    name    = (char *) calloc(len, sizeof(char));
    memcpy(label -> name, name, len);
    label -> cmdPointer =               cmd                 ;
    label -> status     =           Constructed             ;

    return OK;
}

int labelDtor(Label *label) {
    catchNullptr(label);
    if (label -> status == Destructed)
        return LabelReDestruction;

    label ->    name    =   nullptr ;
    label -> cmdPointer =     -1    ;

    label -> status     = Destructed;

    return OK;
}

int addLabel(Label **labels, const char * curLine, const size_t curLen, size_t *labelsNum, const size_t dataSize) {

    int pointer = 0;
    int   err   = 0;

    err = labelTryFind(labels, curLine, *labelsNum, &pointer);
    if (err) return err;

    if (pointer == -1) {
        err = labelCtor(&((*labels)[*labelsNum]), dataSize - sizeof(Header), curLine, curLen);
        (*labelsNum)++;
        if (err) return err;
    }

    return OK;
}

int labelTryFind(Label **labels, const char *labelName, const size_t readyLabels, int *resOfTry) {
    catchNullptr(labels);
    catchNullptr(labelName);
    catchNullptr(resOfTry);

    for (size_t current = 0; current < readyLabels; current++) {
        if (!stricmp((*labels)[current].name, labelName)) {
            *resOfTry =(*labels)[current].cmdPointer;
            return OK;
        }
    }
    *resOfTry = -1;

    return OK;
}