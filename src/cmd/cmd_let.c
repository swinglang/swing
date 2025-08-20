#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <commands.h>
#include <utils.h>

static bool match_let(const char *line) {
    return starts_with(trim(strdup(line)), "let ");
}

static void emit_let(const char *line, FILE *out) {
    // line: let x = 1+1;
    char *copy = strdup(line);
    char *eq = strchr(copy, '=');
    if (!eq) { free(copy); return; }

    *eq = '\0';
    char *var = trim(copy + 4); // skip "let "
    char *expr = trim(eq + 1);

    // remove trailing ';'
    size_t len = strlen(expr);
    if (len > 0 && expr[len-1] == ';') expr[len-1] = '\0';

    fprintf(out, "int %s = %s;\n", var, expr);

    free(copy);
}

Command cmd_let = {
    .name = "let",
    .match = match_let,
    .emit = emit_let
};
