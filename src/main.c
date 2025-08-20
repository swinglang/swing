#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: swingc <file.wh>\n");
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("fopen");
        return 1;
    }

    char buf[1024];
    while (fgets(buf, sizeof(buf), in)) {
        process_line(buf, stdout);
    }

    fclose(in);
    return 0;
}
