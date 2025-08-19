#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "commands.h"
#include "utils.h"

static bool match_int(const char *line) {
    return starts_with(trim(strdup(line)), "int ") && strstr(line, "();");
}

static void emit_int(const char *line, FILE *out) {
    // Example: int main(); {
    char *copy = strdup(line);
    char *paren = strstr(copy, "();");
    if (!paren) { free(copy); return; }

    *paren = '\0';
    char *name = trim(copy + 4); // skip "int "

    fprintf(out, "int %s() {\n", name);
    free(copy);
}

Command cmd_int = {
    .name = "int",
    .match = match_int,
    .emit = emit_int
};
