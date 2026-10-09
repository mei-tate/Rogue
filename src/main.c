#include "rogue.h"
#include "mainMenu.h"

int main(void) {
    char *choices[] = {"Descend into the Hollow", "How to Play", "Leave"};
    Game game = {0};

    if (!screenSetUp()) return EXIT_FAILURE;

    for (;;) {
        int choice = mainMenu(3, choices);
        if (choice == START_GAME) {
            gameLoop(&game);
        } else if (choice == SHOW_HELP) {
            showHelpScreen();
        } else {
            break;
        }
    }

    endwin();
    return EXIT_SUCCESS;
}
