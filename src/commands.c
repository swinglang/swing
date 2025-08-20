#include "commands.h"

// forward declarations
extern Command cmd_print;
extern Command cmd_let;
extern Command cmd_int;
extern Command cmd_void;

Command *commands[] = {
    &cmd_print,
    &cmd_let,
    &cmd_int,
    &cmd_void,
    NULL // sentinel
};
