#ifndef CMINUS_DEBUG
#define CMINUS_DEBUG

#include <stdarg.h>

#include <stdio.h>  // Add this

void debug_print(const char *format, ...);
void debug_comment(char* element, int capture);

#endif