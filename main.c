#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Library/library.h"
#include "Macros/macros.h"
#include "Pages/pages.h"
#include "CodeCompletion/cc.h"

void display_lines() {
    clear();
    int row, col;
    getmaxyx(stdscr, row, col);

    int line_offset = 6;
    int max_display_lines = row - 1;

    int start_line = 0;
    int end_line = line_count - 1;

    if (line_count > max_display_lines) {
        if (current_line <= max_display_lines / 2) {
            start_line = 0;
            end_line = max_display_lines - 1;
        } else if (current_line >= line_count - max_display_lines / 2) {
            start_line = line_count - max_display_lines;
            end_line = line_count - 1;
        } else {
            start_line = current_line - max_display_lines / 2;
            end_line = current_line + max_display_lines / 2 - 1;
        }
    }

    for (int i = start_line, screen_line = 0; i <= end_line; i++, screen_line++) {
        mvprintw(screen_line, 0, "%4d: %s", i + 1, lines[i]);
    }

    mvprintw(row - 1, 0, "Line: %d, Column: %d", current_line + 1, current_col + 1);

    mvprintw(row -1, col - strlen(file_name) - 6, "File: %s", file_name);

    move(current_line - start_line, line_offset + current_col);
    refresh();
}

void editor() {
    int c = getch();
    
    switch (c) {
        case KEY_F(4):
            if (file_name[0] != '\0') {
                save_file();
            } else {
                file_save();
            }
            break;
        case KEY_F(3):
            filesystem();
            break;
        case '\t':
            tab();
            break;
        case ESCAPE:
            endwin();
            exit(0);
            break;
        case KEY_F(1):
            display_help();
            break;
        case KEY_F(2):
            display_info();
            break;
        case KEY_UP:
            if (current_line > 0) {
                current_line--;
                if (current_col > strlen(lines[current_line])) {
                    current_col = strlen(lines[current_line]);
                }
            }
            break;
        case KEY_DOWN:
            if (current_line < line_count - 1) {
                current_line++;
                if (current_col > strlen(lines[current_line])) {
                    current_col = strlen(lines[current_line]);
                }
            }
            break;
        case KEY_LEFT:
            if (current_col > 0) {
                current_col--;
            }
            break;
        case KEY_RIGHT:
            if (current_col < strlen(lines[current_line])) {
                current_col++;
            }
            break;
        case '\n':
            if (line_count < MAX_LINES - 1) {
                memmove(&lines[current_line + 2], &lines[current_line + 1], (line_count - current_line - 1) * MAX_COLS);
                strncpy(lines[current_line + 1], &lines[current_line][current_col], MAX_COLS - 1);
                lines[current_line][current_col] = '\0';
                line_count++;
                current_line++;
                current_col = 0;
            }
            break;
        case KEY_BACKSPACE:
        case 127:
            if (current_col > 0) {
                memmove(&lines[current_line][current_col - 1], &lines[current_line][current_col], strlen(lines[current_line]) - current_col + 1);
                current_col--;
            } else if (current_line > 0) {
                current_col = strlen(lines[current_line - 1]);
                if (current_col + strlen(lines[current_line]) < MAX_COLS) {
                    strcat(lines[current_line - 1], lines[current_line]);
                    memmove(&lines[current_line], &lines[current_line + 1], (line_count - current_line) * MAX_COLS);
                    line_count--;
                    current_line--;
                }
            }
            break;
        default:
            if (c >= 32 && c <= 126) {
                insert_char(c);
                if (checks(c)) {
                    if (current_col > 0) {
                        current_col--;
                        move(current_line, current_col + 6);  // +6 for line number offset
                    }
                }
            }
            break;
    }
    
    display_lines();
}

void init_editor() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
}

int main() {
    init_editor();
    lines[0][0] = '\0';
    display_info();

    display_lines();

    while (1) {
        editor();
    }

    endwin();
    return 0;
}