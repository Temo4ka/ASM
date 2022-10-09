#include "asm.h"

int labelCtor(Label *label, const size_t cmd, const char *name) {
    catchNullptr(label);
    if (label -> status == Constructed)
        return LabelReConstruction;

    label ->    name    = (char *) name;
    label -> cmdPointer =      cmd     ;

    label -> status     =  Constructed ;

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