#include "rogue.h"

void render(Level *level) {
    if (level == NULL) return;
    printGameHub(level);
    refresh();
}

static int placePlayerInFirstRoom(Level *level, Player *player) {
    Room *room;
    if (level == NULL || player == NULL || player->position == NULL ||
        level->numOfRooms == 0) return 0;
    room = level->rooms[0];
    for (int y = room->position.y + 1; y < room->position.y + room->height - 1; y++) {
        for (int x = room->position.x + 1; x < room->position.x + room->width - 1; x++) {
            int occupied = 0;
            for (int i = 0; i < level->numOfMonsters; i++) {
                Monster *monster = level->monsters[i];
                if (monster != NULL && monster->alive &&
                    monster->position->x == x && monster->position->y == y) {
                    occupied = 1;
                    break;
                }
            }
            for (int i = 0; i < level->numOfItems; i++) {
                Item *item = &level->items[i];
                if (item->type != ITEM_NONE && item->position.x == x && item->position.y == y) {
                    occupied = 1;
                    break;
                }
            }
            if (!occupied) {
                player->position->x = x;
                player->position->y = y;
                mvaddch(y, x, '@');
                return 1;
            }
        }
    }
    return 0;
}

static void showFloorStory(int floor) {
    static const char *stories[MAX_GAME_LEVELS] = {
        "The crown pulses beneath the sealed stair.",
        "A voice in stone counts each footstep.",
        "Dead kings left their armories unlocked.",
        "Something vast breathes below.",
        "Wraiths whisper your name in the dark.",
        "The Ash Dragon sleeps upon the crown."
    };
    int height, width;
    getmaxyx(stdscr, height, width);
    int wh = height < 12 ? height - 2 : 12;
    int ww = width < 60 ? width - 2 : 60;
    if (wh < 7 || ww < 30) return;
    WINDOW *window = newwin(wh, ww, (height - wh) / 2, (width - ww) / 2);
    if (window == NULL) return;
    box(window, 0, 0);
    if (has_colors()) wattron(window, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
    mvwprintw(window, 2, 3, "FLOOR %d  |  THE HOLLOW CROWN", floor);
    if (has_colors()) wattroff(window, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
    mvwaddnstr(window, 5, 3, stories[floor - 1], ww - 6);
    wattron(window, A_DIM);
    mvwprintw(window, wh - 2, 3, "Press any key to enter the depths.");
    wattroff(window, A_DIM);
    wrefresh(window);
    wgetch(window);
    delwin(window);
    touchwin(stdscr);
}

void gameLoop(Game *game) {
    int key, completed = 0;
    Level *level;
    Player *player;

    if (game == NULL) return;
    for (int i = 0; i < MAX_GAME_LEVELS; i++) game->levels[i] = NULL;
    clear();
    refresh();
    level = createLevel(1);
    if (level == NULL) {
        mvprintw(1, 2, "Could not create the level. Press any key to return.");
        refresh();
        getch();
        return;
    }
    showFloorStory(1);

    player = playerSetUp();
    if (player == NULL) {
        freeLevel(level);
        mvprintw(1, 2, "Could not create the player. Press any key to return.");
        refresh();
        getch();
        return;
    }

    if (!placePlayerInFirstRoom(level, player)) {
        free(player->position);
        free(player);
        freeLevel(level);
        return;
    }

    level->player = player;
    game->levels[0] = level;
    game->currentLevel = 1;
    render(level);

    while (player->alive && !completed) {
        key = getch();
        if (key == 'q' || key == 'Q') break;
        if (key == 'h' || key == 'H') {
            showHelpScreen();
            render(level);
            continue;
        }
        handleInput(key, player, level);
        if (level->transitionRequested) {
            if (level->level == MAX_GAME_LEVELS) {
                completed = 1;
            } else {
                int nextNumber = level->level + 1;
                level->player = NULL;
                clear();
                refresh();
                level = createLevel(nextNumber);
                if (level == NULL) {
                    mvprintw(1, 2, "Could not create the next level. Press any key.");
                    refresh();
                    getch();
                    break;
                }
                showFloorStory(nextNumber);
                level->player = player;
                game->levels[nextNumber - 1] = level;
                game->currentLevel = nextNumber;
                if (!placePlayerInFirstRoom(level, player)) break;
            }
        }
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

    if (completed) {
        int height, width;
        getmaxyx(stdscr, height, width);
        if (height > 2 && width > 35) {
            mvprintw(height - 3, 2, "You cleared all six levels. Press any key.");
            refresh();
            getch();
        }
    }

    free(player->position);
    free(player);
    for (int i = 0; i < MAX_GAME_LEVELS; i++) {
        freeLevel(game->levels[i]);
        game->levels[i] = NULL;
    }
    game->currentLevel = 0;
    clear();
    refresh();
}
