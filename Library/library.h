#ifndef LIBRARY_H
#define LIBRARY_H

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINES 32*1024
#define MAX_COLS 128

char lines[MAX_LINES][MAX_COLS];
int line_count = 1;  // Start with one line
int current_line = 0;
int current_col = 0;

#define ESCAPE 27

int start_line = 0;  // Make start_line global

char copy[128*128];

char file_name[64];

char text[MAX_LINES * MAX_COLS];

void insert_char(char c) {
    if (current_col < MAX_COLS - 1) {
        memmove(&lines[current_line][current_col + 1], &lines[current_line][current_col], strlen(lines[current_line]) - current_col + 1);
        lines[current_line][current_col] = c;
        current_col++;
    }
}

void tab() {
    for (int i = 0; i < 3; i++) {
        insert_char(' ');
    }
}

void transcribe_to_text() {
    int text_index = 0;
    for (int i = 0; i < line_count; i++) {
        int line_length = strlen(lines[i]);
        if (text_index + line_length + 1 >= MAX_LINES * MAX_COLS) {
            // If we're about to exceed the text array size, break the loop
            break;
        }
        strcpy(&text[text_index], lines[i]);
        text_index += line_length;
        
        // Add a newline character after each line, except for the last one
        if (i < line_count - 1) {
            text[text_index] = '\n';
            text_index++;
        }
    }
    // Ensure the text is null-terminated
    text[text_index] = '\0';
}

#endif