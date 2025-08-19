#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "utils.h"

void process_line(const char *line, FILE *out) {
    for (int i = 0; commands[i].name; i++) {
        if (commands[i].match(line)) {
            commands[i].emit(line, out);
            return;
        }
    }
    fprintf(out, "// Unrecognized: %s\n", line);
}
