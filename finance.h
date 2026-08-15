#ifndef FINANCE_H
#define FINANCE_H

#include "types.h"

int getPropertyMarketValue(const Game *game, int propertyIndex);
int getBuildingValue(const Game *game, int propertyIndex);
int getHouseConstructionCost(const Game *game, int propertyIndex);
int getHotelConstructionCost(const Game *game, int propertyIndex);
int calculateRent(const Game *game, int propertyIndex, int diceRoll);
int calculateNetWorth(const Game *game, int playerId);
int calculateTotalPropertyMarketValue(const Game *game, int playerId);

int getPrevailingLoanInterestRate(const Game *game);
int calculateMaxLoan(const Game *game, int playerId, const int *collateralIndices, int numCollateral);
int takeLoan(Game *game, int playerId, const int *collateralIndices, int numCollateral);
int repayLoan(Game *game, int playerId, int amount);
int refinanceLoan(Game *game, int playerId);
void accrueInterest(Game *game);
void checkLoanDefaults(Game *game);

int getInsurancePremium(const Game *game, int propertyIndex, InsuranceType type);
int purchaseInsurance(Game *game, int playerId, int propertyIndex, InsuranceType type);
void checkInsuranceExpiry(Game *game);
void triggerDisasterCheck(Game *game);

int calculateIncomeTax(const Game *game, int playerId);
int calculateCDFTax(const Game *game, int playerId);
int payPlayerTax(Game *game, int playerId, int taxAmount, const char *taxName);

void updatePropertyAgeAndCondition(Game *game);
int renovateForDepreciation(Game *game, int playerId, int propertyIndex);
int performBuildingMaintenance(Game *game, int playerId, int propertyIndex);
int repairStructuralDamage(Game *game, int playerId, int propertyIndex);

void updateInflation(Game *game);

int handlePlayerPayment(Game *game, int payerId, int payeeId, int amount, const char *reason);

#endif /* FINANCE_H */
