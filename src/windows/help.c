#include "rogue.h"

void showHelpScreen(void) {
    int screenHeight, screenWidth;
    const int windowHeight = 20;
    const int windowWidth = 58;
    WINDOW *help;
    getmaxyx(stdscr, screenHeight, screenWidth);
    if (screenHeight < windowHeight || screenWidth < windowWidth) {
        erase();
        mvprintw(1, 2, "HOW TO PLAY");
        mvprintw(3, 2, "WASD/arrows move. I opens your pack. R rests.");
        mvprintw(5, 2, "Battle: A attack, S arcane strike, P potion, F flee.");
        mvprintw(7, 2, "! heal  ? mana  ) weapon  ] armor  $ gold");
        mvprintw(9, 2, "Defeat every foe to reveal O and descend.");
        mvprintw(11, 2, "Clear six floors to reclaim the crown.");
        mvprintw(14, 2, "Press any key to return.");
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
    mvwprintw(help, 2, 3, "THE HOLLOW CROWN - FIELD GUIDE");
    if (has_colors()) wattroff(help, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
    mvwhline(help, 3, 2, ACS_HLINE, windowWidth - 4);
    mvwprintw(help, 5, 3, "WASD / arrows");
    mvwprintw(help, 5, 21, "Move through rooms and halls.");
    mvwprintw(help, 6, 3, "I");
    mvwprintw(help, 6, 21, "Open pack; press a number to use.");
    mvwprintw(help, 7, 3, "R");
    mvwprintw(help, 7, 21, "Rest for one mana; enemies take a turn.");
    mvwprintw(help, 9, 3, "A / S / P / F");
    mvwprintw(help, 9, 21, "Battle: attack, spell, potion, flee.");
    mvwprintw(help, 11, 3, "! ? ) ] $");
    mvwprintw(help, 11, 21, "Healing, mana, weapon, armor, gold.");
    mvwprintw(help, 13, 3, "O");
    mvwprintw(help, 13, 21, "Appears after the last foe falls.");
    mvwprintw(help, 15, 3, "THE CROWN");
    mvwprintw(help, 15, 21, "Clear six floors to reclaim it.");
    wattron(help, A_DIM);
    mvwprintw(help, 18, 3, "Press any key to return.");
    wattroff(help, A_DIM);
    wrefresh(help);
    wgetch(help);
    delwin(help);
    touchwin(stdscr);
    refresh();
}
