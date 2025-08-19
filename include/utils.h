#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

bool starts_with(const char *s, const char *prefix);
char *trim(char *s);
bool is_integer(const char *s);

#endif
