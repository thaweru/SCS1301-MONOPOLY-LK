#include "players.h"
#include "board.h"
#include "finance.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initPlayers(Player players[NUM_PLAYERS]) {
    const char *names[NUM_PLAYERS] = {
        "Aggressive Investor",
        "Conservative Banker",
        "Risk Taker",
        "Opportunistic Trader"
    };

    for (int i = 0; i < NUM_PLAYERS; i++) {
        players[i].id = i;
        strncpy(players[i].name, names[i], sizeof(players[i].name));
        players[i].strategy = (StrategyType)i;
        players[i].cash = INITIAL_CASH;
        players[i].position = 0;
        players[i].inJail = 0;
        players[i].jailTurns = 0;
        players[i].bankrupt = 0;
        players[i].bankruptRound = -1;
        players[i].loan.active = 0;
        players[i].loan.principal = 0;
        players[i].loan.outstandingBalance = 0;
        players[i].loan.interestRate = 0;
        players[i].loan.duration = 20;
        players[i].loan.roundsElapsed = 0;
        players[i].loan.numCollateral = 0;
        players[i].sufferedLossRecently = 0;
        players[i].activeNationalCard = -1;
        players[i].nationalCardRoundsLeft = 0;
        players[i].pendingInsuranceClaim = 0;

        for (int b = 0; b < BOARD_SIZE; b++) {
            players[i].undevelopablePurchasedRound[b] = 0;
        }
    }
}

/* Purchase Decision on landing on an unowned property */
int playerDecidePurchase(Game *game, int playerId, int squareIndex) {
    Player *p = &game->players[playerId];
    Square *sq = &game->board[squareIndex];
    if (sq->ownerId >= 0) return 0;

    int price = getPropertyMarketValue(game, squareIndex);
    if (p->cash < price) return 0;

    /* Anti-Speculation Act Check */
    if (game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_ANTI_SPECULATION) {
        int undevelopCount = 0;
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (game->board[i].ownerId == playerId && game->board[i].type == SQ_PROPERTY &&
                game->board[i].numHouses == 0 && !game->board[i].hasHotel) {
                undevelopCount++;
            }
        }
        if (undevelopCount >= 3 && sq->type == SQ_PROPERTY) {
            return 0; /* capped at 3 */
        }
    }

    switch (p->strategy) {
        case STRAT_AGGRESSIVE:
            /* Always purchases if at least one future rent payment remains affordable (~500 LKR) */
            if (p->cash - price >= 500) return 1;
            return (p->cash >= price);

        case STRAT_CONSERVATIVE:
            /* Purchases a property only if >=50% of current cash remains afterward */
            if (game->economy.globalEventRoundsLeft > 0 && game->economy.activeGlobalEvent == GLOB_ECONOMIC_RECESSION) {
                return 0; /* avoids new investment during recession */
            }
            if (p->cash - price >= (p->cash / 2)) return 1;
            return 0;

        case STRAT_RISK_TAKER:
            /* Purchases every available property whenever legally possible */
            return (p->cash >= price);

        case STRAT_OPPORTUNISTIC:
            /* Buys only when market value is strong and cash is healthy */
            if (p->cash - price >= 2000) return 1;
            return 0;

        default:
            return (p->cash >= price);
    }
}

/* Auction Bid Decision */
int playerDecideAuctionBid(Game *game, int playerId, const Auction *auction) {
    Player *p = &game->players[playerId];
    int minNextBid = (auction->currentBid == 0) ? auction->openingBid : (auction->currentBid + 250);
    if (p->cash < minNextBid) return 0;

    int marketVal = getPropertyMarketValue(game, auction->propertyIndex);

    switch (p->strategy) {
        case STRAT_AGGRESSIVE:
            /* Bids aggressively up to 120% of estimated market value */
            if (minNextBid <= (int)(marketVal * 1.20) && minNextBid <= p->cash) {
                return minNextBid;
            }
            return 0;

        case STRAT_CONSERVATIVE:
            /* Bids only when price is below market value and keeps 50% cash reserve */
            if (minNextBid < marketVal && (p->cash - minNextBid >= p->cash / 2)) {
                return minNextBid;
            }
            return 0;

        case STRAT_RISK_TAKER:
            /* Bids aggressively in every auction up to available cash */
            if (minNextBid <= p->cash) {
                return minNextBid;
            }
            return 0;

        case STRAT_OPPORTUNISTIC:
            /* Prefers discounted auction buys: bids up to 90% of market value */
            if (minNextBid <= (int)(marketVal * 0.90) && (p->cash - minNextBid >= 1000)) {
                return minNextBid;
            }
            return 0;

        default:
            if (minNextBid <= marketVal && minNextBid <= p->cash) {
                return minNextBid;
            }
            return 0;
    }
}

