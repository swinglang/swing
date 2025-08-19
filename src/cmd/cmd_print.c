#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../include/commands.h"
#include "../include/utils.h"

static bool match_print(const char *line) {
    return starts_with(trim(strdup(line)), "print(");
}

static void emit_print(const char *line, FILE *out) {
    const char *start = strchr(line, '(');
    const char *end   = strrchr(line, ')');
    if (!start || !end) return;

    char inside[256];
    size_t len = end - start - 1;
    strncpy(inside, start+1, len);
    inside[len] = '\0';

    fprintf(out, "printf(\"%%s\\n\", %s);\n", inside);
}

Command cmd_print = {
    .name = "print",
    .match = match_print,
    .emit = emit_print
};
