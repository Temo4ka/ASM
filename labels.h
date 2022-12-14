#include <string.h>

const int MAX_NUM_LABELS = 100;

enum labelStatus {
    Destructed  = 0,
    Constructed = 1,
};

enum labelErrors {
    LabelReConstruction = 1,
    LabelReDestruction  = 2,
};

#define catchNullptr(a) { \
    if ((a) == nullptr) {         \
        fprintf(stderr, "Nullptr caught.\nVariable: %s,\nFile: %s,\nLine: %d,\nFunction: %s\n", #a, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    return NULLCAUGTH;         \
    }\
}

struct Label {
    char *name     =   nullptr ;
    int cmdPointer =      0    ;
    int   status   = Destructed;
};

int labelCtor(Label *label, const size_t cmd, const char *name, const size_t len);

int labelDtor(Label *label) ;

int labelTryFind(Label **labels, const char *labelName, const size_t readyLabels, int *resOfTry);

int addLabel(Label **labels, const char * curLine, const size_t curLen, size_t *labelsNum, const size_t dataSize);