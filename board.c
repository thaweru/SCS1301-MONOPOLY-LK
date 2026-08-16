#include "types.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void initBoard(Square board[BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[i].index = i;
        board[i].ownerId = -1;
        board[i].mortgaged = 0;
        board[i].loanLocked = 0;
        board[i].numHouses = 0;
        board[i].hasHotel = 0;
        board[i].buildingCondition = 100;
        board[i].consecutiveNeglectedRounds = 0;
        board[i].structuralDamage = 0;
        board[i].age = 0;
        board[i].isDamaged = 0;
        board[i].closedTurns = 0;
        board[i].insurance.type = INS_NONE;
        board[i].insurance.propertyIndex = i;
        board[i].insurance.premium = 0;
        board[i].insurance.compensationRate = 0;
        board[i].insurance.startRound = 0;
        board[i].insurance.duration = 20;
        board[i].insurance.active = 0;
        board[i].insurance.expiryNotified = 0;
        board[i].group = GRP_NONE;
        board[i].region = REG_NONE;
        board[i].basePurchasePrice = 0;
        board[i].baseRental = 0;
        board[i].baseMortgageValue = 0;
        board[i].baseHouseCost = 0;
        board[i].baseHotelCost = 0;
    }
    loadBoardFromCSV(board, "board_data.csv");
}

SquareType stringToSquareType(const char *str) {
    if (strcmp(str, "START") == 0) return SQ_START;
    if (strcmp(str, "PROPERTY") == 0) return SQ_PROPERTY;
    if (strcmp(str, "EVENT") == 0) return SQ_EVENT;
    if (strcmp(str, "TAX") == 0) return SQ_TAX;
    if (strcmp(str, "RAILWAY") == 0) return SQ_RAILWAY;
    if (strcmp(str, "UTILITY") == 0) return SQ_UTILITY;
    if (strcmp(str, "SPECIAL") == 0) return SQ_SPECIAL;
    if (strcmp(str, "INSURANCE") == 0) return SQ_INSURANCE;
    if (strcmp(str, "BANK") == 0) return SQ_BANK;
    return SQ_START;
}

PropertyGroup stringToPropertyGroup(const char *str) {
    if (strlen(str) == 0) return -1;  /* No group */
    if (strcmp(str, "BROWN") == 0) return GRP_BROWN;
    if (strcmp(str, "LIGHT_BLUE") == 0) return GRP_LIGHT_BLUE;
    if (strcmp(str, "PINK") == 0) return GRP_PINK;
    if (strcmp(str, "ORANGE") == 0) return GRP_ORANGE;
    if (strcmp(str, "RED") == 0) return GRP_RED;
    if (strcmp(str, "YELLOW") == 0) return GRP_YELLOW;
    if (strcmp(str, "GREEN") == 0) return GRP_GREEN;
    if (strcmp(str, "DARK_BLUE") == 0) return GRP_DARK_BLUE;
    return -1;
}

RegionType stringToRegion(const char *str) {
    if (strlen(str) == 0) return -1;  /* No region */
    if (strcmp(str, "COLOMBO_COMMERCIAL") == 0) return REG_COLOMBO_COMMERCIAL;
    if (strcmp(str, "WESTERN_COASTAL") == 0) return REG_WESTERN_COASTAL;
    if (strcmp(str, "WESTERN_RESIDENTIAL") == 0) return REG_WESTERN_RESIDENTIAL;
    if (strcmp(str, "CENTRAL_HILL") == 0) return REG_CENTRAL_HILL;
    if (strcmp(str, "WESTERN_AIRPORT") == 0) return REG_WESTERN_AIRPORT;
    if (strcmp(str, "SOUTHERN_PROVINCE") == 0) return REG_SOUTHERN_PROVINCE;
    if (strcmp(str, "NORTHERN_PROVINCE") == 0) return REG_NORTHERN_PROVINCE;
    if (strcmp(str, "EASTERN_PROVINCE") == 0) return REG_EASTERN_PROVINCE;
    if (strcmp(str, "PRIME_LUXURY") == 0) return REG_PRIME_LUXURY;
    return -1;
}

