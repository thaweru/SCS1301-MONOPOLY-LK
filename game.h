#ifndef GAME_H
#define GAME_H

#include "types.h"

void initGame(Game *game, unsigned int seed);
void determineTurnOrder(Game *game);
void runGame(Game *game);
void triggerAuction(Game *game, int propertyIndex);
void declareBankruptcy(Game *game, int playerId);
void printRoundSummary(const Game *game);
void printMarketConditions(const Game *game);
void printGameOverSummary(const Game *game);

#endif /* GAME_H */
