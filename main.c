#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    unsigned int seed = (unsigned int)time(NULL);
    if (argc > 1) {
        seed = (unsigned int)atoi(argv[1]);
    }

    Game game;
    initGame(&game, seed);
    runGame(&game);

    return 0;
}
