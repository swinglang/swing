#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to read a file into a string
char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}

// Function to convert Whee script to C code
void whee_to_c(const char *whee_code) {
    printf("#include <stdio.h>\n");
    printf("int main() {\n");

    char *code = strdup(whee_code);
    char *line = strtok(code, "\n");

    while (line) {
        // Convert `print("Hello")` to `printf("Hello\\n");`
        if (strstr(line, "print(")) {
            char content[256];
            sscanf(line, "print(%255[^)])", content);
            printf("    printf(%s\"\\n\");\n", content);
        }
        // Handle variables (`var = "value";`)
        else if (strstr(line, "var =")) {
            char var_name[64], var_value[256];
            sscanf(line, "var = \"%255[^\"]\"", var_value);
            printf("    char var[] = \"%s\";\n", var_value);
        }

        line = strtok(NULL, "\n");
    }

    printf("    return 0;\n");
    printf("}\n");

    free(code);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <whee_script>\n", argv[0]);
        return 1;
    }

    char *whee_code = read_file(argv[1]);
    if (!whee_code) return 1;

    printf("=== Converted C Code ===\n");
    whee_to_c(whee_code);

    free(whee_code);
    return 0;
}
