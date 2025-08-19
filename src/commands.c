#include "../include/commands.h"

// forward declarations from src/cmd/*.c
extern Command cmd_print;
extern Command cmd_let;
extern Command cmd_ref;
// etc...

Command commands[] = {
    cmd_print,
    cmd_let,
    cmd_ref,
    {NULL, NULL, NULL} // sentinel
};
