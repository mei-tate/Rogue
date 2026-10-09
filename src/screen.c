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
        init_pair(COLOR_PAIR_HALL, COLOR_YELLOW, -1);
        init_pair(COLOR_PAIR_ITEM, COLOR_MAGENTA, -1);
        init_pair(COLOR_PAIR_MONSTER, COLOR_RED, -1);
        init_pair(COLOR_PAIR_PLAYER, COLOR_WHITE, -1);
    }

    srand((unsigned int)time(NULL));
    return 1;
}

int printGameHub(Level *level) {
    int height, width;
    Player *player;

    if (level == NULL || level->player == NULL) return 0;
    player = level->player;
    getmaxyx(stdscr, height, width);
    if (height < 2 || width < 1) return 0;

    erase();
    for (int y = 0; y < level->mapHeight && y < height - 2; y++) {
        for (int x = 0; x < width; x++) {
            char tile = level->tiles[y][x];
            int pair = 0;
            switch (tile) {
                case '#': pair = COLOR_PAIR_HALL; break;
                case 'O': pair = COLOR_PAIR_START; break;
                case '-': case '|': pair = COLOR_PAIR_STATUS; break;
                default: break;
            }
            if (pair && has_colors()) attron(COLOR_PAIR(pair));
            mvaddch(y, x, tile);
            if (pair && has_colors()) attroff(COLOR_PAIR(pair));
        }
    }
    for (int i = 0; i < level->numOfItems; i++) {
        Item *item = &level->items[i];
        if (item->type == ITEM_NONE) continue;
        char symbol = item->type == ITEM_POTION ? '!' :
            item->type == ITEM_MANA ? '?' : item->type == ITEM_WEAPON ? ')' :
            item->type == ITEM_ARMOR ? ']' : '$';
        if (has_colors()) attron(COLOR_PAIR(COLOR_PAIR_ITEM) | A_BOLD);
        mvaddch(item->position.y, item->position.x, symbol);
        if (has_colors()) attroff(COLOR_PAIR(COLOR_PAIR_ITEM) | A_BOLD);
    }
    for (int i = 0; i < level->numOfMonsters; i++) {
        Monster *monster = level->monsters[i];
        if (monster == NULL || !monster->alive) continue;
        if (has_colors()) attron(COLOR_PAIR(COLOR_PAIR_MONSTER) | A_BOLD);
        mvaddch(monster->position->y, monster->position->x, monster->symbol);
        if (has_colors()) attroff(COLOR_PAIR(COLOR_PAIR_MONSTER) | A_BOLD);
    }
    if (has_colors()) attron(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);
    mvaddch(player->position->y, player->position->x, '@');
    if (has_colors()) attroff(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);

    attron(COLOR_PAIR(COLOR_PAIR_STATUS) | A_DIM);
    mvhline(height - 2, 0, ACS_HLINE, width);
    attroff(COLOR_PAIR(COLOR_PAIR_STATUS) | A_DIM);
    mvhline(height - 1, 0, ' ', width);
    if (has_colors()) attron(COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
    mvprintw(height - 2, 1, " The Hollow Crown  |  Floor %d/%d", level->level, MAX_GAME_LEVELS);
    if (width >= 96) {
        mvprintw(height - 1, 1, "HP %d/%d  MP %d/%d  ATK %d  DEF %d  Rank %d  XP %d  Gold %d  Pack %d/%d  I:pack R:rest H:help Q:menu",
                 player->health, player->maxHealth, player->mana, player->maxMana,
                 player->attack, player->defense, player->rank, player->exp,
                 player->gold, player->inventoryCount, INVENTORY_CAPACITY);
    } else {
        mvprintw(height - 1, 1, "HP%d/%d MP%d/%d ATK%d DEF%d R%d XP%d $%d P%d/%d",
                 player->health, player->maxHealth, player->mana, player->maxMana,
                 player->attack, player->defense, player->rank, player->exp,
                 player->gold, player->inventoryCount, INVENTORY_CAPACITY);
    }
    if (has_colors()) attroff(COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);

    move(player->position->y, player->position->x);
    return 1;
}
