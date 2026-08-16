#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Forward declaration for external triggers */
extern void triggerAuction(Game *game, int propertyIndex);
extern void declareBankruptcy(Game *game, int playerId);

/* Calculate Property Market Value with all active modifiers */
int getPropertyMarketValue(const Game *game, int propertyIndex) {
    const Square *sq = &game->board[propertyIndex];
    if (sq->type != SQ_PROPERTY && sq->type != SQ_RAILWAY && sq->type != SQ_UTILITY) {
        return 0;
    }

    double baseVal = (double)sq->basePurchasePrice;
    /* 1. Inflation */
    baseVal *= game->economy.cumulativeInflationMultiplier;

    /* 2. Dynamic Market Modifier */
    if (sq->group >= 0 && sq->group < NUM_PROPERTY_GROUPS) {
        if (game->economy.groupMarket[sq->group].boomRoundsLeft > 0) {
            baseVal *= 1.20; /* +20% property value */
        } else if (game->economy.groupMarket[sq->group].declineRoundsLeft > 0) {
            baseVal *= 0.85; /* -15% property value */
        }
    }

    /* 3. Regional Card Modifier */
    if (game->economy.activeRegionalCardId >= 0 && game->economy.regionalCardRoundsLeft > 0) {
        const RegionalCard *card = &game->regionalDeck[game->economy.activeRegionalCardId];
        if (card->targetRegion == sq->region || (card->targetGroup != GRP_NONE && card->targetGroup == sq->group)) {
            baseVal *= (1.0 + (double)card->valueModifierPct / 100.0);
        }
    }

    /* 4. Global Economic Event Modifier */
    if (game->economy.globalEventRoundsLeft > 0) {
        switch (game->economy.activeGlobalEvent) {
            case GLOB_TOURISM_BOOM:
                if (sq->region == REG_SOUTHERN_PROVINCE || sq->region == REG_WESTERN_COASTAL) {
                    baseVal *= 1.15;
                }
                break;
            case GLOB_ECONOMIC_RECESSION:
                baseVal *= 0.85;
                break;
            case GLOB_STOCK_MARKET_BOOM:
                baseVal *= 1.15;
                break;
            case GLOB_FOREIGN_INVESTMENT:
                if (sq->region == REG_COLOMBO_COMMERCIAL || sq->region == REG_PRIME_LUXURY) {
                    baseVal *= 1.25;
                }
                break;
            default:
                break;
        }
    }

    /* 5. Age-based Depreciation (Rule-LK 16) */
    if (sq->age > 50) {
        int deprSteps = (sq->age - 50) / 5;
        int deprPct = deprSteps;
        if (deprPct > 30) deprPct = 30;
        baseVal *= (1.0 - (double)deprPct / 100.0);
    }

    /* 6. Structural Damage (Rule-LK 28) */
    if (sq->structuralDamage) {
        baseVal *= 0.85; /* -15% property value */
    }

    if (baseVal < 100.0) baseVal = 100.0;
    return (int)baseVal;
}

/* Calculate Total Property Market Value of a player's owned properties (excluding buildings) */
int calculateTotalPropertyMarketValue(const Game *game, int playerId) {
    int total = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (game->board[i].ownerId == playerId && game->board[i].type == SQ_PROPERTY) {
            total += getPropertyMarketValue(game, i);
        }
    }
    return total;
}

/* Calculate Building Value */
int getBuildingValue(const Game *game, int propertyIndex) {
    const Square *sq = &game->board[propertyIndex];
    if (sq->type != SQ_PROPERTY) return 0;

    int houseCost = getHouseConstructionCost(game, propertyIndex);
    int hotelCost = getHotelConstructionCost(game, propertyIndex);

    if (sq->hasHotel) {
        return hotelCost + (4 * houseCost);
    } else {
        return sq->numHouses * houseCost;
    }
}

