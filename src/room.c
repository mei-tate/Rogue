#include "rogue.h"

Room *createRoom (int y, int x, int height, int width){
    Room *newRoom;
    newRoom = malloc(sizeof(Room));
    if (newRoom == NULL) {
        return NULL;
    }

    newRoom->position.y = y;
    newRoom->position.x = x;
    newRoom->height = height;
    newRoom->width = width;
    
    // Randomly generate doors
    newRoom->doors = malloc(sizeof(Position) * 4);
    if (newRoom->doors == NULL) {
        free(newRoom);
        return NULL;
    }

    // Top door
    newRoom->doors[0].x = rand() % (width - 2) + newRoom->position.x + 1;
    newRoom->doors[0].y = newRoom->position.y;
    // Left door
    newRoom->doors[1].y = rand() % (height - 2) + newRoom->position.y + 1;
    newRoom->doors[1].x = newRoom->position.x;

    // Bottom door
    newRoom->doors[2].x = rand() % (width - 2) + newRoom->position.x + 1;
    newRoom->doors[2].y = newRoom->position.y + newRoom->height - 1;

    // Right door
    newRoom->doors[3].y = rand() % (height - 2) + newRoom->position.y + 1;
    newRoom->doors[3].x = newRoom->position.x + newRoom->width - 1;

    return newRoom;
};

int drawRoom(Room *room){
    int x;
    int y;

    // Draw top and bottom
    for (x = room->position.x; x < room->position.x + room->width; x++){
        mvprintw(room->position.y, x, "-"); // Top
        mvprintw(room->position.y + room->height - 1, x, "-"); // Bottom
    }

    // Draw floor and side walls
    for (y = room->position.y+1; y < room->position.y + room->height - 1; y++){
        // Draw side walls
        mvprintw(y, room->position.x, "|");
        mvprintw(y, room->position.x + room->width - 1, "|");

        for (x = room->position.x + 1; x < room->position.x + room->width-1; x++){
            // Draw floors
            mvprintw(y, x, ".");
        }
    }

    // Draw Doors
    mvprintw(room->doors[0].y, room->doors[0].x, "+");
    mvprintw(room->doors[1].y, room->doors[1].x, "+");
    mvprintw(room->doors[2].y, room->doors[2].x, "+");
    mvprintw(room->doors[3].y, room->doors[3].x, "+");

    return 1;
};


// Find a shortest route through empty cells/hallways. Walls and room interiors block it.
int connectDoors(Position *door1, Position *door2){
    int height, width, cellCount, start, target, head = 0, tail = 0;
    int *queue = NULL, *parent = NULL;
    int found = 0;
    static const int dx[] = {1, -1, 0, 0};
    static const int dy[] = {0, 0, 1, -1};

    if (door1 == NULL || door2 == NULL) return 0;
    getmaxyx(stdscr, height, width);
    if (door1->x < 0 || door1->x >= width || door1->y < 0 || door1->y >= height ||
        door2->x < 0 || door2->x >= width || door2->y < 0 || door2->y >= height) {
        return 0;
    }

    cellCount = height * width;
    start = door1->y * width + door1->x;
    target = door2->y * width + door2->x;
    queue = malloc((size_t)cellCount * sizeof(*queue));
    parent = malloc((size_t)cellCount * sizeof(*parent));
    if (queue == NULL || parent == NULL) goto cleanup;

    for (int i = 0; i < cellCount; i++) parent[i] = -1;
    parent[start] = start;
    queue[tail++] = start;

    // Breadth-first search: only blank cells and existing hallways may be crossed.
    while (head < tail && !found) {
        int current = queue[head++];
        int x = current % width;
        int y = current / width;

        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            int next;
            chtype tile;
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
            next = ny * width + nx;
            if (parent[next] != -1) continue;
            tile = mvinch(ny, nx) & A_CHARTEXT;
            if (next != target && tile != ' ' && tile != '#') continue;
            parent[next] = current;
            queue[tail++] = next;
            if (next == target) { found = 1; break; }
        }
    }

    if (found) {
        int current = target;
        // Trace the route back to the start and draw every intermediate cell.
        while (current != start) {
            int x = current % width;
            int y = current / width;
            if (current != target) mvaddch(y, x, '#');
            current = parent[current];
        }
    }

cleanup:
    free(queue);
    free(parent);
    return found;
}


// Free memory
void freeRooms(Room **rooms, int roomCount){
    int x;
    if (rooms == NULL) {
        return;
    }
    for (x = 0; x < roomCount; x++){
        if (rooms[x] == NULL) {
            continue;
        }
        free(rooms[x]->doors);
        free(rooms[x]);
    }

    free(rooms);
}
