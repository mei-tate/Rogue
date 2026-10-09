#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <ncurses.h>
#include <stdlib.h>

enum {START_GAME, SHOW_HELP, QUIT_GAME};

int mainMenu(int numberItems, char * choices[]);

#endif
