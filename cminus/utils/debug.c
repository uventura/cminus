#include "debug.h"

#include <stdarg.h>
#include <stdio.h>

#define LEXER_COMMENT_LIMIT 10000

void debug_print(const char *format, ...) {
    #ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
    #endif
}

void debug_comment(char* element, int capture) {
    // https://stackoverflow.com/questions/1130597/start-states-in-lex-flex
    static char buffer[LEXER_COMMENT_LIMIT];
    static long unsigned index = 0;

    if(capture) {
        if (index < sizeof(buffer)-1)
            buffer[index++] = element[0];
    } else {
        buffer[index] = '\0';
        debug_print("\033[0;35m<Start Comment>\033[0m\n %s\n\033[0;35m<End Comment>\033[0m", buffer);
        index = 0;
    }
}
