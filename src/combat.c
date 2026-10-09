#include "rogue.h"

static const char *monsterName(char symbol) {
    switch (symbol) {
        case 'X': return "Cave spider";
        case 'G': return "Goblin raider";
        case 'T': return "Cave troll";
        case 'D': return "The Ash Dragon";
        case 'W': return "Wraith";
        default: return "Dungeon creature";
    }
}

static void drawBattle(WINDOW *window, Player *player, Monster *monster,
                       const char *message) {
    int h, w;
    getmaxyx(window, h, w);
    werase(window);
    box(window, 0, 0);
    if (has_colors()) wattron(window, COLOR_PAIR(COLOR_PAIR_END) | A_BOLD);
    mvwprintw(window, 1, 3, "ENCOUNTER: %s", monsterName(monster->symbol));
    if (has_colors()) wattroff(window, COLOR_PAIR(COLOR_PAIR_END) | A_BOLD);
    mvwhline(window, 2, 2, ACS_HLINE, w - 4);
    mvwprintw(window, 4, 4, "You     HP %d/%d   MP %d/%d   ATK %d   DEF %d",
              player->health, player->maxHealth, player->mana, player->maxMana,
              player->attack, player->defense);
    mvwprintw(window, 6, 4, "%s  HP %d   ATK %d   DEF %d",
              monsterName(monster->symbol), monster->health, monster->attack, monster->defense);
    if (message != NULL) mvwaddnstr(window, h - 7, 3, message, w - 6);
    if (has_colors()) wattron(window, COLOR_PAIR(COLOR_PAIR_START) | A_BOLD);
    mvwprintw(window, h - 5, 3, "A  Attack       S  Arcane strike (2 MP)");
    mvwprintw(window, h - 4, 3, "P  Use a potion  F  Flee (55%% chance)");
    if (has_colors()) wattroff(window, COLOR_PAIR(COLOR_PAIR_START) | A_BOLD);
    wattron(window, A_DIM);
    mvwprintw(window, h - 2, 3, "Each action gives the enemy a chance to strike.");
    wattroff(window, A_DIM);
    wrefresh(window);
}

static void damagePlayer(Player *player, Monster *monster) {
    int damage = monster->attack - player->defense;
    if (damage < 1) damage = 1;
    player->health -= damage;
    if (player->health <= 0) {
        player->health = 0;
        player->alive = 0;
    }
}

static void damageMonster(Monster *monster, int damage) {
    damage -= monster->defense;
    if (damage < 1) damage = 1;
    monster->health -= damage;
    if (monster->health <= 0) {
        monster->health = 0;
        killMonster(monster);
    }
}

static void openExit(Level *level, Monster *lastMonster) {
    if (level == NULL || lastMonster == NULL) return;
    for (int i = 0; i < level->numOfMonsters; i++)
        if (level->monsters[i] != NULL && level->monsters[i]->alive) return;
    int x = lastMonster->position->x, y = lastMonster->position->y;
    level->tiles[y][x] = 'O';
    mvaddch(y, x, 'O');
}

int combat(Player *player, Monster *monster, Level *level) {
    int height, width, done = 0, escaped = 0;
    const char *message = "The dungeon stirs around you.";
    if (player == NULL || monster == NULL || level == NULL ||
        !player->alive || !monster->alive) return 0;
    getmaxyx(stdscr, height, width);
    int wh = height < 18 ? height - 2 : 18;
    int ww = width < 58 ? width - 2 : 58;
    if (wh < 14 || ww < 42) return 0;
    WINDOW *window = newwin(wh, ww, (height - wh) / 2, (width - ww) / 2);
    if (window == NULL) return 0;
    keypad(window, TRUE);

    while (!done && player->alive && monster->alive) {
        drawBattle(window, player, monster, message);
        int key = wgetch(window);
        message = "";
        if (key == 'a' || key == 'A' || key == '1') {
            damageMonster(monster, player->attack);
            if (!monster->alive) {
                message = "The creature falls. You gain experience and gold.";
                player->gold += 2 + rand() % (3 + level->level);
                awardExperience(player, 4 + level->level * 2);
                openExit(level, monster);
                done = 1;
            } else message = "Your weapon strikes true.";
        } else if (key == 's' || key == 'S' || key == '2') {
            if (player->mana < 2) {
                message = "Not enough mana. Rest to recover a point.";
                continue;
            }
            player->mana -= 2;
            damageMonster(monster, player->attack + 4);
            if (!monster->alive) {
                message = "Arcane force scatters the creature. You gain a reward.";
                player->gold += 2 + rand() % (3 + level->level);
                awardExperience(player, 4 + level->level * 2);
                openExit(level, monster);
                done = 1;
            } else message = "Your arcane strike tears into the enemy.";
        } else if (key == 'p' || key == 'P' || key == '3') {
            if (!consumeItem(player, ITEM_POTION)) {
                message = "No usable healing draught in your pack.";
                continue;
            }
            message = "You drink a healing draught.";
        } else if (key == 'f' || key == 'F' || key == 27) {
            if (rand() % 100 < 55) {
                escaped = 1;
                done = 1;
                message = "You break away from the fight.";
            } else message = "The enemy blocks your escape.";
        } else continue;

        if (!done && monster->alive && player->alive) {
            damagePlayer(player, monster);
            if (player->alive) message = "The enemy hits you.";
            else message = "You fall beneath the enemy's attack.";
        }
    }
    drawBattle(window, player, monster, message);
    if (!player->alive || !escaped) wgetch(window);
    delwin(window);
    touchwin(stdscr);
    refresh();
    return 1;
}
