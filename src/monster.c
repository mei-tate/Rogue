#include "rogue.h"

int addMonster(Level *level){
    int x;
    if (level == NULL) return 0;
    level->monsters = calloc((size_t)level->numOfRooms, sizeof(*level->monsters));
    if (level->monsters == NULL) return 0;
    level->numOfMonsters = 0;

    for (x = 0; x < level->numOfRooms; x++){
        if ((rand() % 2) == 0){
            Monster *monster = selectMonster(level->level);
            if (monster == NULL || !setStartingPosition(monster, level->rooms[x])) {
                if (monster != NULL) {
                    free(monster->position);
                    free(monster);
                }
                return 0;
            }
            monster->room = level->rooms[x];
            level->monsters[level->numOfMonsters] = monster;
            level->numOfMonsters++;
        }
    }

    // Every level needs at least one monster so the exit can appear on a death.
    if (level->numOfMonsters == 0) {
        Monster *monster = selectMonster(level->level);
        if (monster == NULL || !setStartingPosition(monster, level->rooms[0])) {
            if (monster != NULL) {
                free(monster->position);
                free(monster);
            }
            return 0;
        }
        monster->room = level->rooms[0];
        level->monsters[level->numOfMonsters++] = monster;
    }

    return 1;
}

Monster *selectMonster(int level){
    int monster;
    switch (level){
        case 1:
        case 2:
        case 3:
            monster = (rand() % 2) + 1; // Random number between 1 and 2
            break;
        case 4:
        case 5:
            monster = (rand() % 2) + 2; // Random number between 2 and 3
            break;
        case 6:
            monster = 4;
            break;
        default:
            monster = 4;
            break;
    }
    /*
    1 Spider
        symbol: X
        levels: 1-3
        health: 2
        attack: 1
        speed: 1
        defence: 1
        pathfinding: 0 (random)

    2 Goblin
        symbol: G
        levels: 1-5
        health: 5
        attack: 3
        speed: 1
        defence: 1
        pathfinding: 1 (seeking)

    3 Troll
        symbol: T
        levels: 4-5
        health: 15
        attack: 5
        speed: 1
        defence: 1
        pathfinding: 0 (random)

    3 Dragon
        symbol: D
        levels: 6
        health: 15
        attack: 5
        speed: 2
        defence: 4
        pathfinding: 1 (seeking)

    */
    switch (monster){
        case 1:
        // Spider
            return createMonster('X',2,1,1,1,0);
        case 2:
        // Goblin
            return createMonster('G',5,3,1,1,1);
        case 3:
        // Troll
            return createMonster('T',10,5,1,1,0);  
        case 4:
        // Dragon
            return createMonster('D',15,5,2,4,1);
        default:
            break;
        }
    
    return NULL;
}


Monster *createMonster(char symbol, int health, int attack, int speed, int defense, int pathfinding){
    Monster *newMonster;
    newMonster = malloc(sizeof(Monster));
    if (newMonster == NULL) return NULL;

    newMonster->position = malloc(sizeof(*newMonster->position));
    if (newMonster->position == NULL) {
        free(newMonster);
        return NULL;
    }

    newMonster->symbol = symbol;
    newMonster->health = health;
    newMonster->attack = attack;
    newMonster->speed = speed;
    newMonster->defense = defense;
    newMonster->pathfinding = pathfinding;
    newMonster->alive = 1;
    return newMonster;
}

int killMonster(Monster *monster){
    if (monster == NULL || monster->position == NULL) return 0;
    mvaddch(monster->position->y, monster->position->x, '.');
    monster->alive = 0;
    return 1;
}


int setStartingPosition(Monster *monster, Room *room){
    if (monster == NULL || monster->position == NULL || room == NULL) return 0;
    monster->position->x = (rand() % (room->width - 2)) + room->position.x + 1;
    monster->position->y = (rand() % (room->height - 2)) + room->position.y + 1;

    mvaddch(monster->position->y, monster->position->x, monster->symbol);

    return 1;
}

