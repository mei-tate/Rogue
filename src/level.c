#include "rogue.h"

Level *createLevel(int level){
    Level *newLevel;
    int mapWidth;
    newLevel = malloc(sizeof(Level));
    if (newLevel == NULL) {
        return NULL;
    }
    newLevel->level = level;
    newLevel->numOfRooms = 3;
    newLevel->numOfMonsters = 0;
    newLevel->monsters = NULL;
    newLevel->player = NULL;
    newLevel->tiles = NULL;
    newLevel->rooms = NULL;
    getmaxyx(stdscr, newLevel->mapHeight, mapWidth);
    (void)mapWidth;
    newLevel->rooms = roomSetUp();

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

Room **roomSetUp(){
    int x;
    Room **rooms;
    rooms = malloc(sizeof(Room*) * 3);
    if (rooms == NULL) {
        return NULL;
    }

    rooms[0] = createRoom(13, 13, 6, 8);
    rooms[1] = createRoom(2, 40, 6, 8);
    rooms[2] = createRoom(10, 40, 6, 12);

    if (rooms[0] == NULL || rooms[1] == NULL || rooms[2] == NULL) {
        freeRooms(rooms, 3);
        return NULL;
    }
    
    for (x = 0; x < 3; x++){
        drawRoom(rooms[x]);
    }

    connectDoors(&rooms[0]->doors[3], &rooms[2]->doors[1]);
    connectDoors(&rooms[1]->doors[2], &rooms[0]->doors[0]);

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