/* Construction Costs */
int getHouseConstructionCost(const Game *game, int propertyIndex) {
    const Square *sq = &game->board[propertyIndex];
    double cost = (double)sq->baseHouseCost;
    cost *= game->economy.cumulativeInflationMultiplier;

    if (sq->group >= 0 && sq->group < NUM_PROPERTY_GROUPS) {
        if (game->economy.groupMarket[sq->group].boomRoundsLeft > 0) {
            cost *= 1.10; /* +10% */
        }
    }

    /* Government Regulation: Housing Subsidy */
    if (game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_HOUSING_SUBSIDY) {
        cost *= 0.70; /* -30% */
    }

    /* Regional Card */
    if (game->economy.activeRegionalCardId >= 0 && game->economy.regionalCardRoundsLeft > 0) {
        const RegionalCard *card = &game->regionalDeck[game->economy.activeRegionalCardId];
        if (card->targetRegion == sq->region || card->targetGroup == sq->group) {
            cost *= (1.0 + (double)card->constrModifierPct / 100.0);
        }
    }

    if (cost < 50.0) cost = 50.0;
    return (int)cost;
}

int getHotelConstructionCost(const Game *game, int propertyIndex) {
    const Square *sq = &game->board[propertyIndex];
    double cost = (double)sq->baseHotelCost;
    cost *= game->economy.cumulativeInflationMultiplier;

    if (sq->group >= 0 && sq->group < NUM_PROPERTY_GROUPS) {
        if (game->economy.groupMarket[sq->group].boomRoundsLeft > 0) {
            cost *= 1.10;
        }
    }

    if (game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_HOUSING_SUBSIDY) {
        cost *= 0.70;
    }

    if (game->economy.activeRegionalCardId >= 0 && game->economy.regionalCardRoundsLeft > 0) {
        const RegionalCard *card = &game->regionalDeck[game->economy.activeRegionalCardId];
        if (card->targetRegion == sq->region || card->targetGroup == sq->group) {
            cost *= (1.0 + (double)card->constrModifierPct / 100.0);
        }
    }

    if (cost < 100.0) cost = 100.0;
    return (int)cost;
}