int loadBoardFromCSV(Square *board, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return -1;
    }

    char line[1024];
    int lineNum = 0;
    
    /* Skip header line */
    if (fgets(line, sizeof(line), file) == NULL) {
        fprintf(stderr, "Error: File is empty\n");
        fclose(file);
        return -1;
    }

    /* Parse data lines */
    while (fgets(line, sizeof(line), file) != NULL) {
        lineNum++;
        
        /* Remove newline */
        line[strcspn(line, "\n")] = 0;
        
        /* Skip empty lines */
        if (strlen(line) == 0) continue;

        /* Parse CSV line */
        char *token;
        char lineCopy[1024];
        strncpy(lineCopy, line, sizeof(lineCopy) - 1);
        lineCopy[sizeof(lineCopy) - 1] = 0;

        int squareId = -1;
        char name[256] = "";
        char typeStr[64] = "";
        char groupStr[64] = "";
        char regionStr[64] = "";
        int purchasePrice = 0;
        int rental = 0;
        int houseCost = 0;
        int hotelCost = 0;
        int mortgageValue = 0;

        /* Parse comma-separated values */
        int fieldNum = 0;
        token = strtok(lineCopy, ",");
        
        while (token != NULL && fieldNum < 10) {
            /* Trim whitespace */
            while (isspace(*token)) token++;
            char *end = token + strlen(token) - 1;
            while (end > token && isspace(*end)) {
                *end = 0;
                end--;
            }

            switch (fieldNum) {
                case 0: squareId = atoi(token); break;
                case 1: strncpy(name, token, sizeof(name) - 1); break;
                case 2: strncpy(typeStr, token, sizeof(typeStr) - 1); break;
                case 3: strncpy(groupStr, token, sizeof(groupStr) - 1); break;
                case 4: strncpy(regionStr, token, sizeof(regionStr) - 1); break;
                case 5: purchasePrice = (strlen(token) > 0) ? atoi(token) : 0; break;
                case 6: rental = (strlen(token) > 0) ? atoi(token) : 0; break;
                case 7: houseCost = (strlen(token) > 0) ? atoi(token) : 0; break;
                case 8: hotelCost = (strlen(token) > 0) ? atoi(token) : 0; break;
                case 9: mortgageValue = (strlen(token) > 0) ? atoi(token) : 0; break;
            }
            
            fieldNum++;
            token = strtok(NULL, ",");
        }

        /* Validate square ID */
        if (squareId < 0 || squareId >= 40) {
            fprintf(stderr, "Warning: Invalid square ID %d at line %d\n", squareId, lineNum + 1);
            continue;
        }

        /* Populate board square */
        board[squareId].type = stringToSquareType(typeStr);
        strncpy(board[squareId].name, name, sizeof(board[squareId].name) - 1);
        board[squareId].name[sizeof(board[squareId].name) - 1] = 0;
        
        board[squareId].group = stringToPropertyGroup(groupStr);
        board[squareId].region = stringToRegion(regionStr);
        board[squareId].basePurchasePrice = purchasePrice;
        board[squareId].baseRental = rental;
        board[squareId].baseHouseCost = houseCost;
        board[squareId].baseHotelCost = hotelCost;
        board[squareId].baseMortgageValue = mortgageValue;
    }

    fclose(file);
    printf("Successfully loaded board data from '%s'\n", filename);
    return 0;
}

int countPropertiesInGroup(PropertyGroup grp) {
    if (grp == GRP_BROWN || grp == GRP_DARK_BLUE) {
        return 2;
    }
    return 3;
}

int countOwnedInGroup(const Square board[BOARD_SIZE], PropertyGroup grp, int playerId) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i].type == SQ_PROPERTY && board[i].group == grp && board[i].ownerId == playerId) {
            count++;
        }
    }
    return count;
}

int hasMonopoly(const Square board[BOARD_SIZE], PropertyGroup grp, int playerId) {
    if (grp == GRP_NONE || playerId < 0) return 0;
    return countOwnedInGroup(board, grp, playerId) == countPropertiesInGroup(grp);
}

int countStationsOwned(const Square board[BOARD_SIZE], int playerId) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i].type == SQ_RAILWAY && board[i].ownerId == playerId && !board[i].mortgaged) {
            count++;
        }
    }
    return count;
}

int countUtilitiesOwned(const Square board[BOARD_SIZE], int playerId) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i].type == SQ_UTILITY && board[i].ownerId == playerId && !board[i].mortgaged) {
            count++;
        }
    }
    return count;
}

const char* getGroupName(PropertyGroup grp) {
    switch (grp) {
        case GRP_BROWN: return "Brown";
        case GRP_LIGHT_BLUE: return "Light Blue";
        case GRP_PINK: return "Pink";
        case GRP_ORANGE: return "Orange";
        case GRP_RED: return "Red";
        case GRP_YELLOW: return "Yellow";
        case GRP_GREEN: return "Green";
        case GRP_DARK_BLUE: return "Dark Blue";
        default: return "None";
    }
}

const char* getRegionName(RegionType reg) {
    switch (reg) {
        case REG_COLOMBO_COMMERCIAL: return "Colombo Commercial";
        case REG_WESTERN_COASTAL: return "Western Coastal";
        case REG_WESTERN_RESIDENTIAL: return "Western Residential";
        case REG_WESTERN_AIRPORT: return "Western Airport";
        case REG_CENTRAL_HILL: return "Central Hill Country";
        case REG_SOUTHERN_PROVINCE: return "Southern Province";
        case REG_NORTHERN_PROVINCE: return "Northern Province";
        case REG_EASTERN_PROVINCE: return "Eastern Province";
        case REG_PRIME_LUXURY: return "Prime Luxury";
        default: return "General";
    }
}
