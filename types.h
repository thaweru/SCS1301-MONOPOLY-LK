#ifndef TYPES_H
#define TYPES_H

#define PLAYERS 4
#define MAX_ROUNDS 10
#define SQUARES 40
#define START_CASH 30000
#define BID_INCREMENT 250
#define PASS_GO_CASH 2000
#define MAX_RENT 2000
#define INC_TAX 15
#define COM_DEV_TAX 10

typedef enum PlyrStrat{
    AGGRESSIVE_INVESTOR,
    CONSERVATIVE_BANKER,
    RISK_TAKER,
    OPPORTUNISTIC_TRADER
} plyrstrat;

typedef struct Square sqr;

//Player attributes
typedef struct Player{
	char name[24];
	plyrstrat strat;
	int cash;
    int income;
	char pos;
	char InJail;
	char railutil;
    char properties;
	sqr *lastBuy;
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
	char houses;
	plyr *owner;
	sqr *prevBuy;
	sqr *nextofgroup;
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

int dice(char *duble);
game init_game(int n, char **arr);
void start_game(plyr *plyrs, int n, int cash);
plyr spawn_player(char n);
void run_game(int N, int n, game g);
void landing_action(plyr *p, sqr *s, int roll, game *g);
int canBuy(plyr *p, sqr *s);
int net_worth(plyr *p, game *g);
void auction(sqr *s, game *g);
int auction_bid(plyr p, sqr s, int nextBid);
int projected_appriciation(sqr *s);
char isMonopoly(game *g, plyr *p, sqr *s);
char canBuild(game *g, sqr *s);
char winner_of_game(game g);
int total_assets(plyr *p, game *g);
void find_cash(plyr *p, int dues);
#endif
