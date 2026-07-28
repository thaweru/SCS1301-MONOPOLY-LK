#ifndef TYPES_H
#define TYPES_H


//Main structure of the program
struct Game{
	struct Player player[4];
	struct Board board;
	int currentRound = 0;
	struct EconomicState economy;
};

void startGame();
void runRound();
char isGameOver();
struct Player determineWinner();

//Player attributes
struct Player{
	char name[];
	int cash;
	int position;
	char InJail;
	int jailTurns;
	struct Property property[];
	struct Loan loan;
	struct InsurancePolicy[] policies;
	struct PlayerStratergy stratergy;
};

void takeTurn();
void purchaseProperty(struct Property);
void payRent(int);
void declareBankrupt();
int calculateNetWorth();

//Board attributes
struct Board{
	struct Square square[40];
};

void initializeBoard();
struct Square getSquare(int);

struct Square{
	int index;
	char name[];
	union SquareType type;
};

struct Property{
	int purchasePrice;
	int morgageValue;
	int baseRental;
	int houseCost;
	int hotelCost;
	enum PropertyGroup group;
	struct Player owner;
	char morgaged;
	int numHouses;
	char hasHotel;
	int condition;
	int age;
};

int calculateRent(int);
void construct();
void morgage();
void renovate();
#endif
