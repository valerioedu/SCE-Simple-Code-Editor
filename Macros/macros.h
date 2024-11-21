#ifndef MACROS_H
#define MACROS_H

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../File/editorfile.h"
#include "../Library/library.h"

void CTRL_C() {
    
}

void CTRL_V() {
    for (int i = 0; i < strlen(copy); i++) {
        insert_char(copy[i]);
    }
}

#endif