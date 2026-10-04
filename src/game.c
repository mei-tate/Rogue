#include "rogue.h"

void render(Level *level) {
    if (level == NULL) return;
    printGameHub(level);
    refresh();
}

void gameLoop(Game *game) {
    int key;
    Level *level;
    Player *player;

    if (game == NULL) return;
    clear();
    refresh();
    level = createLevel(1);
    if (level == NULL) {
        mvprintw(1, 2, "Could not create the level. Press any key to return.");
        refresh();
        getch();
        return;
    }

    player = playerSetUp();
    if (player == NULL) {
        freeLevel(level);
        mvprintw(1, 2, "Could not create the player. Press any key to return.");
        refresh();
        getch();
        return;
    }

    level->player = player;
    game->levels[0] = level;
    game->currentLevel = 1;
    render(level);

    while (player->alive) {
        key = getch();
        if (key == 'q' || key == 'Q') break;
        if (key == 'h' || key == 'H') {
            showHelpScreen();
            render(level);
            continue;
        }
        handleInput(key, player, level);
        render(level);
    }

    if (!player->alive) {
        int height, width;
        getmaxyx(stdscr, height, width);
        if (height > 2 && width > 18) {
            if (has_colors()) attron(A_BOLD | COLOR_PAIR(COLOR_PAIR_END));
            mvprintw(height - 3, 2, "You have fallen. Press any key.");
            if (has_colors()) attroff(A_BOLD | COLOR_PAIR(COLOR_PAIR_END));
            refresh();
            getch();
        }
    }

    free(player->position);
    free(player);
    freeLevel(level);
    game->levels[0] = NULL;
    game->currentLevel = 0;
    clear();
    refresh();
}
