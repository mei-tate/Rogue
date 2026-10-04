#include "rogue.h"

int screenSetUp(void) {
    if (initscr() == NULL) return 0;

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(COLOR_PAIR_START, COLOR_GREEN, -1);
        init_pair(COLOR_PAIR_END, COLOR_RED, -1);
        init_pair(COLOR_PAIR_HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COLOR_PAIR_STATUS, COLOR_CYAN, -1);
        init_pair(COLOR_PAIR_HEALTH, COLOR_GREEN, -1);
    }

    srand((unsigned int)time(NULL));
    return 1;
}

int printGameHub(Level *level) {
    int height, width, row;
    Player *player;

    if (level == NULL || level->player == NULL) return 0;
    player = level->player;
    getmaxyx(stdscr, height, width);
    if (height < 2 || width < 1) return 0;

    row = height - 2;
    attron(COLOR_PAIR(COLOR_PAIR_STATUS) | A_DIM);
    mvhline(row, 0, ACS_HLINE, width);
    attroff(COLOR_PAIR(COLOR_PAIR_STATUS) | A_DIM);

    mvhline(height - 1, 0, ' ', width);
    if (has_colors()) attron(COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
    mvaddnstr(height - 1, 0, " LVL ", width);
    if (width >= 9) mvprintw(height - 1, 5, "%d", level->level);
    if (width >= 23) mvprintw(height - 1, 9, "  HP ");
    if (width >= 29) {
        if (has_colors()) attron(COLOR_PAIR(COLOR_PAIR_HEALTH));
        mvprintw(height - 1, 14, "%d/%d", player->health, player->maxHealth);
        if (has_colors()) attroff(COLOR_PAIR(COLOR_PAIR_HEALTH));
    }
    if (width >= 41) mvprintw(height - 1, 27, "  ATK %d", player->attack);
    if (width >= 55) mvprintw(height - 1, 37, "  DEF %d", player->defense);
    if (width >= 69) mvprintw(height - 1, 48, "  EXP %d", player->exp);
    if (width >= 82) mvprintw(height - 1, 62, "  GOLD %d", player->gold);
    if (has_colors()) attroff(COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);

    move(player->position->y, player->position->x);
    return 1;
}
