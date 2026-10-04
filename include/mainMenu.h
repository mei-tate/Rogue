#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <ncurses.h>
#include <stdlib.h>

enum {START_GAME, QUIT_GAME};

int mainMenu(int numberItems, char * choices[]);

#endif