/* Loan Decisions at Bank */
int playerDecideTakeLoan(Game *game, int playerId) {
    Player *p = &game->players[playerId];
    if (p->loan.active) return 0;

    int eligible[BOARD_SIZE];
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        Square *sq = &game->board[i];
        if (sq->ownerId == playerId && !sq->mortgaged && !sq->loanLocked &&
            (sq->type == SQ_PROPERTY || sq->type == SQ_RAILWAY || sq->type == SQ_UTILITY)) {
            eligible[count++] = i;
        }
    }
    if (count == 0) return 0;

    int shouldBorrow = 0;
    switch (p->strategy) {
        case STRAT_AGGRESSIVE:
            /* Takes loans when funds would increase rental income (e.g. has monopoly or low cash) */
            if (p->cash < 5000) shouldBorrow = 1;
            break;
        case STRAT_CONSERVATIVE:
            /* Avoids loans unless bankruptcy imminent */
            if (p->cash < 1000) shouldBorrow = 1;
            break;
        case STRAT_RISK_TAKER:
            /* Always borrows maximum loan permitted */
            shouldBorrow = 1;
            break;
        case STRAT_OPPORTUNISTIC:
            /* Takes loan only when return exceeds borrowing cost */
            if (p->cash < 3000) shouldBorrow = 1;
            break;
    }

    if (shouldBorrow) {
        return takeLoan(game, playerId, eligible, count);
    }
    return 0;
}

int playerDecideRepayLoan(Game *game, int playerId) {
    Player *p = &game->players[playerId];
    if (!p->loan.active) return 0;

    switch (p->strategy) {
        case STRAT_AGGRESSIVE:
            /* Repays once excess cash exceeds 2x outstanding loan */
            if (p->cash >= p->loan.outstandingBalance * 2) {
                return repayLoan(game, playerId, p->loan.outstandingBalance);
            }
            break;
        case STRAT_CONSERVATIVE:
            /* Repays immediately whenever visiting Bank with sufficient funds */
            if (p->cash >= p->loan.outstandingBalance) {
                return repayLoan(game, playerId, p->loan.outstandingBalance);
            } else if (p->cash > 2000) {
                return repayLoan(game, playerId, p->cash - 2000);
            }
            break;
        case STRAT_RISK_TAKER:
            /* Refinances rather than repaying */
            if (p->loan.roundsElapsed > 10) {
                refinanceLoan(game, playerId);
            }
            break;
        case STRAT_OPPORTUNISTIC:
            if (p->cash >= p->loan.outstandingBalance + 5000) {
                return repayLoan(game, playerId, p->loan.outstandingBalance);
            }
            break;
    }
    return 0;
}

/* Insurance Decision on landing on Insurance square */
InsuranceType playerDecideInsurance(Game *game, int playerId, int propertyIndex) {
    Player *p = &game->players[playerId];
    Square *sq = &game->board[propertyIndex];
    if (sq->ownerId != playerId || sq->insurance.active) return INS_NONE;

    switch (p->strategy) {
        case STRAT_AGGRESSIVE:
            /* Basic for houses, Comprehensive for hotels */
            if (sq->hasHotel) return INS_COMPREHENSIVE;
            if (sq->numHouses > 0) return INS_BASIC;
            return INS_NONE;

        case STRAT_CONSERVATIVE:
            /* Always Comprehensive for every developed property */
            if (sq->numHouses > 0 || sq->hasHotel) return INS_COMPREHENSIVE;
            return INS_BASIC;

        case STRAT_RISK_TAKER:
            /* Reactive: only after suffering a loss */
            if (p->sufferedLossRecently && (sq->numHouses > 0 || sq->hasHotel)) {
                p->sufferedLossRecently = 0;
                return INS_BASIC;
            }
            return INS_NONE;

        case STRAT_OPPORTUNISTIC:
            /* Comprehensive for high-value developments */
            if (sq->hasHotel || sq->group == GRP_DARK_BLUE || sq->group == GRP_GREEN || sq->group == GRP_YELLOW) {
                return INS_COMPREHENSIVE;
            }
            return INS_NONE;
    }
    return INS_NONE;
}

/* Maintenance at Start of Turn */
void playerPerformTurnMaintenance(Game *game, int playerId) {
    Player *p = &game->players[playerId];
    for (int i = 0; i < BOARD_SIZE; i++) {
        Square *sq = &game->board[i];
        if (sq->ownerId == playerId && (sq->numHouses > 0 || sq->hasHotel)) {
            if (sq->structuralDamage) {
                repairStructuralDamage(game, playerId, i);
            } else {
                int threshold = 90;
                if (p->strategy == STRAT_RISK_TAKER) threshold = 50;
                else if (p->strategy == STRAT_OPPORTUNISTIC) threshold = 75;

                if (sq->buildingCondition < threshold) {
                    performBuildingMaintenance(game, playerId, i);
                }
            }
        }
    }
}

