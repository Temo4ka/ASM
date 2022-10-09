#ifndef functionList
#include "functionList.h"
#endif
#include "labels.h"

#ifndef catchNullptr
#define catchNullptr(a) { \
    if ((a) == nullptr) {         \
        fprintf(stderr, "Nullptr caught.\nVariable: %s,\nFile: %s,\nLine: %d,\nFunction: %s\n", #a, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    return NULLCAUGTH;         \
    }\
}
#endif

enum ArgType {
    TypeNum = (1 << 4),
    TypeReg = (1 << 5),
    TypeRAM = (1 << 6)
};

enum StackCommands {
      HALT  = 0,
      PUSH  = 1,
      POP   = 2,
      ADD   = 3,
      OUT   = 4,
      IN    = 5,
      DIV   = 6,
    PRODUCT = 7,
      JUMP  = 8,
};

int stackAsmTex(Lines *commandList, FILE *outStream);

int stackAsmBin(Lines *commandList, Label **labels, size_t *labelsNum, FILE *outStream);