/* Rent Calculation */
int calculateRent(const Game *game, int propertyIndex, int diceRoll) {
    const Square *sq = &game->board[propertyIndex];
    if (sq->ownerId < 0) return 0;
    if (sq->mortgaged) return 0;
    if (sq->isDamaged) return 0;
    if (sq->closedTurns > 0) return 0;

    /* Railway */
    if (sq->type == SQ_RAILWAY) {
        int count = countStationsOwned(game->board, sq->ownerId);
        double rent = 0;
        switch (count) {
            case 1: rent = 250; break;
            case 2: rent = 500; break;
            case 3: rent = 1000; break;
            case 4: rent = 2000; break;
            default: rent = 250; break;
        }
        /* Fuel Shortage event or Fuel Crisis */
        if (game->economy.globalEventRoundsLeft > 0 && game->economy.activeGlobalEvent == GLOB_FUEL_CRISIS) {
            rent *= 2.0;
        }
        if (game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_RAILWAY_MODERNIZATION) {
            rent *= 1.50;
        }
        rent *= game->economy.cumulativeInflationMultiplier;
        return (int)rent;
    }

    /* Utility */
    if (sq->type == SQ_UTILITY) {
        int count = countUtilitiesOwned(game->board, sq->ownerId);
        int mult = (count >= 2) ? 10 : 4;
        if (game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_ELECTRICITY_TARIFF) {
            mult = (count >= 2) ? 15 : 6;
        }
        double rent = (double)(diceRoll * mult);
        rent *= game->economy.cumulativeInflationMultiplier;
        return (int)rent;
    }

    /* Residential Property */
    if (sq->type == SQ_PROPERTY) {
        double rent = (double)sq->baseRental;

        /* Development Multiplier */
        if (sq->hasHotel) {
            rent *= 10.0;
        } else {
            switch (sq->numHouses) {
                case 1: rent *= 2.0; break;
                case 2: rent *= 3.0; break;
                case 3: rent *= 5.0; break;
                case 4: rent *= 7.0; break;
                default:
                    if (hasMonopoly(game->board, sq->group, sq->ownerId)) {
                        rent *= 2.0; /* monopoly double rent on undeveloped */
                    } else {
                        rent *= 1.0;
                    }
                    break;
            }
        }

        /* Building Condition Gate (Rule-LK 26) */
        if (sq->numHouses > 0 || sq->hasHotel) {
            if (sq->buildingCondition >= 90) {
                rent *= 1.0;
            } else if (sq->buildingCondition >= 75) {
                rent *= 0.90;
            } else if (sq->buildingCondition >= 50) {
                rent *= 0.75;
            } else if (sq->buildingCondition >= 25) {
                rent *= 0.50;
            } else {
                return 0; /* building closed */
            }
        }

        /* Structural Damage */
        if (sq->structuralDamage) {
            rent *= 0.75; /* -25% max rent */
        }

        /* Dynamic Market */
        if (sq->group >= 0 && sq->group < NUM_PROPERTY_GROUPS) {
            if (game->economy.groupMarket[sq->group].boomRoundsLeft > 0) {
                rent *= 1.25; /* +25% rental income */
            } else if (game->economy.groupMarket[sq->group].declineRoundsLeft > 0) {
                rent *= 0.80; /* -20% rental income */
            }
        }

        /* Regional Card */
        if (game->economy.activeRegionalCardId >= 0 && game->economy.regionalCardRoundsLeft > 0) {
            const RegionalCard *card = &game->regionalDeck[game->economy.activeRegionalCardId];
            if (card->targetRegion == sq->region || card->targetGroup == sq->group) {
                rent *= (1.0 + (double)card->rentModifierPct / 100.0);
            }
        }

        /* Global Economic Event */
        if (game->economy.globalEventRoundsLeft > 0) {
            switch (game->economy.activeGlobalEvent) {
                case GLOB_TOURISM_BOOM:
                    if (sq->region == REG_SOUTHERN_PROVINCE || sq->region == REG_WESTERN_COASTAL) {
                        rent *= 1.20;
                    }
                    break;
                case GLOB_HEAVY_MONSOON:
                    if (!sq->hasHotel) {
                        rent *= 0.85;
                    }
                    break;
                case GLOB_ECONOMIC_RECESSION:
                    rent *= 0.85;
                    break;
                case GLOB_POLITICAL_UNREST:
                    rent *= 0.80;
                    break;
                default:
                    break;
            }
        }

        /* National Event Card on Owner */
        const Player *owner = &game->players[sq->ownerId];
        if (owner->nationalCardRoundsLeft > 0) {
            if (owner->activeNationalCard == 0 && sq->hasHotel) { /* Tourism Hype */
                rent *= 2.0;
            } else if (owner->activeNationalCard == 13 && sq->hasHotel) { /* Festival Season */
                rent *= 1.5;
            }
        }

        /* Inflation */
        rent *= game->economy.cumulativeInflationMultiplier;

        if (rent < 10.0) rent = 10.0;
        return (int)rent;
    }

    return 0;
}

/* Calculate Authoritative Net Worth (Rule 15 / FR-WIN-03) */
int calculateNetWorth(const Game *game, int playerId) {
    const Player *p = &game->players[playerId];
    if (p->bankrupt) return 0;

    int cash = p->cash;
    int propVal = 0;
    int bldgVal = 0;
    int railVal = 0;
    int utilVal = 0;

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (game->board[i].ownerId == playerId) {
            if (game->board[i].type == SQ_PROPERTY) {
                propVal += getPropertyMarketValue(game, i);
                bldgVal += getBuildingValue(game, i);
            } else if (game->board[i].type == SQ_RAILWAY) {
                railVal += getPropertyMarketValue(game, i);
            } else if (game->board[i].type == SQ_UTILITY) {
                utilVal += getPropertyMarketValue(game, i);
            }
        }
    }

    int claims = p->pendingInsuranceClaim;
    int outstandingLoan = p->loan.active ? p->loan.outstandingBalance : 0;
    int accruedInterest = 0; /* interest already rolled into balance */
    int taxesDue = 0;

    int netWorth = cash + propVal + bldgVal + railVal + utilVal + claims - outstandingLoan - accruedInterest - taxesDue;
    return netWorth;
}