/* Construction on Monopolies during turn */
void playerPerformTurnConstruction(Game *game, int playerId) {
    Player *p = &game->players[playerId];

    /* Check each property group */
    for (int g = 0; g < NUM_PROPERTY_GROUPS; g++) {
        if (!hasMonopoly(game->board, (PropertyGroup)g, playerId)) continue;

        /* Check all properties in group are unmortgaged */
        int groupIndices[3];
        int count = 0;
        int anyMortgaged = 0;
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (game->board[i].type == SQ_PROPERTY && game->board[i].group == g) {
                groupIndices[count++] = i;
                if (game->board[i].mortgaged) anyMortgaged = 1;
            }
        }
        if (anyMortgaged || count == 0) continue;

        /* Strategy-specific constraints */
        if (p->strategy == STRAT_CONSERVATIVE) {
            if (game->economy.globalEventRoundsLeft > 0 && game->economy.activeGlobalEvent == GLOB_ECONOMIC_RECESSION) {
                continue; /* avoids investing in recession */
            }
        }
        if (p->strategy == STRAT_OPPORTUNISTIC) {
            if (game->economy.currentInflationRate > 5 &&
                !(game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_HOUSING_SUBSIDY)) {
                continue; /* delays construction during high inflation */
            }
        }

        /* Build houses / upgrade hotels evenly */
        int keepBuilding = 1;
        while (keepBuilding) {
            keepBuilding = 0;

            /* Find property in group with lowest development */
            int minHouses = 5;
            int targetIdx = -1;

            for (int k = 0; k < count; k++) {
                int idx = groupIndices[k];
                int dev = game->board[idx].hasHotel ? 5 : game->board[idx].numHouses;
                if (dev < minHouses) {
                    minHouses = dev;
                    targetIdx = idx;
                }
            }

            if (targetIdx < 0 || minHouses >= 5) break;

            Square *targetSq = &game->board[targetIdx];

            if (minHouses < 4) {
                /* Build House */
                int cost = getHouseConstructionCost(game, targetIdx);
                int minReserve = 1000;
                if (p->strategy == STRAT_CONSERVATIVE) minReserve = 5000;
                else if (p->strategy == STRAT_AGGRESSIVE) minReserve = 200;
                else if (p->strategy == STRAT_RISK_TAKER) minReserve = 0;

                if (p->cash >= cost + minReserve) {
                    p->cash -= cost;
                    targetSq->numHouses++;
                    printf("\n%s constructed one house on %s.\n", p->name, targetSq->name);
                    printf("Construction Cost : LKR %d.\n", cost);
                    keepBuilding = 1;
                }
            } else if (minHouses == 4) {
                /* Upgrade to Hotel */
                if (p->strategy == STRAT_CONSERVATIVE && p->loan.active) {
                    /* Conservative never builds hotel while loan is outstanding */
                    break;
                }
                int cost = getHotelConstructionCost(game, targetIdx);
                int minReserve = 1000;
                if (p->strategy == STRAT_CONSERVATIVE) minReserve = 5000;
                else if (p->strategy == STRAT_AGGRESSIVE) minReserve = 200;
                else if (p->strategy == STRAT_RISK_TAKER) minReserve = 0;

                if (p->cash >= cost + minReserve) {
                    p->cash -= cost;
                    targetSq->numHouses = 0;
                    targetSq->hasHotel = 1;
                    printf("\n%s upgraded %s to a Hotel.\n", p->name, targetSq->name);
                    printf("Construction Cost : LKR %d.\n", cost);
                    keepBuilding = 1;
                }
            }
        }
    }
}

/* Renovation for Age-based Depreciation during turn */
void playerPerformTurnRenovation(Game *game, int playerId) {
    Player *p = &game->players[playerId];
    for (int i = 0; i < BOARD_SIZE; i++) {
        Square *sq = &game->board[i];
        if (sq->ownerId == playerId && sq->type == SQ_PROPERTY) {
            int deprThreshold = 30;
            if (p->strategy == STRAT_CONSERVATIVE) deprThreshold = 10;
            else if (p->strategy == STRAT_OPPORTUNISTIC) deprThreshold = 15;
            else if (p->strategy == STRAT_AGGRESSIVE) deprThreshold = 20;

            if (sq->age > (50 + deprThreshold * 5)) {
                renovateForDepreciation(game, playerId, i);
            }
        }
    }
}
