#ifndef CFILES_H
#define CFILES_H

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_DEFINITIONS 1024

const char* c_primitive_types[] = {
    "void", "int", "char", "short", "long", "float", "double",

    "size_t", "int8_t", "int16_t", "int32_t", "int64_t",

    "uint8_t", "uint16_t", "uint32_t", "uint64_t",
};

const char* c_blue_keywords[] = { 
    "void", "char", "short", "int", "long", "float", "double",
    "size_t", "int8_t", "int16_t", "int32_t", "int64_t",
    "uint8_t", "uint16_t", "uint32_t", "uint64_t",
    
    "const", "volatile",
    
    "auto", "register", "static", "extern",

    "signed", "unsigned", "bool", "struct", "union", "enum", "class",
    "inline", "virtual", "explicit", "friend", "typedef", "namespace",
    "template", "typename", "mutable", "using", "asm", "alignas", "alignof",
    "decltype", "constexpr", "noexcept", "thread_local", "nullptr"
};

const char* c_purple_keywords[] = { 
    "if", "else", "switch", "case", "default",
    "for", "while", "do",
    "goto", "continue", "break", "return",
    "#ifndef", "#define", "#undef", "#include", "#ifdef", "#endif",
    "#if", "#elif", "#else",
    "#error", "#warning", "#pragma"
};

const char* c_yellow_keywords[] = {
    "\\t", "\\n", "\\r", "\\v", "\\f", "\\a", "\\b", "\\e", "\\0"
};

const char* c_light_blue_keywords[] = {
    "%%d", "%%f", "%%c", "%%s", "%%p", "%%ld", "%%lld", 
    "%%lu", "%%llu", "%%x", "%%X", "%%o", "%%O", "%%u",
};

char* definitions[MAX_DEFINITIONS];

void save_definition(char* def) {
    for (int i = 0; i < MAX_DEFINITIONS; i++) {
        if (definitions[i] == NULL) {
            definitions[i] = strdup(def);
            return;
        } else if (strcmp(definitions[i], def) == 0) {
            return;
        }
    }
}

void detect_defines(char* line) {
    char line_copy[MAX_COLS];
    strncpy(line_copy, line, MAX_COLS - 1);
    line_copy[MAX_COLS - 1] = '\0';
    char* word = strtok(line_copy, " \t\n");
    while (word != NULL) {
        bool is_defined = false;
        if (strcmp(word, "#define") == 0) {
            bool is_defined = true;
        }
        if (is_defined) {
            word = strtok(NULL, " \t\n;,");
            if (word != NULL) {
                save_definition(word);
            }
        }    
        word = strtok(NULL, " \t\n");
    }
}

#endif