/* Loan Interest Rate Mapping (Appendix D + Government Regulations) */
int getPrevailingLoanInterestRate(const Game *game) {
    int rate = game->economy.currentBaseLoanInterest;
    if (game->economy.currentInflationRate <= -3) {
        rate = 5;  /* Boom / low rate */
    } else if (game->economy.currentInflationRate <= 2) {
        rate = 8;  /* Stable */
    } else if (game->economy.currentInflationRate <= 5) {
        rate = 10; /* Moderate */
    } else if (game->economy.currentInflationRate <= 8) {
        rate = 12; /* High */
    } else {
        rate = 15; /* Recession */
    }

    if (game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_REDUCE_LOAN_INTEREST) {
        rate -= 2;
    }
    if (game->economy.globalEventRoundsLeft > 0 && game->economy.activeGlobalEvent == GLOB_FOREIGN_INVESTMENT) {
        rate -= 2;
    }
    if (game->economy.globalEventRoundsLeft > 0 && game->economy.activeGlobalEvent == GLOB_ECONOMIC_RECESSION) {
        rate += 3;
    }

    if (rate < 3) rate = 3;
    return rate;
}

/* Calculate Maximum Permitted Loan (Rule-LK 2) */
int calculateMaxLoan(const Game *game, int playerId, const int *collateralIndices, int numCollateral) {
    int totalMortgageVal = 0;
    for (int i = 0; i < numCollateral; i++) {
        int idx = collateralIndices[i];
        if (idx >= 0 && idx < BOARD_SIZE) {
            const Square *sq = &game->board[idx];
            if (sq->ownerId == playerId && !sq->mortgaged && !sq->loanLocked) {
                totalMortgageVal += sq->baseMortgageValue;
            }
        }
    }
    return (int)(totalMortgageVal * 0.75);
}

/* Take out a Bank Loan (FR-BANK-01..05) */
int takeLoan(Game *game, int playerId, const int *collateralIndices, int numCollateral) {
    Player *p = &game->players[playerId];
    if (p->loan.active) return 0;
    if (numCollateral <= 0) return 0;

    int maxLoan = calculateMaxLoan(game, playerId, collateralIndices, numCollateral);
    if (maxLoan <= 0) return 0;

    p->loan.active = 1;
    p->loan.principal = maxLoan;
    p->loan.outstandingBalance = maxLoan;
    p->loan.interestRate = getPrevailingLoanInterestRate(game);
    p->loan.duration = 20;
    p->loan.roundsElapsed = 0;
    p->loan.numCollateral = numCollateral;

    printf("\n%s obtained a secured loan.\n", p->name);
    printf("Loan Amount : LKR %d.\n", maxLoan);
    printf("Collateral :\n");

    for (int i = 0; i < numCollateral; i++) {
        int idx = collateralIndices[i];
        p->loan.collateralIndices[i] = idx;
        game->board[idx].loanLocked = 1;
        printf("%s\n", game->board[idx].name);
    }

    printf("Interest Rate : %d%%\n", p->loan.interestRate);
    printf("Duration : %d Rounds\n", p->loan.duration);

    p->cash += maxLoan;
    return maxLoan;
}

