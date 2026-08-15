#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 40
#define NUM_PLAYERS 4
#define INITIAL_CASH 30000
#define GO_REWARD 2000
#define MAX_ROUNDS 500
#define JAIL_BAIL 300
#define JAIL_SQUARE 10
#define GO_TO_JAIL_SQUARE 30
#define BANK_SQUARE 38
#define INCOME_TAX_SQUARE 4
#define CDF_TAX_SQUARE 2
#define SRI_LANKA_INS_SQUARE 17
#define CEYLINCO_INS_SQUARE 33

#define NUM_NATIONAL_CARDS 20
#define NUM_REGIONAL_CARDS 12
#define NUM_GLOBAL_EVENTS 8
#define NUM_GOV_REGULATIONS 8
#define NUM_PROPERTY_GROUPS 8

/* Square Types */
typedef enum {
    SQ_START,
    SQ_PROPERTY,
    SQ_RAILWAY,
    SQ_UTILITY,
    SQ_BANK,
    SQ_INSURANCE,
    SQ_EVENT,
    SQ_TAX,
    SQ_SPECIAL
} SquareType;

/* Property Groups (8 colour groups) */
typedef enum {
    GRP_BROWN = 0,
    GRP_LIGHT_BLUE = 1,
    GRP_PINK = 2,
    GRP_ORANGE = 3,
    GRP_RED = 4,
    GRP_YELLOW = 5,
    GRP_GREEN = 6,
    GRP_DARK_BLUE = 7,
    GRP_NONE = -1
} PropertyGroup;

/* Geographic Regions for Regional Development */
typedef enum {
    REG_COLOMBO_COMMERCIAL,
    REG_WESTERN_COASTAL,
    REG_WESTERN_RESIDENTIAL,
    REG_WESTERN_AIRPORT,
    REG_CENTRAL_HILL,
    REG_SOUTHERN_PROVINCE,
    REG_NORTHERN_PROVINCE,
    REG_EASTERN_PROVINCE,
    REG_PRIME_LUXURY,
    REG_NONE
} RegionType;

/* Insurance Policy Types */
typedef enum {
    INS_BASIC = 0,
    INS_COMPREHENSIVE = 1,
    INS_BUSINESS_INTERRUPTION = 2,
    INS_NONE = -1
} InsuranceType;

/* Strategy Types */
typedef enum {
    STRAT_AGGRESSIVE = 0,
    STRAT_CONSERVATIVE = 1,
    STRAT_RISK_TAKER = 2,
    STRAT_OPPORTUNISTIC = 3
} StrategyType;

/* Disaster Types */
typedef enum {
    DISASTER_FIRE,
    DISASTER_FLOOD,
    DISASTER_RIOT,
    DISASTER_BUILDING_COLLAPSE,
    DISASTER_ELECTRICAL_FAILURE
} DisasterType;

/* Global Economic Events (Rule-LK 18) */
typedef enum {
    GLOB_TOURISM_BOOM,
    GLOB_FUEL_CRISIS,
    GLOB_HEAVY_MONSOON,
    GLOB_ECONOMIC_RECESSION,
    GLOB_STOCK_MARKET_BOOM,
    GLOB_GOV_HOUSING_PROG,
    GLOB_FOREIGN_INVESTMENT,
    GLOB_POLITICAL_UNREST,
    GLOB_NONE = -1
} GlobalEventType;

/* Government Regulations (Rule-LK 24) */
typedef enum {
    GOV_INCREASE_PROP_TAX,
    GOV_REDUCE_LOAN_INTEREST,
    GOV_HOUSING_SUBSIDY,
    GOV_LUXURY_PROP_TAX,
    GOV_RAILWAY_MODERNIZATION,
    GOV_ELECTRICITY_TARIFF,
    GOV_INSURANCE_REGULATION,
    GOV_ANTI_SPECULATION,
    GOV_NONE = -1
} GovRegulationType;

/* Insurance Policy Struct */
typedef struct {
    InsuranceType type;
    int propertyIndex;
    int premium;
    int compensationRate;  /* 80 or 100 % */
    int startRound;
    int duration;          /* 20 rounds */
    int active;
    int expiryNotified;
} InsurancePolicy;

