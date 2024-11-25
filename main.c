#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/library.h"
#include "include/macros.h"
#include "include/pages.h"
#include "include/cc.h"
#include "include/colors.h"

void update_screen_content(int start_line) {
    int row, col;
    getmaxyx(stdscr, row, col);
    
    for (int i = 0; i < row - 1; i++) {
        move(i, 0);
        clrtoeol();
        if (i + start_line < line_count) {
            mvprintw(i, 0, "%4d: %s", i + start_line + 1, lines[i + start_line]);
            detect_variables(lines[i + start_line]);
        }
    }

    int end_line = line_count - 1;
    int line_offset = 6;

    for (int i = start_line, screen_line = 0; i <= end_line; i++, screen_line++) {
        mvprintw(screen_line, 0, "%4d: ", i + 1);
        
        KeywordInfo blue_info = check_blue_keywords(lines[i]);
        KeywordInfo purple_info = check_purple_keywords(lines[i]);
        KeywordInfo function_info = check_functions(lines[i]);
        KeywordInfo parentheses_info = color_parentheses(lines[i]);
        KeywordInfo variable_info = check_variables(lines[i]);

        int blue_idx = 0, purple_idx = 0, function_idx = 0, parentheses_idx = 0, variable_idx = 0;
        
        for (int j = 0; j < strlen(lines[i]); j++) {
            move(screen_line, line_offset + j);
            
            if (blue_idx < blue_info.count && j == blue_info.keywords[blue_idx].start) {
                attron(COLOR_PAIR(1));
            } else if (purple_idx < purple_info.count && j == purple_info.keywords[purple_idx].start) {
                attron(COLOR_PAIR(2));
            } else if (function_idx < function_info.count && j == function_info.keywords[function_idx].start) {
                attron(COLOR_PAIR(3));
            } else if (parentheses_idx < parentheses_info.count && j == parentheses_info.keywords[parentheses_idx].start) {
                attron(COLOR_PAIR(4));
            } else if (variable_idx < variable_info.count && j == variable_info.keywords[variable_idx].start) {
                attron(COLOR_PAIR(5));
            }

            addch(lines[i][j]);

            if (blue_idx < blue_info.count && j == blue_info.keywords[blue_idx].end - 1) {
                attroff(COLOR_PAIR(1));
                blue_idx++;
            } else if (purple_idx < purple_info.count && j == purple_info.keywords[purple_idx].end - 1) {
                attroff(COLOR_PAIR(2));
                purple_idx++;
            } else if (function_idx < function_info.count && j == function_info.keywords[function_idx].end - 1) {
                attroff(COLOR_PAIR(3));
                function_idx++;
            } else if (parentheses_idx < parentheses_info.count && j == parentheses_info.keywords[parentheses_idx].end - 1) {
                attroff(COLOR_PAIR(4));
                parentheses_idx++;
            } else if (variable_idx < variable_info.count && j == variable_info.keywords[variable_idx].end - 1) {
                attroff(COLOR_PAIR(5));
                variable_idx++;
            }
        }
    }
}

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

    mvprintw(row - 1, 0, "Line: %d, Column: %d", current_line + 1, current_col + 1);
    mvprintw(row - 1, col - strlen(file_name) - 6, "File: %s", file_name);
}