/* Repay a portion or full loan */
int repayLoan(Game *game, int playerId, int amount) {
    Player *p = &game->players[playerId];
    if (!p->loan.active || amount <= 0) return 0;

    int actualRepay = amount;
    if (actualRepay > p->cash) actualRepay = p->cash;
    if (actualRepay > p->loan.outstandingBalance) actualRepay = p->loan.outstandingBalance;

    p->cash -= actualRepay;
    p->loan.outstandingBalance -= actualRepay;

    printf("\n%s repaid LKR %d.\n", p->name, actualRepay);
    printf("Outstanding Balance : LKR %d.\n", p->loan.outstandingBalance);

    if (p->loan.outstandingBalance <= 0) {
        p->loan.active = 0;
        p->loan.outstandingBalance = 0;
        for (int i = 0; i < p->loan.numCollateral; i++) {
            int idx = p->loan.collateralIndices[i];
            game->board[idx].loanLocked = 0;
        }
        p->loan.numCollateral = 0;
    }
    return actualRepay;
}

/* Refinance Loan */
int refinanceLoan(Game *game, int playerId) {
    Player *p = &game->players[playerId];
    if (!p->loan.active) return 0;

    p->loan.roundsElapsed = 0;
    p->loan.interestRate = getPrevailingLoanInterestRate(game);
    printf("\n%s refinanced existing loan at %d%% interest rate.\n", p->name, p->loan.interestRate);
    return 1;
}

/* Accrue Loan Interest at end of round (Rule-LK 4) */
void accrueInterest(Game *game) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        Player *p = &game->players[i];
        if (p->bankrupt || !p->loan.active) continue;

        p->loan.roundsElapsed++;
        int interest = (int)((double)p->loan.outstandingBalance * (double)p->loan.interestRate / 100.0);
        if (interest < 1 && p->loan.outstandingBalance > 0) interest = 1;
        p->loan.outstandingBalance += interest;
    }
}

/* Check Loan Defaults at end of round (Rule-LK 6, 7) */
void checkLoanDefaults(Game *game) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        Player *p = &game->players[i];
        if (p->bankrupt || !p->loan.active) continue;

        if (p->loan.roundsElapsed >= p->loan.duration && p->loan.outstandingBalance > 0) {
            printf("\n%s has defaulted.\n", p->name);
            printf("Collateral has been foreclosed.\n");
            printf("Outstanding debt cleared.\n");

            int foreclosed[BOARD_SIZE];
            int numForeclosed = p->loan.numCollateral;
            for (int k = 0; k < numForeclosed; k++) {
                foreclosed[k] = p->loan.collateralIndices[k];
            }

            p->loan.active = 0;
            p->loan.outstandingBalance = 0;
            p->loan.numCollateral = 0;

            for (int k = 0; k < numForeclosed; k++) {
                int idx = foreclosed[k];
                Square *sq = &game->board[idx];
                sq->ownerId = -1;
                sq->loanLocked = 0;
                sq->mortgaged = 0;
                sq->numHouses = 0;
                sq->hasHotel = 0;
                sq->insurance.active = 0;
                sq->insurance.type = INS_NONE;
                /* Foreclosed property goes to auction (FR-AUC-01) */
                triggerAuction(game, idx);
            }

            /* If player has no remaining assets, declared bankrupt */
            int remainingAssets = p->cash;
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (game->board[j].ownerId == p->id) remainingAssets += 1000;
            }
            if (remainingAssets <= 0) {
                declareBankruptcy(game, p->id);
            }
        }
    }
}

/* Insurance Premiums (Appendix E) */
int getInsurancePremium(const Game *game, int propertyIndex, InsuranceType type) {
    int propVal = getPropertyMarketValue(game, propertyIndex);
    double rate = 0.05;
    switch (type) {
        case INS_BASIC: rate = 0.05; break;
        case INS_COMPREHENSIVE: rate = 0.10; break;
        case INS_BUSINESS_INTERRUPTION: rate = 0.15; break;
        default: return 0;
    }
    if (game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_INSURANCE_REGULATION) {
        rate *= 0.80; /* 20% discount */
    }
    int prem = (int)(propVal * rate);
    if (prem < 100) prem = 100;
    return prem;
}

