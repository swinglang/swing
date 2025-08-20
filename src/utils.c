// utils.c
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "utils.h"

// Remove leading/trailing whitespace
char *trim(char *s) {
    if (!s) return NULL;
    while (isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;

    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';

    return s;
}

// Check if string starts with prefix
bool starts_with(const char *s, const char *prefix) {
    if (!s || !prefix) return false;
    while (*prefix) {
        if (*s != *prefix) return false;
        s++;
        prefix++;
    }
    return true;
}

// Optional helper for integers
bool is_integer(const char *s) {
    if (!s || *s == '\0') return false;
    if (*s == '-' || *s == '+') s++;
    while (*s) {
        if (!isdigit((unsigned char)*s)) return false;
        s++;
    }
    return true;
}
