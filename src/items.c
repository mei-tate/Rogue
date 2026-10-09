#include "rogue.h"

static char itemSymbol(ItemType type) {
    switch (type) {
        case ITEM_POTION: return '!';
        case ITEM_MANA: return '?';
        case ITEM_WEAPON: return ')';
        case ITEM_ARMOR: return ']';
        case ITEM_GOLD: return '$';
        default: return ' ';
    }
}

int addItems(Level *level) {
    int capacity, desired;
    if (level == NULL || level->numOfRooms < 1) return 0;
    capacity = level->numOfRooms * 2;
    level->items = calloc((size_t)capacity, sizeof(*level->items));
    if (level->items == NULL) return 0;

    desired = 2 + level->level / 2;
    if (desired > capacity) desired = capacity;
    for (int itemIndex = 0; itemIndex < desired; itemIndex++) {
        int placed = 0;
        for (int attempt = 0; attempt < 100 && !placed; attempt++) {
            Room *room = level->rooms[rand() % level->numOfRooms];
            int x = room->position.x + 1 + rand() % (room->width - 2);
            int y = room->position.y + 1 + rand() % (room->height - 2);
            if ((mvinch(y, x) & A_CHARTEXT) != '.') continue;
            ItemType type;
            int roll = rand() % 100;
            if (roll < 40) type = ITEM_POTION;
            else if (roll < 58) type = ITEM_MANA;
            else if (roll < 75) type = ITEM_GOLD;
            else if (roll < 88) type = ITEM_WEAPON;
            else type = ITEM_ARMOR;
            int value = type == ITEM_GOLD ? 4 + rand() % (5 + level->level) : 1;
            level->items[level->numOfItems] = (Item){ {x, y}, type, value };
            level->numOfItems++;
            mvaddch(y, x, itemSymbol(type));
            placed = 1;
        }
    }
    return 1;
}

int collectItem(Level *level, Player *player, int x, int y) {
    if (level == NULL || player == NULL) return 0;
    for (int i = 0; i < level->numOfItems; i++) {
        Item *item = &level->items[i];
        if (item->type == ITEM_NONE || item->position.x != x || item->position.y != y)
            continue;
        if (item->type == ITEM_GOLD) {
            player->gold += item->value;
        } else {
            if (player->inventoryCount >= INVENTORY_CAPACITY) {
                mvprintw(0, 1, "Your pack is full. Press I to manage it.");
                return 0;
            }
            player->inventory[player->inventoryCount++] = *item;
        }
        item->type = ITEM_NONE;
        return 1;
    }
    return 0;
}
