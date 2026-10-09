#ifndef ROGUE_H
#define ROGUE_H

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define COLOR_PAIR_START 1
#define COLOR_PAIR_END 2
#define COLOR_PAIR_HIGHLIGHT 3
#define COLOR_PAIR_STATUS 4
#define COLOR_PAIR_HEALTH 5
#define COLOR_PAIR_HALL 6
#define COLOR_PAIR_ITEM 7
#define COLOR_PAIR_MONSTER 8
#define COLOR_PAIR_PLAYER 9
#define MAX_GAME_LEVELS 6
#define INVENTORY_CAPACITY 10

/************ Struct Definitions ***************/

typedef struct Position {
    int x;
    int y;
} Position;

typedef enum ItemType {
    ITEM_NONE,
    ITEM_POTION,
    ITEM_MANA,
    ITEM_WEAPON,
    ITEM_ARMOR,
    ITEM_GOLD
} ItemType;

typedef struct Item {
    Position position;
    ItemType type;
    int value;
} Item;

typedef struct Level{
    char **tiles;
    int numOfRooms;
    int level;
    struct Room **rooms;
    struct Monster **monsters;
    int numOfMonsters;
    Item *items;
    int numOfItems;
    int mapHeight;
    int transitionRequested;
    struct Player *player;
} Level;

typedef struct Player {
    Position * position;
    int health;
    int attack;
    int speed;
    int defense;
    int gold;
    int maxHealth;
    int exp;
    int rank;
    int mana;
    int maxMana;
    int inventoryCount;
    Item inventory[INVENTORY_CAPACITY];
    int alive;
    // Room * room;
} Player;

typedef struct Monster {
    Position *position;
    char symbol;
    int health;
    int attack;
    int speed;
    int defense;
    int pathfinding;
    int alive;
    struct Room *room;
} Monster;

typedef struct Room {
    Position position;
    int height;
    int width;

    Position *doors;
    // Monster **monsters;
    // Item **items;
} Room;


/************* Global Variables *************/
// screen functions
int screenSetUp(void);
int printGameHub(Level *level);
void showHelpScreen(void);

// Level and Map Setup Functions
Level *createLevel(int level);
char **saveLevelPositions();
Room **roomSetUp(int level, int mapHeight, int mapWidth, int *roomCount);
void freeLevel(Level *level);
void freeLevelPositions(char **level, int height);

// Player Functions
Player *playerSetUp();
int handleInput(int input, Player *user, Level *level);
int movePlayer(Position position, Player *user, Level *level);
int checkPosition(Position position, Player *user, Level *level);
void showInventory(Player *player);
int collectItem(Level *level, Player *player, int x, int y);
int consumeItem(Player *player, ItemType type);
void awardExperience(Player *player, int amount);


// Room functions
Room *createRoom (int y, int x, int height, int width);
int drawRoom(Room *room);
int connectDoors(Position *door1, Position *door2);
void freeRooms(Room **rooms, int roomCount);


// Monster functions
int addMonster(Level *level);
int addItems(Level *level);
Monster *selectMonster(int level);
Monster *createMonster(char symbol, int health, int attack, int speed, int defense, int pathfinding);
int setStartingPosition(Monster *monster, Room *room);
int moveMonsters(Level *level, Player *player);
Monster *getMonsterAt(Position *position, Level *level);
int killMonster(Monster *monster);


// Combat Functions
int combat(Player *player, Monster *monster, Level *level);


// Game 
typedef struct Game
{
    struct Level * levels[MAX_GAME_LEVELS];
    int currentLevel;
} Game;

void render(Level *level);
void gameLoop(Game * game);

#endif
