#ifndef VARIABLES_H
#define VARIABLES_H

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cfiles.h"
#include "library.h"

#define MAX_VARIABLES 128*128

char* variables[MAX_VARIABLES];

void save_variables(char* var) {
    for (int i = 0; i < MAX_VARIABLES; i++) {
        if (variables[i] == NULL) {
            variables[i] = strdup(var);
            return;
        } else if (strcmp(variables[i], var) == 0) {
            return;
        }
    }
}

void detect_variables(char* line) {
    char line_copy[MAX_COLS];
    strncpy(line_copy, line, MAX_COLS - 1);
    line_copy[MAX_COLS - 1] = '\0';

    char* word = strtok(line_copy, " \t\n");
    while (word != NULL) {
        bool is_keyword = false;
        for (int i = 0; i < sizeof(c_primitive_types) / sizeof(c_primitive_types[0]); i++) {
            if (strcmp(word, c_primitive_types[i]) == 0) {
                is_keyword = true;
                break;
            }
        }
        
        if (is_keyword) {
            word = strtok(NULL, " \t\n;,");
            if (word != NULL) {
                save_variables(word);
            }
        }
        
        word = strtok(NULL, " \t\n");
    }
}

#endif