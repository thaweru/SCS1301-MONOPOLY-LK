#ifndef TYPES_H
#define TYPES_H


struct game{
	struct players Player[4];
	struct board Board;
	int currentRound = 0;
	struct economy EconomicState;
};

#endif
