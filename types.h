#ifndef TYPES_H
#define TYPES_H

#define PLAYERS 4
#define MAX_ROUNDS 500
#define SQUARES 40
#define START_CASH 30000
#define BID_INCREMENT 250

typedef enum stratergyType{
        AGGRESSIVE_INVESTOR,
        CONSERVATIVE_BANKER,
        RISK_TAKER,
        OPPORTUNISTIC_TRADER
} stratType;

//Player attributes
typedef struct Player{
	char name[24];
	char strategy;
	int cash;
	char pos;
	char InJail;
	char railutil;
    char properties;
    stratType strat;
//	struct Loan;
} plyr;

typedef enum sqrType{
	EMPTY = -1,
	START,
	PROPERTY,
	RAILWAY,
	UTILITY,
	EVENT,
	SPECIAL,
	INSURANCE,
	BANK,
    TAX
} sqrTyp;

typedef enum propertyGroup{
	NONE = -1,
	BROWN,
	LIGHT_BLUE,
	PINK,
	ORANGE,
	RED,
	YELLOW,
	GREEN,
	DARK_BLUE,
	JAIL,
	GO_TO_JAIL,
	EVENT_CARD,
	CD_FUND
}sqrgrp;

typedef enum EconomicState{
    STABLE,
    BOOM,
    MID_I,
    HIGH_I,
    RECESSION,
} econ;

typedef struct Square{
	sqrTyp type;
	char name[42];
	sqrgrp group;
	int buyPrice;
	int baseRent;
	int houseCost;
	int hotelCost;
	plyr *owner;
	char houses;
} sqr;

typedef struct Board{
	sqr s[40];
} bd;

typedef struct Game{
    sqr square[SQUARES];
    plyr player[PLAYERS];
    int currRound;
    econ econState;
} game;

typedef struct HighBid{
    int value;
    plyr *bidder;
}bid;

int dice(char *duble);
game init_game(int n, char **arr);
void start_game(plyr *plyrs, int n, int cash);
plyr spawn_player(char n);
void run_game(int N, int n, game g);
void landing_action(plyr *p, sqr *s, int roll, game *g);
int canBuy(plyr *p, sqr *s);
int net_worth(plyr *p, game *g);
void auction(sqr *s, game *g);
int decide_bid(plyr *p, sqr *s, int nextbid);
char bankrupt(plyr p);
#endif
