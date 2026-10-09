#include "rogue.h"

Level *createLevel(int level){
    Level *newLevel;
    int mapWidth, screenHeight;
    if (level < 1 || level > MAX_GAME_LEVELS) return NULL;
    newLevel = malloc(sizeof(Level));
    if (newLevel == NULL) {
        return NULL;
    }
    newLevel->level = level;
    newLevel->numOfRooms = 0;
    newLevel->numOfMonsters = 0;
    newLevel->monsters = NULL;
    newLevel->player = NULL;
    newLevel->tiles = NULL;
    newLevel->rooms = NULL;
    getmaxyx(stdscr, screenHeight, mapWidth);
    newLevel->mapHeight = screenHeight;
    newLevel->transitionRequested = 0;
    newLevel->rooms = roomSetUp(level, screenHeight - 2, mapWidth,
                                &newLevel->numOfRooms);

    if (newLevel->rooms == NULL) {
        free(newLevel);
        return NULL;
    }
    newLevel->tiles = saveLevelPositions();
    if (newLevel->tiles == NULL) {
        freeRooms(newLevel->rooms, newLevel->numOfRooms);
        free(newLevel);
        return NULL;
    }

    if (!addMonster(newLevel)) {
        freeLevel(newLevel);
        return NULL;
    }
    return newLevel;
};

Room **roomSetUp(int level, int mapHeight, int mapWidth, int *roomCount){
    int count, columns, rows, slotWidth, slotHeight;
    Room **rooms;
    if (roomCount == NULL || level < 1 || level > MAX_GAME_LEVELS) return NULL;
    *roomCount = 0;
    if (level <= 2) count = 3 + rand() % 2;
    else if (level <= 4) count = 4 + rand() % 3;
    else count = 6;

    columns = count <= 4 ? 2 : 3;
    rows = (count + columns - 1) / columns;
    slotWidth = (mapWidth - 2) / columns;
    slotHeight = (mapHeight - 2) / rows;
    if (slotWidth < 9 || slotHeight < 7) return NULL;

    rooms = calloc((size_t)count, sizeof(*rooms));
    if (rooms == NULL) {
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        int row = i / columns, column = i % columns;
        int maxWidth = slotWidth - 1;
        int maxHeight = slotHeight - 1;
        int width, height, x, y;
        if (maxWidth > 12) maxWidth = 12;
        if (maxHeight > 8) maxHeight = 8;
        width = 8 + rand() % (maxWidth - 7);
        height = 6 + rand() % (maxHeight - 5);
        x = 1 + column * slotWidth + rand() % (slotWidth - width + 1);
        y = 1 + row * slotHeight + rand() % (slotHeight - height + 1);
        rooms[i] = createRoom(y, x, height, width);
        if (rooms[i] == NULL) {
            freeRooms(rooms, count);
            return NULL;
        }
        drawRoom(rooms[i]);
    }

    // Connect rooms in a chain, which guarantees that every room is reachable.
    // Each connection uses its own doors, and the pathfinder avoids existing halls.
    int usedDoors[6][4] = {{0}};
    for (int i = 1; i < count; i++) {
        int connected = 0;
        int offset = rand() % 16;
        for (int attempt = 0; attempt < 16 && !connected; attempt++) {
            int pair = (offset + attempt) % 16;
            int firstDoor = pair / 4;
            int secondDoor = pair % 4;
            if (usedDoors[i - 1][firstDoor] || usedDoors[i][secondDoor]) continue;
            if (connectDoors(&rooms[i - 1]->doors[firstDoor],
                             &rooms[i]->doors[secondDoor])) {
                usedDoors[i - 1][firstDoor] = 1;
                usedDoors[i][secondDoor] = 1;
                connected = 1;
            }
        }
        if (!connected) {
            freeRooms(rooms, count);
            return NULL;
        }
    }

    *roomCount = count;
    return rooms;
};




char **saveLevelPositions(){
    int x;
    int y;
    int height;
    int width;
    char **positions;

    getmaxyx(stdscr, height, width);
    positions = malloc(sizeof(char*) * (size_t) height);
    if (positions == NULL) {
        return NULL;
    }

    for (y = 0; y < height; y++){
        positions[y] = malloc(sizeof(char) * (size_t) width);
        if (positions[y] == NULL) {
            while (y > 0) {
                free(positions[--y]);
            }
            free(positions);
            return NULL;
        }
        for (x = 0; x < width; x++){
            positions[y][x] = (char) (mvinch(y, x) & A_CHARTEXT);
        }
    }
    return positions;
}


void freeLevelPositions(char **level, int height) {
    int y;

    if (level == NULL) {
        return;
    }

    for (y = 0; y < height; y++) {
        free(level[y]);
    }

    free(level);
}

void freeLevel(Level *level) {
    if (level == NULL) {
        return;
    }
    freeLevelPositions(level->tiles, level->mapHeight);
    freeRooms(level->rooms, level->numOfRooms);
    for (int i = 0; i < level->numOfMonsters; i++) {
        if (level->monsters[i] != NULL) {
            free(level->monsters[i]->position);
            free(level->monsters[i]);
        }
    }
    free(level->monsters);
    free(level);
}
