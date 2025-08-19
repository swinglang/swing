#include <stdio.h>
#include <stdlib.h>
#include "math.h"

// Parse an integer literal (string -> int)
int cmd_int(const char *token) {
    return atoi(token);
}
