#include "rogue.h"
#include "mainMenu.h"

int main(void) {
    char *choices[] = {"Start Game", "End Game"};
    Game game = {0};

    if (!screenSetUp()) return EXIT_FAILURE;

    for (;;) {
        int choice = mainMenu(2, choices);
        if (choice == START_GAME) {
            gameLoop(&game);
        } else {
            break;
        }
    }

    endwin();
    return EXIT_SUCCESS;
}
