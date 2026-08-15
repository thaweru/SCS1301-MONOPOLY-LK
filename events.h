#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

void initEvents(Game *game);
void checkDynamicMarket(Game *game);
void checkRegionalCard(Game *game);
void checkGlobalEconomicEvent(Game *game);
void checkGovernmentRegulation(Game *game);
void updateRoundEvents(Game *game);
void drawNationalEventCard(Game *game, int playerId);

#endif /* EVENTS_H */