void editor() {
    int c = getch();
    bool need_redraw = false;
    int row, col;
    getmaxyx(stdscr, row, col);
    
    switch (c) {
        case KEY_DC:
            if (current_col < strlen(lines[current_line])) {
                memmove(&lines[current_line][current_col], &lines[current_line][current_col + 1], strlen(lines[current_line]) - current_col);
                need_redraw = true;
            } else if (current_line < line_count - 1) {
                if (strlen(lines[current_line]) + strlen(lines[current_line + 1]) < MAX_COLS) {
                    strcat(lines[current_line], lines[current_line + 1]);
                    memmove(&lines[current_line + 1], &lines[current_line + 2], (line_count - current_line - 2) * MAX_COLS);
                    line_count--;
                    need_redraw = true;
                }
            }
            break;
        case KEY_F(4):
            if (file_name[0] != '\0') {
                save_file();
            } else {
                file_save();
            }
            need_redraw = true;
            break;
        case KEY_F(3):
            filesystem();
            need_redraw = true;
            break;
        case '\t':
            tab();
            need_redraw = true;
            break;
        case ESCAPE:
            endwin();
            exit(0);
            break;
        case KEY_F(1):
            display_help();
            need_redraw = true;
            break;
        case KEY_F(2):
            display_info();
            need_redraw = true;
            break;
        case KEY_UP:
            if (current_line > 0) {
                current_line--;
                if (current_col > strlen(lines[current_line])) {
                    current_col = strlen(lines[current_line]);
                }
                need_redraw = true;
            }
            break;
        case KEY_DOWN:
            if (current_line < line_count - 1) {
                current_line++;
                if (current_col > strlen(lines[current_line])) {
                    current_col = strlen(lines[current_line]);
                }
                need_redraw = true;
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
                need_redraw = true;
            }
            break;
        case KEY_BACKSPACE:
        case 127:
            if (current_col > 0) {
                memmove(&lines[current_line][current_col - 1], &lines[current_line][current_col], strlen(lines[current_line]) - current_col + 1);
                current_col--;
                need_redraw = true;
            } else if (current_line > 0) {
                current_col = strlen(lines[current_line - 1]);
                if (current_col + strlen(lines[current_line]) < MAX_COLS) {
                    strcat(lines[current_line - 1], lines[current_line]);
                    memmove(&lines[current_line], &lines[current_line + 1], (line_count - current_line) * MAX_COLS);
                    line_count--;
                    current_line--;
                    need_redraw = true;
                }
            }
            break;
        default:
            if (c >= 32 && c <= 126) {
                insert_char(c);
                if (checks(c)) {
                    if (current_col > 0) {
                        current_col--;
                    }
                }
                need_redraw = true;
                detect_variables(lines[current_line]);
            }
            break;
    }
    
    int screen_line = current_line;
    int start_line = 0;
    if (line_count > row - 1) {
        if (current_line > row / 2) {
            start_line = current_line - row / 2;
            screen_line = row / 2;
        }
        if (start_line + row > line_count) {
            start_line = line_count - row + 1;
            screen_line = current_line - start_line;
        }
    }
    
    // Move cursor to the correct position before any screen update
    move(screen_line, 6 + current_col);
    
    if (need_redraw) {
        // Use a separate function to update the screen content
        update_screen_content(start_line);
    }
    
    // Update status line without moving the cursor
    mvprintw(row - 1, 0, "Line: %d, Column: %d", current_line + 1, current_col + 1);
    mvprintw(row - 1, col - strlen(file_name) - 6, "File: %s", file_name);
    
    // Ensure the cursor is at the correct position
    move(screen_line, 6 + current_col);
    
    // Use curs_set to make the cursor visible
    curs_set(1);
    
    // Refresh only the changed parts of the screen
    wnoutrefresh(stdscr);
    doupdate();
}

void init_editor() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    memset(variables, 0, sizeof(variables));
    if (can_change_color()) {
        init_color(8, 0, 0, 800);       // Dark blue
        init_color(9, 600, 0, 600);     // Purple
        init_color(10, 1000, 1000, 0);  // Bright yellow for functions
        init_color(11, 800, 800, 0);    // Dark yellow for parentheses
    }
    
    init_pair(1, 8, COLOR_BLACK);   // Dark blue for type keywords
    init_pair(2, 9, COLOR_BLACK);   // Purple for purple keywords
    init_pair(3, 11, COLOR_BLACK);  // Bright yellow for functions
    init_pair(4, 10, COLOR_BLACK);  // Dark yellow for parentheses
    init_pair(5, COLOR_CYAN, COLOR_BLACK);  // Cyan for variables
}

int main() {
    init_editor();
    lines[0][0] = '\0';
    display_info();

    for (int i = 0; i < line_count; i++) {
        detect_variables(lines[i]);
    }

    display_lines();
    update_screen_content(0);

    while (1) {
        editor();
    }

    endwin();
    return 0;
}