/* Purchase Insurance (FR-INS-01..03) */
int purchaseInsurance(Game *game, int playerId, int propertyIndex, InsuranceType type) {
    Player *p = &game->players[playerId];
    Square *sq = &game->board[propertyIndex];
    if (sq->ownerId != playerId) return 0;

    int premium = getInsurancePremium(game, propertyIndex, type);
    if (p->cash < premium) return 0;

    p->cash -= premium;
    sq->insurance.type = type;
    sq->insurance.propertyIndex = propertyIndex;
    sq->insurance.premium = premium;
    sq->insurance.compensationRate = (type == INS_BASIC) ? 80 : 100;
    sq->insurance.startRound = game->currentRound;
    sq->insurance.duration = 20;
    sq->insurance.active = 1;
    sq->insurance.expiryNotified = 0;

    const char *typeStr = (type == INS_BASIC) ? "Basic Property Insurance" :
                          (type == INS_COMPREHENSIVE) ? "Comprehensive Insurance" :
                          "Business Interruption Insurance";

    printf("\n%s purchased.\n", typeStr);
    printf("Property : %s\n", sq->name);
    printf("Premium : LKR %d.\n", premium);
    return 1;
}

/* Check Insurance Expiry */
void checkInsuranceExpiry(Game *game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        Square *sq = &game->board[i];
        if (sq->insurance.active) {
            int elapsed = game->currentRound - sq->insurance.startRound;
            int remaining = sq->insurance.duration - elapsed;
            if (remaining == 3 && !sq->insurance.expiryNotified) {
                printf("\nInsurance policy on %s expires in 3 rounds.\n", sq->name);
                sq->insurance.expiryNotified = 1;
            } else if (remaining <= 0) {
                sq->insurance.active = 0;
                sq->insurance.type = INS_NONE;
            }
        }
    }
}

/* Disasters (Rule-LK 10, FR-INS-04) */
void triggerDisasterCheck(Game *game) {
    if (game->currentRound % 10 != 0) return;

    /* Find developed properties */
    int candidates[BOARD_SIZE];
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (game->board[i].type == SQ_PROPERTY && game->board[i].ownerId >= 0 &&
            (game->board[i].numHouses > 0 || game->board[i].hasHotel)) {
            candidates[count++] = i;
        }
    }
    if (count == 0) return;

    int chosenIdx = candidates[rand() % count];
    Square *sq = &game->board[chosenIdx];
    Player *owner = &game->players[sq->ownerId];

    DisasterType disaster = (DisasterType)(rand() % 5);
    const char *disasterName = "Disaster";
    switch (disaster) {
        case DISASTER_FIRE: disasterName = "Fire"; break;
        case DISASTER_FLOOD: disasterName = "Flood"; break;
        case DISASTER_RIOT: disasterName = "Riot"; break;
        case DISASTER_BUILDING_COLLAPSE: disasterName = "Building Collapse"; break;
        case DISASTER_ELECTRICAL_FAILURE: disasterName = "Electrical Failure"; break;
    }

    printf("\n%s occurred.\n", disasterName);
    printf("Affected Property : %s.\n", sq->name);

    int repairCost = (int)(getPropertyMarketValue(game, chosenIdx) * 0.25);
    if (repairCost < 500) repairCost = 500;

    if (sq->insurance.active) {
        int comp = (repairCost * sq->insurance.compensationRate) / 100;
        if (sq->insurance.type == INS_BUSINESS_INTERRUPTION && sq->hasHotel) {
            comp += calculateRent(game, chosenIdx, 7) * 5; /* 5 rounds hotel rent compensation */
        }
        printf("Insurance Claim Approved.\n");
        printf("Compensation Paid : LKR %d.\n", comp);
        owner->cash += comp;
        sq->isDamaged = 0;
    } else {
        printf("Property is uninsured. Repair Cost : LKR %d.\n", repairCost);
        owner->sufferedLossRecently = 1;
        if (owner->cash >= repairCost) {
            owner->cash -= repairCost;
            printf("%s paid LKR %d for automatic repairs.\n", owner->name, repairCost);
            sq->isDamaged = 0;
        } else {
            sq->isDamaged = 1;
            printf("%s has insufficient funds. Building closed pending repair.\n", owner->name);
        }
    }
}

