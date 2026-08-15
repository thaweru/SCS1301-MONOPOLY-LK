#ifndef PLAYERS_H
#define PLAYERS_H

#include "types.h"

void initPlayers(Player players[NUM_PLAYERS]);

int playerDecidePurchase(Game *game, int playerId, int squareIndex);
int playerDecideAuctionBid(Game *game, int playerId, const Auction *auction);
int playerDecideTakeLoan(Game *game, int playerId);
int playerDecideRepayLoan(Game *game, int playerId);
InsuranceType playerDecideInsurance(Game *game, int playerId, int propertyIndex);
void playerPerformTurnMaintenance(Game *game, int playerId);
void playerPerformTurnConstruction(Game *game, int playerId);
void playerPerformTurnRenovation(Game *game, int playerId);

#endif /* PLAYERS_H */
