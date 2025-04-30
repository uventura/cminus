#include "debug.h"

#include <stdio.h>

void debug_print(char** message) {
    #ifdef DEBUG
        printf("%s\n", message);
    #endif
}

void token_debug_print(char** token) {
    #ifdef DEBUG
        printf("Token: %s\n", token);
    #endif
}