/* Taxes */
int calculateIncomeTax(const Game *game, int playerId) {
    int totalMarketVal = calculateTotalPropertyMarketValue(game, playerId);
    int rate = 15; /* base 15% */
    if (game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_INCREASE_PROP_TAX) {
        rate += 5;
    }
    int tax = (totalMarketVal * rate) / 100;
    return tax;
}

int calculateCDFTax(const Game *game, int playerId) {
    int totalMarketVal = calculateTotalPropertyMarketValue(game, playerId);
    int rate = 10; /* base 10% */
    if (game->economy.govRegulationRoundsLeft > 0 && game->economy.activeGovRegulation == GOV_INCREASE_PROP_TAX) {
        rate += 5;
    }
    int tax = (totalMarketVal * rate) / 100;
    return tax;
}

int payPlayerTax(Game *game, int playerId, int taxAmount, const char *taxName) {
    Player *p = &game->players[playerId];
    printf("\n%s landed on %s.\n", p->name, taxName);
    printf("Tax Assessed : LKR %d.\n", taxAmount);

    if (p->cash >= taxAmount) {
        p->cash -= taxAmount;
        printf("%s paid LKR %d in tax. Current Balance : LKR %d.\n", p->name, taxAmount, p->cash);
        return 1;
    } else {
        return handlePlayerPayment(game, playerId, -1, taxAmount, taxName);
    }
}

/* Property Age & Building Condition Updates */
void updatePropertyAgeAndCondition(Game *game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        Square *sq = &game->board[i];
        if (sq->type == SQ_PROPERTY && sq->ownerId >= 0) {
            sq->age++;
            if (sq->age > 50 && (sq->age - 50) % 5 == 0) {
                int deprSteps = (sq->age - 50) / 5;
                int deprPct = deprSteps > 30 ? 30 : deprSteps;
                if (deprPct % 5 == 0) {
                    printf("\nProperty\n%s\nhas depreciated by %d%%.\nCurrent Value\nLKR %d.\n",
                           sq->name, deprPct, getPropertyMarketValue(game, i));
                }
            }

            if (sq->numHouses > 0 || sq->hasHotel) {
                if (sq->buildingCondition > 2) {
                    sq->buildingCondition -= 2;
                } else {
                    sq->buildingCondition = 0;
                }

                sq->consecutiveNeglectedRounds++;
                if (sq->consecutiveNeglectedRounds > 20 && !sq->structuralDamage) {
                    sq->structuralDamage = 1;
                    printf("\n%s suffered structural damage due to neglected maintenance!\n", sq->name);
                }
            }
        }
    }
}

/* Renovate for Age-based Depreciation (Rule-LK 17) */
int renovateForDepreciation(Game *game, int playerId, int propertyIndex) {
    Player *p = &game->players[playerId];
    Square *sq = &game->board[propertyIndex];
    if (sq->ownerId != playerId || sq->age <= 0) return 0;

    int cost = (int)(getPropertyMarketValue(game, propertyIndex) * 0.10);
    if (cost < 100) cost = 100;
    if (p->cash < cost) return 0;

    p->cash -= cost;
    sq->age = 0;
    printf("\n%s renovated %s for LKR %d (age reset to 0).\n", p->name, sq->name, cost);
    return 1;
}

/* Perform Regular Building Maintenance (Rule-LK 27) */
int performBuildingMaintenance(Game *game, int playerId, int propertyIndex) {
    Player *p = &game->players[playerId];
    Square *sq = &game->board[propertyIndex];
    if (sq->ownerId != playerId || (sq->numHouses == 0 && !sq->hasHotel)) return 0;
    if (sq->buildingCondition >= 100) return 0;

    int cost = 0;
    if (sq->hasHotel) {
        cost = (int)(getHotelConstructionCost(game, propertyIndex) * 0.08);
    } else {
        cost = (int)(getHouseConstructionCost(game, propertyIndex) * 0.05 * sq->numHouses);
    }
    if (sq->structuralDamage) {
        cost = (int)(cost * 1.50); /* +50% maintenance cost */
    }
    if (cost < 50) cost = 50;
    if (p->cash < cost) return 0;

    p->cash -= cost;
    sq->buildingCondition = 100;
    sq->consecutiveNeglectedRounds = 0;
    printf("\n%s performed maintenance on %s for LKR %d (condition restored to 100%%).\n",
           p->name, sq->name, cost);
    return 1;
}

