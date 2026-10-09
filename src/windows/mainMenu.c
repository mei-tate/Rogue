#include "mainMenu.h"
#include "rogue.h"

static void drawMenu(WINDOW *window, int selected, char *choices[], int count) {
    int height, width;
    getmaxyx(window, height, width);
    werase(window);
    box(window, 0, 0);

    if (has_colors()) wattron(window, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
    mvwprintw(window, 2, 4, "THE HOLLOW CROWN");
    if (has_colors()) wattroff(window, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
    wattron(window, A_DIM);
    mvwprintw(window, 4, 4, "Six floors below. One crown to reclaim.");
    wattroff(window, A_DIM);
    mvwhline(window, 6, 2, ACS_HLINE, width - 4);

    for (int i = 0; i < count; i++) {
        int row = 9 + i * 2;
        int pair = (i == 0) ? COLOR_PAIR_START : COLOR_PAIR_END;
        if (has_colors()) wattron(window, COLOR_PAIR(pair) | A_BOLD);
        if (i == selected) {
            wattron(window, A_UNDERLINE);
            mvwprintw(window, row, 7, "  >  %-20s  ", choices[i]);
            wattroff(window, A_UNDERLINE);
        } else {
            mvwprintw(window, row, 10, "%s", choices[i]);
        }
        if (has_colors()) wattroff(window, COLOR_PAIR(pair) | A_BOLD);
    }

    wattron(window, A_DIM);
    mvwprintw(window, height - 3, 2, "W/S or arrows to choose; ENTER to confirm");
    wattroff(window, A_DIM);

    wrefresh(window);
}

int mainMenu(int numberItems, char *choices[]) {
    int screenHeight, screenWidth;
    int selected = 0;
    WINDOW *window;

    if (numberItems <= 0 || choices == NULL) return QUIT_GAME;
    getmaxyx(stdscr, screenHeight, screenWidth);
    if (screenHeight < 22 || screenWidth < 52) {
        erase();
        mvprintw(0, 0, "Enlarge the terminal to at least 52 columns by 22 rows.");
        mvprintw(2, 0, "Press any key to exit.");
        refresh();
        getch();
        return QUIT_GAME;
    }

    int windowHeight = 20;
    int windowWidth = 50;
    int startY = (screenHeight - windowHeight) / 2;
    int startX = (screenWidth - windowWidth) / 2;
    window = newwin(windowHeight, windowWidth, startY, startX);
    if (window == NULL) return QUIT_GAME;
    keypad(window, TRUE);

    while (1) {
        int key;
        erase();
        if (has_colors()) {
            attron(COLOR_PAIR(COLOR_PAIR_STATUS) | A_DIM);
            for (int y = 0; y < screenHeight; y += 2)
                mvaddch(y, 0, ACS_CKBOARD);
            attroff(COLOR_PAIR(COLOR_PAIR_STATUS) | A_DIM);
        }
        refresh();
        drawMenu(window, selected, choices, numberItems);
        key = wgetch(window);
        switch (key) {
            case KEY_UP:
            case 'w':
                selected = (selected + numberItems - 1) % numberItems;
                break;
            case KEY_DOWN:
            case 's':
                selected = (selected + 1) % numberItems;
                break;
            case '\n':
            case '\r':
            case KEY_ENTER:
                delwin(window);
                return selected;
            case 'q':
            case 27:
                delwin(window);
                return QUIT_GAME;
            default:
                break;
        }
    }
}
