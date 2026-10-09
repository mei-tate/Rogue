#include "rogue.h"

Player *playerSetUp(){
    Player *newPlayer;
    newPlayer = malloc(sizeof(Player));
    if (newPlayer == NULL) return NULL;

    newPlayer->position = malloc(sizeof(*newPlayer->position));
    if (newPlayer->position == NULL) {
        free(newPlayer);
        return NULL;
    }

    newPlayer->position->x = 14;
    newPlayer->position->y = 14;
    newPlayer->health = 20;
    newPlayer->maxHealth = 20;
    newPlayer->attack = 1;
    newPlayer->speed = 1;
    newPlayer->defense = 0;
    newPlayer->gold = 0;
    newPlayer->exp = 0;
    newPlayer->alive = 1;

    return newPlayer;
}

int handleInput(int input, Player *user, Level *level){
    Position newPosition;
    if (user == NULL || user->position == NULL || level == NULL || !user->alive)
        return 0;
    newPosition = *user->position;

    switch (input){
        // Move Up
        case 'w':
            newPosition.y--;
            break;

        // Move Down
        case 's':
            newPosition.y++;
            break;

        // Move Left
        case 'a':
            newPosition.x--;
            break;

        // Move Right
        case 'd':
            newPosition.x++;
            break;

        default:
            return 0;
    }

    checkPosition(newPosition, user, level);
    moveMonsters(level, user);

    return 1;
}

// Check what is at the next position
int checkPosition(Position position, Player *user, Level *level){
    int height;
    int width;
    
    if (user == NULL || user->position == NULL || level == NULL || level->tiles == NULL) {
        return 0;
    }
    getmaxyx(stdscr, height, width);
    if (position.x < 0 || position.y < 0 || position.x >= width || position.y >= height) {
        return 0;
    }

    switch (mvinch(position.y, position.x) & A_CHARTEXT){
        case '.':
        case '#':
        case '+':
            movePlayer(position, user, level);
            break;
        case 'O':
            movePlayer(position, user, level);
            level->transitionRequested = 1;
            break;
        case 'X':
        case 'G':
        case 'T':
        case 'D':
            combat(user, getMonsterAt(&position, level), level);
            break;
        default:
            break;
    };

    return 1;
};

// Redraw Player in a new given position
int movePlayer(Position position, Player *user, Level *level){
    if (user == NULL || user->position == NULL || level == NULL || level->tiles == NULL)
        return 0;
    mvaddch(user->position->y, user->position->x,
            level->tiles[user->position->y][user->position->x]);

    *user->position = position;

    mvaddch(user->position->y, user->position->x, '@');
    move(user->position->y, user->position->x);

    return 1;
};