/* Repair Structural Damage (Rule-LK 29) */
int repairStructuralDamage(Game *game, int playerId, int propertyIndex) {
    Player *p = &game->players[playerId];
    Square *sq = &game->board[propertyIndex];
    if (sq->ownerId != playerId || !sq->structuralDamage) return 0;

    int replaceVal = getBuildingValue(game, propertyIndex);
    if (replaceVal <= 0) replaceVal = getHouseConstructionCost(game, propertyIndex);
    int cost = (int)(replaceVal * 0.25);
    if (cost < 200) cost = 200;
    if (p->cash < cost) return 0;

    p->cash -= cost;
    sq->structuralDamage = 0;
    sq->buildingCondition = 100;
    sq->consecutiveNeglectedRounds = 0;
    printf("\n%s repaired structural damage on %s for LKR %d.\n", p->name, sq->name, cost);
    return 1;
}

/* Inflation Draw (FR-INFL-01..03) */
void updateInflation(Game *game) {
    if (game->currentRound % 10 != 0) return;
    int rates[6] = {-3, 0, 2, 5, 8, 12};
    int chosen = rates[rand() % 6];
    game->economy.currentInflationRate = chosen;
    game->economy.cumulativeInflationMultiplier *= (1.0 + (double)chosen / 100.0);
}

/* General Payment & Debt Recovery Handler */
int handlePlayerPayment(Game *game, int payerId, int payeeId, int amount, const char *reason) {
    Player *payer = &game->players[payerId];
    if (payer->cash >= amount) {
        payer->cash -= amount;
        if (payeeId >= 0) {
            game->players[payeeId].cash += amount;
        }
        return 1;
    }

    /* Debt Recovery / Liquidation */
    /* 1. Try selling houses / hotels at 50% */
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (payer->cash >= amount) break;
        Square *sq = &game->board[i];
        if (sq->ownerId == payerId) {
            if (sq->hasHotel) {
                int refund = getHotelConstructionCost(game, i) / 2;
                sq->hasHotel = 0;
                sq->numHouses = 4;
                payer->cash += refund;
                printf("%s sold Hotel on %s for LKR %d.\n", payer->name, sq->name, refund);
            }
            while (sq->numHouses > 0 && payer->cash < amount) {
                int refund = getHouseConstructionCost(game, i) / 2;
                sq->numHouses--;
                payer->cash += refund;
                printf("%s sold House on %s for LKR %d.\n", payer->name, sq->name, refund);
            }
        }
    }

    /* 2. Try mortgaging unpledged properties */
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (payer->cash >= amount) break;
        Square *sq = &game->board[i];
        if (sq->ownerId == payerId && !sq->mortgaged && !sq->loanLocked && sq->numHouses == 0 && !sq->hasHotel) {
            sq->mortgaged = 1;
            int mortVal = sq->baseMortgageValue;
            payer->cash += mortVal;
            printf("%s mortgaged %s for LKR %d.\n", payer->name, sq->name, mortVal);
        }
    }

    if (payer->cash >= amount) {
        payer->cash -= amount;
        if (payeeId >= 0) {
            game->players[payeeId].cash += amount;
        }
        return 1;
    } else {
        /* Bankruptcy */
        if (payeeId >= 0) {
            game->players[payeeId].cash += payer->cash;
        }
        payer->cash = 0;
        printf("%s cannot afford %s of LKR %d.\n", payer->name, reason, amount);
        declareBankruptcy(game, payerId);
        return 0;
    }
}
