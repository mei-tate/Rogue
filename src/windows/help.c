#include "rogue.h"

void showHelpScreen(void) {
    int screenHeight, screenWidth;
    const int windowHeight = 18;
    const int windowWidth = 50;
    WINDOW *help;

    getmaxyx(stdscr, screenHeight, screenWidth);
    if (screenHeight < windowHeight || screenWidth < windowWidth) {
        erase();
        mvprintw(1, 2, "HELP - COMMANDS");
        mvprintw(3, 2, "W/A/S/D  Move: up, left, down, right.");
        mvprintw(4, 2, "h        Opens this help screen.");
        mvprintw(5, 2, "Q        Return to the main menu.");
        mvprintw(12, 2, "Press any key to return to the game.");
        refresh();
        getch();
        return;
    }

    help = newwin(windowHeight, windowWidth,
                  (screenHeight - windowHeight) / 2,
                  (screenWidth - windowWidth) / 2);
    if (help == NULL) return;
    keypad(help, TRUE);
    werase(help);
    box(help, 0, 0);

    if (has_colors()) wattron(help, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
    mvwprintw(help, 2, 3, "HELP - COMMANDS");
    if (has_colors()) wattroff(help, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
    mvwhline(help, 3, 2, ACS_HLINE, windowWidth - 4);

    if (has_colors()) wattron(help, COLOR_PAIR(COLOR_PAIR_START) | A_BOLD);
    mvwprintw(help, 5, 3, "w / a / s / d");
    if (has_colors()) wattroff(help, COLOR_PAIR(COLOR_PAIR_START) | A_BOLD);
    mvwprintw(help, 5, 20, "Move: up, left, down, right.");

    if (has_colors()) wattron(help, COLOR_PAIR(COLOR_PAIR_START) | A_BOLD);
    mvwprintw(help, 6, 3, "h");
    if (has_colors()) wattroff(help, COLOR_PAIR(COLOR_PAIR_START) | A_BOLD);
    mvwprintw(help, 6, 20, "Opens this help screen.");

    if (has_colors()) wattron(help, COLOR_PAIR(COLOR_PAIR_END) | A_BOLD);
    mvwprintw(help, 7, 3, "Q");
    if (has_colors()) wattroff(help, COLOR_PAIR(COLOR_PAIR_END) | A_BOLD);
    mvwprintw(help, 7, 20, "Return to the main menu.");

    wattron(help, A_DIM);
    mvwprintw(help, 16, 3, "Press any key to return to the game.");
    wattroff(help, A_DIM);
    wrefresh(help);
    wgetch(help);
    delwin(help);
    touchwin(stdscr);
    refresh();
}
