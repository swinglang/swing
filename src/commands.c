#include "commands.h"

// forward declarations
extern Command cmd_print;
extern Command cmd_let;
extern Command cmd_ref;

Command *commands[] = {
    &cmd_print,
    &cmd_let,
    &cmd_int,
    NULL // sentinel
};
