#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    const char *name;
    bool (*match)(const char *line);
    void (*emit)(const char *line, FILE *out);
} Command;

extern Command *commands[];

#endif
