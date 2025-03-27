#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_FILE "/tmp/whee_temp.c"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <whee_script>\n", argv[0]);
        return 1;
    }

    char command[512];

    // Run whee-to-c and store output in temp file
    snprintf(command, sizeof(command), "whee-to-c \"%s\" | tail -n +2 > %s", argv[1], TEMP_FILE);
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error: whee-to-c failed\n");
        return 1;
    }

    // Compile and run the generated C code
    snprintf(command, sizeof(command), "gcc %s -o /tmp/whee_exec && /tmp/whee_exec", TEMP_FILE);
    result = system(command);

    // Cleanup
    remove(TEMP_FILE);
    remove("/tmp/whee_exec");

    return result;
}
