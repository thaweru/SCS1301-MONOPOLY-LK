#ifndef BOARD_H
#define BOARD_H

#include "types.h"

void initBoard(Square board[BOARD_SIZE]);
int countPropertiesInGroup(PropertyGroup grp);
int countOwnedInGroup(const Square board[BOARD_SIZE], PropertyGroup grp, int playerId);
int hasMonopoly(const Square board[BOARD_SIZE], PropertyGroup grp, int playerId);
int countStationsOwned(const Square board[BOARD_SIZE], int playerId);
int countUtilitiesOwned(const Square board[BOARD_SIZE], int playerId);
const char* getGroupName(PropertyGroup grp);
const char* getRegionName(RegionType reg);

#endif /* BOARD_H */
