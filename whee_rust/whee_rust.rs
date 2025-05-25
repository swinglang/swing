#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>  // JSON library

#define SYNTAX_FILE "/etc/whee/syntax.json"

void load_syntax_rules(json_t **grammar) {
    json_error_t error;
    *grammar = json_load_file(SYNTAX_FILE, 0, &error);
    if (!*grammar) {
        fprintf(stderr, "Error loading syntax file: %s\n", error.text);
        exit(1);
    }
}

void convert_whee_to_c(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening Whee file");
        return;
    }

    json_t *grammar;
    load_syntax_rules(&grammar);

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char command[100], argument[900];
        if (sscanf(line, "%s %[^\n]", command, argument) == 2) {
            json_t *rule = json_object_get(grammar, command);
            if (rule) {
                const char *replacement = json_string_value(json_object_get(rule, "replacement"));

                // If it's a string replacement, apply it properly
                if (strstr(replacement, "%s") != NULL) {
                    // Apply string substitution manually for %s in the replacement string
                    char formatted_replacement[1024];
                    snprintf(formatted_replacement, sizeof(formatted_replacement), replacement, argument);
                    printf("%s\n", formatted_replacement);
                } else {
                    printf(replacement, argument);
                    printf("\n");
                }
            } else {
                printf("// Unrecognized command: %s\n", line);
            }
        }
    }

    fclose(file);
    json_decref(grammar);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: whee-to-c <filename>\n");
        return 1;
    }

    printf("=== Converted C Code ===\n");
    convert_whee_to_c(argv[1]);
    return 0;
}
