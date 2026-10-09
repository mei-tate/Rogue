#include "rogue.h"

Player *playerSetUp(void) {
    Player *player = calloc(1, sizeof(*player));
    if (player == NULL) return NULL;
    player->position = calloc(1, sizeof(*player->position));
    if (player->position == NULL) {
        free(player);
        return NULL;
    }
    player->health = player->maxHealth = 20;
    player->attack = 2;
    player->speed = 1;
    player->defense = 1;
    player->rank = 1;
    player->mana = player->maxMana = 5;
    player->alive = 1;
    return player;
}

static int isMonsterTile(int tile) {
    return tile == 'X' || tile == 'G' || tile == 'T' || tile == 'D' || tile == 'W';
}

int handleInput(int input, Player *user, Level *level) {
    Position next;
    int tile;
    if (user == NULL || user->position == NULL || level == NULL || !user->alive) return 0;
    if (input == 'i' || input == 'I') {
        showInventory(user);
        return 0;
    }
    if (input == 'h' || input == 'H') return 0;
    if (input == 'r' || input == 'R') {
        if (user->mana < user->maxMana) user->mana++;
        moveMonsters(level, user);
        return 1;
    }

    next = *user->position;
    switch (input) {
        case 'w': case KEY_UP: next.y--; break;
        case 's': case KEY_DOWN: next.y++; break;
        case 'a': case KEY_LEFT: next.x--; break;
        case 'd': case KEY_RIGHT: next.x++; break;
        default: return 0;
    }
    int height, width;
    getmaxyx(stdscr, height, width);
    if (next.x < 0 || next.y < 0 || next.x >= width || next.y >= height - 2) return 0;
    tile = (int)(mvinch(next.y, next.x) & A_CHARTEXT);
    if (isMonsterTile(tile)) {
        checkPosition(next, user, level);
        // Combat resolves its own enemy responses in the battle window.
        return 1;
    }
    int moved = checkPosition(next, user, level);
    if (moved && !level->transitionRequested && user->alive)
        moveMonsters(level, user);
    return moved;
}

int checkPosition(Position position, Player *user, Level *level) {
    int height, width, tile;
    if (user == NULL || user->position == NULL || level == NULL || level->tiles == NULL) return 0;
    getmaxyx(stdscr, height, width);
    if (position.x < 0 || position.y < 0 || position.x >= width || position.y >= height - 2)
        return 0;
    tile = (int)(mvinch(position.y, position.x) & A_CHARTEXT);
    switch (tile) {
        case '.': case '#': case '+':
            movePlayer(position, user, level);
            return 1;
        case 'O':
            movePlayer(position, user, level);
            level->transitionRequested = 1;
            return 1;
        case '!': case '?': case ')': case ']': case '$':
            if (!collectItem(level, user, position.x, position.y)) return 0;
            movePlayer(position, user, level);
            return 1;
        default:
            if (isMonsterTile(tile)) {
                combat(user, getMonsterAt(&position, level), level);
                return 1;
            }
            return 0;
    }
}

int movePlayer(Position position, Player *user, Level *level) {
    if (user == NULL || user->position == NULL || level == NULL || level->tiles == NULL) return 0;
    mvaddch(user->position->y, user->position->x,
            level->tiles[user->position->y][user->position->x]);
    *user->position = position;
    mvaddch(position.y, position.x, '@');
    move(position.y, position.x);
    return 1;
}

static void removeInventoryItem(Player *player, int index) {
    for (int i = index; i + 1 < player->inventoryCount; i++)
        player->inventory[i] = player->inventory[i + 1];
    player->inventoryCount--;
}

int consumeItem(Player *player, ItemType type) {
    if (player == NULL) return 0;
    for (int i = 0; i < player->inventoryCount; i++) {
        Item *item = &player->inventory[i];
        if (item->type != type) continue;
        switch (type) {
            case ITEM_POTION:
                if (player->health >= player->maxHealth) return 0;
                player->health += 10;
                if (player->health > player->maxHealth) player->health = player->maxHealth;
                break;
            case ITEM_MANA:
                if (player->mana >= player->maxMana) return 0;
                player->mana += 4;
                if (player->mana > player->maxMana) player->mana = player->maxMana;
                break;
            case ITEM_WEAPON: player->attack += item->value; break;
            case ITEM_ARMOR: player->defense += item->value; break;
            default: return 0;
        }
        removeInventoryItem(player, i);
        return 1;
    }
    return 0;
}

static const char *itemName(ItemType type) {
    switch (type) {
        case ITEM_POTION: return "Healing draught    +10 health";
        case ITEM_MANA: return "Aether vial        +4 mana";
        case ITEM_WEAPON: return "Whetstone          +1 attack";
        case ITEM_ARMOR: return "Iron sigil         +1 defense";
        default: return "Unknown item";
    }
}

void showInventory(Player *player) {
    int height, width;
    if (player == NULL) return;
    getmaxyx(stdscr, height, width);
    int wh = height < 18 ? height - 2 : 18;
    int ww = width < 56 ? width - 2 : 56;
    if (wh < 8 || ww < 32) return;
    WINDOW *window = newwin(wh, ww, (height - wh) / 2, (width - ww) / 2);
    if (window == NULL) return;
    keypad(window, TRUE);
    int done = 0;
    while (!done) {
        werase(window);
        box(window, 0, 0);
        if (has_colors()) wattron(window, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
        mvwprintw(window, 1, 3, "THE PACK  %d/%d", player->inventoryCount, INVENTORY_CAPACITY);
        if (has_colors()) wattroff(window, COLOR_PAIR(COLOR_PAIR_STATUS) | A_BOLD);
        if (player->inventoryCount == 0) mvwprintw(window, 4, 3, "Empty. Search the rooms for supplies.");
        for (int i = 0; i < player->inventoryCount && i < wh - 5; i++)
            mvwprintw(window, 3 + i, 3, "%d. %s", (i + 1) % 10, itemName(player->inventory[i].type));
        wattron(window, A_DIM);
        mvwprintw(window, wh - 2, 2, "Number uses item  |  I or ESC closes");
        wattroff(window, A_DIM);
        wrefresh(window);
        int key = wgetch(window);
        if (key == 27 || key == 'i' || key == 'I' || key == 'q') done = 1;
        else if (key >= '1' && key <= '9') {
            int index = key - '1';
            if (index < player->inventoryCount)
                consumeItem(player, player->inventory[index].type);
        } else if (key == '0' && player->inventoryCount == 10) {
            consumeItem(player, player->inventory[9].type);
        }
    }
    delwin(window);
    touchwin(stdscr);
    refresh();
}

void awardExperience(Player *player, int amount) {
    if (player == NULL || amount < 1) return;
    player->exp += amount;
    while (player->exp >= player->rank * 10) {
        player->exp -= player->rank * 10;
        player->rank++;
        player->maxHealth += 4;
        player->health = player->maxHealth;
        player->attack++;
        player->maxMana++;
        player->mana = player->maxMana;
        mvprintw(0, 1, "You reach rank %d. Health and power restored.", player->rank);
    }
}