/* Commercial Loan Struct */
typedef struct {
    int active;
    int principal;
    int outstandingBalance;
    int interestRate;      /* percentage e.g. 8 */
    int duration;          /* default 20 rounds */
    int roundsElapsed;
    int numCollateral;
    int collateralIndices[BOARD_SIZE];
} Loan;

/* Square / Property Struct */
typedef struct {
    int index;
    char name[64];
    SquareType type;
    PropertyGroup group;
    RegionType region;
    int basePurchasePrice;
    int baseRental;
    int baseMortgageValue;
    int baseHouseCost;
    int baseHotelCost;

    int ownerId;           /* -1 = Bank / Unowned */
    int mortgaged;         /* 1 if mortgaged to Bank (Rule 7, forfeits rent) */
    int loanLocked;        /* 1 if pledged as loan collateral (Rule-LK 3, keeps rent) */
    int numHouses;         /* 0..4 */
    int hasHotel;          /* 0 or 1 */
    int buildingCondition; /* 100 down to 0 */
    int consecutiveNeglectedRounds; /* counter for >20 neglected maintenance */
    int structuralDamage;  /* 1 if structurally damaged */
    int age;               /* rounds since last renovation (Rule-LK 15) */
    int isDamaged;         /* 1 if damaged by disaster pending repair */
    int closedTurns;       /* turns closed due to event */
    InsurancePolicy insurance;
} Square;

/* Forward declaration of Game struct */
struct Game;

/* Player Struct */
typedef struct {
    int id;
    char name[64];
    StrategyType strategy;
    int cash;
    int position;
    int inJail;
    int jailTurns;
    int bankrupt;
    int bankruptRound;
    Loan loan;
    int sufferedLossRecently;       /* for Risk Taker reactive insurance */
    int undevelopablePurchasedRound[BOARD_SIZE]; /* for Anti-Speculation Act */
    int activeNationalCard;         /* active National Card ID or -1 */
    int nationalCardRoundsLeft;
    int pendingInsuranceClaim;      /* claim amount payable */
} Player;

/* Dynamic Market Group State */
typedef struct {
    int boomRoundsLeft;
    int declineRoundsLeft;
    int cooldownRoundsLeft;         /* 30 rounds cooldown after boom/decline */
} MarketGroupState;

/* Regional Development Card Struct */
typedef struct {
    int id;
    char name[64];
    char description[128];
    RegionType targetRegion;
    PropertyGroup targetGroup;      /* or GRP_NONE */
    int valueModifierPct;
    int rentModifierPct;
    int constrModifierPct;
} RegionalCard;

/* National Event Card Struct */
typedef struct {
    int id;
    char name[64];
    char description[128];
} NationalCard;

/* Global Economic Event Struct */
typedef struct {
    GlobalEventType type;
    char name[64];
    char description[128];
} GlobalEvent;

/* Government Regulation Struct */
typedef struct {
    GovRegulationType type;
    char name[64];
    char description[128];
} GovRegulation;

/* Economy State */
typedef struct {
    int currentInflationRate;       /* -3, 0, 2, 5, 8, 12 */
    double cumulativeInflationMultiplier;
    int currentBaseLoanInterest;    /* base loan rate e.g. 8 */
    MarketGroupState groupMarket[NUM_PROPERTY_GROUPS];
    int activeRegionalCardId;
    int regionalCardRoundsLeft;
    GlobalEventType activeGlobalEvent;
    int globalEventRoundsLeft;
    GovRegulationType activeGovRegulation;
    int govRegulationRoundsLeft;
    int lastBoomGroup;
    int lastDeclineGroup;
} EconomyState;

/* Auction Struct */
typedef struct {
    int propertyIndex;
    int openingBid;
    int currentBid;
    int highestBidderId;
    int activeParticipants[NUM_PLAYERS];
} Auction;

/* Overall Game State */
typedef struct Game {
    Square board[BOARD_SIZE];
    Player players[NUM_PLAYERS];
    int turnOrder[NUM_PLAYERS];
    int currentRound;
    int totalRoundsRun;
    EconomyState economy;
    int nationalDeck[NUM_NATIONAL_CARDS];
    int nationalDeckTop;
    RegionalCard regionalDeck[NUM_REGIONAL_CARDS];
    GlobalEvent globalEvents[NUM_GLOBAL_EVENTS];
    GovRegulation govRegulations[NUM_GOV_REGULATIONS];
    int isGameOver;
    int winnerId;
} Game;

#endif /* TYPES_H */
