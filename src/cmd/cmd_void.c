#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <commands.h>
#include <utils.h>

static bool match_void(const char *line) {
    return starts_with(trim(strdup(line)), "fn ") && strstr(line, "();");
}

static void emit_void(const char *line, FILE *out) {
    // Example: fn ggg(); {
    char *copy = strdup(line);
    char *paren = strstr(copy, "();");
    if (!paren) { free(copy); return; }

    *paren = '\0';
    char *name = trim(copy + 3); // skip "fn "

    fprintf(out, "void %s() {\n", name);
    free(copy);
}

Command cmd_void = {
    .name = "void",
    .match = match_void,
    .emit = emit_void
};