static int insideRoom(const Room *room, int x, int y) {
    return room != NULL && x > room->position.x &&
        x < room->position.x + room->width - 1 &&
        y > room->position.y && y < room->position.y + room->height - 1;
}

static int occupiedByMonster(const Level *level, const Monster *self, int x, int y) {
    for (int i = 0; i < level->numOfMonsters; i++) {
        Monster *other = level->monsters[i];
        if (other != NULL && other != self && other->alive &&
            other->position->x == x && other->position->y == y) return 1;
    }
    return 0;
}

static int canMonsterEnter(const Level *level, const Player *player,
                           const Monster *monster, int x, int y) {
    chtype tile;
    if (!insideRoom(monster->room, x, y) ||
        (player->position->x == x && player->position->y == y) ||
        occupiedByMonster(level, monster, x, y)) return 0;
    tile = mvinch(y, x) & A_CHARTEXT;
    return tile == '.';
}

static void takeStep(Level *level, Player *player, Monster *monster,
                     int nextX, int nextY) {
    int oldX = monster->position->x;
    int oldY = monster->position->y;
    if (!canMonsterEnter(level, player, monster, nextX, nextY)) return;
    mvaddch(oldY, oldX, level->tiles[oldY][oldX]);
    monster->position->x = nextX;
    monster->position->y = nextY;
    mvaddch(nextY, nextX, monster->symbol);
}

static void moveRandomly(Level *level, Player *player, Monster *monster) {
    static const int dx[] = {0, 0, -1, 1, 0};
    static const int dy[] = {-1, 1, 0, 0, 0};
    int direction = rand() % 5;
    takeStep(level, player, monster,
             monster->position->x + dx[direction],
             monster->position->y + dy[direction]);
}

static void moveTowardPlayer(Level *level, Player *player, Monster *monster) {
    int dx = player->position->x - monster->position->x;
    int dy = player->position->y - monster->position->y;
    // Try the larger gap first; fallback lets the monster move around blockers.
    if (abs(dx) >= abs(dy)) {
        if (dx != 0) takeStep(level, player, monster,
            monster->position->x + (dx > 0 ? 1 : -1), monster->position->y);
        if (monster->position->x == player->position->x && dy != 0)
            takeStep(level, player, monster, monster->position->x,
                monster->position->y + (dy > 0 ? 1 : -1));
    } else {
        if (dy != 0) takeStep(level, player, monster, monster->position->x,
            monster->position->y + (dy > 0 ? 1 : -1));
        if (monster->position->y == player->position->y && dx != 0)
            takeStep(level, player, monster,
                monster->position->x + (dx > 0 ? 1 : -1), monster->position->y);
    }
    // Try the other axis if the preferred step was blocked.
    if (dx != 0 && dy != 0 && monster->position->x == player->position->x - dx &&
        monster->position->y == player->position->y - dy) {
        takeStep(level, player, monster,
            monster->position->x + (abs(dx) >= abs(dy) ? 0 : (dx > 0 ? 1 : -1)),
            monster->position->y + (abs(dx) >= abs(dy) ? (dy > 0 ? 1 : -1) : 0));
    }
}

int moveMonsters(Level *level, Player *player) {
    if (level == NULL || player == NULL || player->position == NULL) return 0;
    for (int i = 0; i < level->numOfMonsters; i++) {
        Monster *monster = level->monsters[i];
        if (monster == NULL || !monster->alive) continue;
        if (insideRoom(monster->room, player->position->x, player->position->y))
            moveTowardPlayer(level, player, monster);
        else
            moveRandomly(level, player, monster);
    }
    return 1;
}

Monster *getMonsterAt(Position *position, Level *level) {
    if (position == NULL || level == NULL) return NULL;
    for (int i = 0; i < level->numOfMonsters; i++) {
        Monster *monster = level->monsters[i];
        if (monster != NULL && monster->alive &&
            position->x == monster->position->x &&
            position->y == monster->position->y) return monster;
    }
    return NULL;
}
