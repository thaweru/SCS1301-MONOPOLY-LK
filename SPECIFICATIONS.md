# MONOPOLY-LK — Software Requirements Specification

## Document Control

| Field | Value |
|---|---|
| System | MONOPOLY-LK Simulation |
| Course | SCS 1301 — Data Structures and Program Design using C |
| Institution | University of Colombo School of Computing (UCSC) |
| Deliverable | Take-Home Assignment 1 |
| Assignment issued | Tuesday, July 21, 2026 |
| Submission deadline | August 16, 2026 @ 23:55 Hrs — Softcopy to UGVLE |
| Viva dates | September 21 – October 4, 2026 |
| Grade value | 40% of module total |
| Spec compiled | August 15, 2026 |
| Spec status | **Draft — see Section 9 for open items requiring confirmation before/while implementing** |

**Source documents consolidated into this specification:**

1. `Assignment_1_-_2026.pdf` — MONOPOLY in LK, primary assignment brief
2. `SCS1301_Assignment_01_clarifications.pdf` — Rent File and Other Clarifications
3. `scs1301_a1_class_diagram_drawio.pdf` — Provided UML class diagram
4. `Rent_....csv` — Per-property purchase price / base rent file — **not readable in this session; see Appendix F**

---

## How to Use This Document

This specification reorganizes the assignment brief into a standard software-requirements format: functional requirements grouped by subsystem, a data model derived from the provided class diagram, non-functional/build requirements, and the required output format. Every requirement below cites its original **Rule N** / **Rule-LK N** number so you can trace it back to the source PDF (useful for the viva).

The source material was written across two separate documents and, in places, describes the same time window or similar-sounding event under two different mechanisms. **Section 9 catalogues every place this happens or where a value is simply never defined**, along with a reasonable default assumption for each. Treat Section 9 as your first stop before writing `finance.c` or `events.c`, and consider confirming the flagged items with your instructor — several of them (repair cost basis, income tax base, loan interest source) directly affect whether your numeric output matches the grader's expectations.

---

## Table of Contents

1. Introduction
2. Overall Description
3. System Architecture & Data Model
4. Board Specification
5. Functional Requirements (5.1 – 5.19)
6. Player Strategy Requirements
7. Non-Functional Requirements
8. Output Requirements
9. Open Issues, Ambiguities & Assumptions Requiring Confirmation
10. Reference Data (Appendices A–F)
11. Revision History

---

## 1. Introduction

### 1.1 Purpose

This document specifies the functional and non-functional requirements for **MONOPOLY-LK**, a fully autonomous, Sri-Lanka-themed economic simulation to be implemented in C. It is intended to serve as the single reference used while designing `types.h` and implementing `board.c`, `players.c`, `finance.c`, `events.c`, `game.c`, and `main.c`.

### 1.2 Scope

The system simulates four computer-controlled players competing on a 40-square board through property trading, construction, loans, insurance, taxation, inflation, and randomised economic/regulatory events, until one solvent player remains or 500 rounds elapse. The program is a **non-interactive command-line simulation** — no runtime input is accepted once execution starts (per the assignment's framing; see also NFR-02 in Section 7).

Out of scope: any GUI, networked play, save/load of game state, or human-controlled players.

### 1.3 Definitions, Acronyms & Overlapping Terminology

| Term | Meaning in this spec |
|---|---|
| LKR | Sri Lankan Rupee — the game's sole currency unit; all amounts are integers |
| Round | One complete pass of all four players taking a turn |
| Turn | A single player's move within a round (Rule 3) |
| Monopoly | Owning every property in a colour group (Rule 8) |
| Loan-Locked | A property pledged as loan collateral (Rule-LK 3) — **not the same as "Mortgaged"**; see OI-3 |
| Mortgaged | A property sold back to the Bank for its mortgage value in exchange for forfeiting rent (Rule 7) |

Several near-identical names appear in different, independently-triggered subsystems. Keep these as **separate data structures** even though the vocabulary overlaps:

| Concept A | Concept B | Why they differ |
|---|---|---|
| **National Event Card** deck (Appendix A, 20 cards, drawn when a player *lands* on an Event square) | **Global Economic Event** (Rule-LK 18, fires automatically every 15 rounds for *all* players) | Different trigger (on-landing vs. scheduled), different effect lists, both loosely themed "Tourism/Housing/etc." |
| Property **age-based depreciation** (Rule-LK 15–17, up to −30% market value after 50 rounds) | Building **condition/maintenance** (Rule-LK 25–29, −2%/round, restored by maintenance) | Different trigger, different % scale, different remedy cost |
| "Renovate" under Rule-LK 17 (restores depreciation, costs 10% of current market value) | "Renovate" under Rule-LK 29 (restores structural damage, costs 25% of replacement value) | Same word, two different operations — name them distinctly in code |
| Square 2 "Community Development Fund" (fixed wealth tax, per clarification doc) | Squares 7/22/36 "National Event Card" (random card draw) | Both are typed `Event` in Table 1 but behave completely differently — see OI-14 |

### 1.4 References

- Rule numbers **1–15** = traditional rules, Section 1.3 of the assignment brief.
- Rule numbers **LK 1–36** = MONOPOLY-LK extensions, Section 2 of the assignment brief.
- Appendix letters **A–E** = reference tables from the assignment brief; **Appendix F** is new in this document.

---

## 2. Overall Description

### 2.1 Product Perspective

MONOPOLY-LK is a standalone console program, compiled from multiple `.c` files into a single executable (`monopoly`), that runs a closed economic simulation to completion and prints a structured event log plus periodic summaries.

### 2.2 Operating Environment & User Characteristics

- **No human user during execution.** All four players are autonomous; the "user" of this system is whoever runs the executable and reads the resulting transcript.
- Runs entirely via CLI/terminal (Program Requirements, Section 4 of brief).
- Compiler target: GCC, invoked as `gcc *.c -o monopoly`, must build with **zero compiler errors**.

### 2.3 Constraints

- Implementation language: **C** only.
- All monetary values: **integers** (no floats/doubles for money — Rule "All monetary calculations shall be performed using integer values").
- Minimum required source files: `types.h`, `board.c`, `players.c`, `finance.c`, `events.c`, `game.c`, `main.c` (additional files permitted).
- **Global variables avoided wherever possible.**
- **Dynamic memory allocation only where justified** (e.g., a fixed 4-player, 40-square, fixed-deck design likely needs little to none).
- Hard simulation ceiling: **500 rounds**.

### 2.4 Assumptions & Dependencies

- A pseudo-random number generator (e.g., seeded `rand()`) drives all dice rolls, card draws, event selection, and disaster occurrence.
- The board, property base data (Appendix B, and Appendix F once populated), rent tables, interest tables, and card decks are static configuration data known at compile time or loaded once at startup — none of it is meant to be user-supplied at runtime.
- See Section 9 for assumptions made to fill genuine gaps in the source rules.

---

## 3. System Architecture & Data Model

### 3.1 Conceptual Class Model (from the provided diagram)

The supplied class diagram describes an object-oriented decomposition. Since C has no classes or inheritance, treat this as the **entity/relationship model** to translate into structs, enums, and explicit dispatch — not as literal code to transcribe.

**Entities identified in the diagram:**

| Class | Key attributes | Key behaviours |
|---|---|---|
| `Game` | 4 players, 1 board, currentRound, economy state | startGame, runRound, isGameOver, determineWinner |
| `Board` | 40 squares | initializeBoard, getSquare(index) |
| `Square` («abstract») | index, name, type | — base for all square kinds |
| `Property` (a Square) | purchasePrice, mortgageValue, baseRental, houseCost, hotelCost, group, owner, mortgaged, numHouses, hasHotel, condition, age | calculateRent(diceValue), construct, mortgage, renovate |
| `RailwayStation` (a Square) | owner, mortgaged | calculateRent(stationsOwned) |
| `UtilityCompany` (a Square) | owner, mortgaged | calculateRent(diceValue, utilitiesOwned) |
| `BankSquare`, `InsuranceSquare`, `EventSquare`, `TaxSquare`, `SpecialSquare` | (Square variants, no extra fields shown) | — |
| `Player` | name, cash, position, inJail, jailTurns, properties[], loan, policies[], strategy | takeTurn, purchaseProperty, payRent, declareBankrupt, calculateNetWorth |
| `PlayerStrategy` («interface») | — | decidePurchase, decideBid, decideLoan, decideInsurance |
| `AggressiveInvestor`, `ConservativeBanker`, `RiskTaker`, `OpportunisticTrader` | (implementations of `PlayerStrategy`) | |
| `Loan` | principal, outstandingBalance, interestRate, duration, roundsElapsed, collateral[] | accrueInterest, repay, checkDefault |
| `InsurancePolicy` | type, property, premium, compensationRate, expiryRound | isExpired, fileClaim |
| `Auction` | property, currentBid, highestBidder, participants[] | placeBid, resolve |
| `EconomicEvent` | name, effects | apply(game) |
| `GovernmentRegulation` | name, effect | apply(game) |

**Relationships (cardinalities as drawn):**

| Relationship | From → To | Cardinality |
|---|---|---|
| has | Game → Board | 1 → 1 |
| has | Game → Player | 1 → 4 |
| contains | Board → Square | 1 → 40 |
| holds | Player → Loan | 1 → 0..1 |
| holds | Player → InsurancePolicy | 1 → 0..* |
| owns | Player → Property | 1 → 0..* |
| uses | Player → PlayerStrategy | 1 → 1 |
| collateral | Loan → Property | 1 → 1..* |
| covers | InsurancePolicy → Property | 1 → 1 |
| bidders | Auction → Player | 1 → 1..* |
| concerns | Auction → Property | 1 → 1 |
| triggers | EconomicEvent / GovernmentRegulation / Auction resolution → Game | 1 → 1 (three "triggers" links in the diagram) |

> **Note on fidelity:** the diagram PDF was supplied as a flattened export; the association labels and multiplicities above are transcribed as printed but their exact pairing to individual arrows could not be double-checked against the visual layout. If your copy of the diagram renders differently, trust the visual over this table.

### 3.2 Mapping to Required C Source Files

| File | Owns these requirement groups |
|---|---|
| `types.h` | All enums/structs from 3.3–3.4 below |
| `board.c` | Board init, square lookup/movement (§5.1 movement portion) |
| `players.c` | All four strategy implementations (§6), purchase/bid/loan/insurance decisions |
| `finance.c` | Bank & loans (§5.5), insurance (§5.6), taxation (§5.7), property depreciation (§5.9), building maintenance (§5.10) |
| `events.c` | Dynamic market (§5.11), regional cards (§5.12), national event cards (§5.13), global economic events (§5.14), government regulations (§5.15) |
| `game.c` | Turn/round orchestration (§5.1), auctions (§5.16), jail (§5.17), bankruptcy (§5.18), win detection (§5.19), round/end-of-game reporting (§8) |
| `main.c` | Entry point, RNG seeding, startup banner, main loop invocation |

### 3.3 Suggested Enumerations

```c
typedef enum { SQ_START, SQ_PROPERTY, SQ_RAILWAY, SQ_UTILITY, SQ_BANK,
               SQ_INSURANCE, SQ_EVENT, SQ_TAX, SQ_SPECIAL } SquareType;

typedef enum { GRP_BROWN, GRP_LIGHT_BLUE, GRP_PINK, GRP_ORANGE,
               GRP_RED, GRP_YELLOW, GRP_GREEN, GRP_DARK_BLUE } PropertyGroup;

typedef enum { INS_BASIC, INS_COMPREHENSIVE, INS_BUSINESS_INTERRUPTION, INS_NONE } InsuranceType;

typedef enum { STRAT_AGGRESSIVE, STRAT_CONSERVATIVE, STRAT_RISK_TAKER,
               STRAT_OPPORTUNISTIC } StrategyType;

typedef enum { DISASTER_FIRE, DISASTER_FLOOD, DISASTER_RIOT,
               DISASTER_BUILDING_COLLAPSE, DISASTER_ELECTRICAL_FAILURE } DisasterType;
```

### 3.4 Suggested Data Representation (illustrative, non-binding)

Since C lacks inheritance, the `Square` hierarchy is best modelled as one struct with a `SquareType` tag plus type-specific fields (a tagged union, or simply optional fields left unused for irrelevant types — either is defensible for a 40-element fixed array):

```c
typedef struct {
    int index;
    char name[50];
    SquareType type;
    PropertyGroup group;        /* meaningful only when type == SQ_PROPERTY */
    int purchasePrice;          /* per-property, from Appendix F / Rent file */
    int baseRental;             /* per-property, from Appendix F / Rent file */
    int mortgageValue, houseCost, hotelCost;  /* per-group, from Appendix B */
    int ownerId;                /* -1 = Bank */
    int mortgaged;              /* traditional mortgage, forfeits rent (Rule 7) */
    int loanLocked;             /* pledged as loan collateral (Rule-LK 3) — see OI-3 */
    int numHouses;
    int hasHotel;
    int buildingCondition;      /* 100 down to 0, Rule-LK 25 */
    int age;                    /* rounds since last renovation, Rule-LK 15 */
    int insurancePolicyId;      /* -1 = uninsured */
} Square;
```

The `PlayerStrategy` interface has no state of its own — in C this is naturally a **function-pointer table** (or a `StrategyType` tag dispatched through `switch` in `players.c`), e.g.:

```c
typedef struct {
    int  (*decidePurchase)(struct Player *self, Square *sq);
    int  (*decideBid)(struct Player *self, Auction *a);
    int  (*decideLoan)(struct Player *self);
    InsuranceType (*decideInsurance)(struct Player *self, Square *sq);
} StrategyOps;
```

Either approach satisfies the diagram's `PlayerStrategy` interface without needing C++-style polymorphism.

---

## 4. Board Specification

### 4.1 Complete Board Layout (40 Squares)

*(Table 1 of the brief, merged with the property-group list in §1.1.1 for a single authoritative reference. Full group financial baselines are in Appendix B.)*

| # | Type | Name | Group |
|---|---|---|---|
| 0 | Start | GO | — |
| 1 | Property | Pettah | Brown |
| 2 | Event *(fixed tax — see §5.7 & OI-14)* | Community Development Fund | — |
| 3 | Property | Maradana | Brown |
| 4 | Tax | Income Tax | — |
| 5 | Railway | Colombo Fort Railway Station | — |
| 6 | Property | Bambalapitiya | Light Blue |
| 7 | Event *(card draw)* | National Event Card | — |
| 8 | Property | Wellawatte | Light Blue |
| 9 | Property | Mount Lavinia | Light Blue |
| 10 | Special | Jail / Just Visiting | — |
| 11 | Property | Nugegoda | Pink |
| 12 | Utility | Ceylon Electricity Board | — |
| 13 | Property | Maharagama | Pink |
| 14 | Property | Kottawa | Pink |
| 15 | Railway | Kandy Railway Station | — |
| 16 | Property | Negombo | Orange |
| 17 | Insurance | Sri Lanka Insurance | — |
| 18 | Property | Katunayake | Orange |
| 19 | Property | Ja-Ela | Orange |
| 20 | Special | Free Parking | — |
| 21 | Property | Kandy City | Red |
| 22 | Event *(card draw)* | National Event Card | — |
| 23 | Property | Peradeniya | Red |
| 24 | Property | Katugastota | Red |
| 25 | Railway | Galle Railway Station | — |
| 26 | Property | Galle Fort | Yellow |
| 27 | Property | Unawatuna | Yellow |
| 28 | Utility | National Water Supply and Drainage Board | — |
| 29 | Property | Hikkaduwa | Yellow |
| 30 | Special | Go To Jail | — |
| 31 | Property | Jaffna Town | Green |
| 32 | Property | Nallur | Green |
| 33 | Insurance | Ceylinco Insurance | — |
| 34 | Property | Trincomalee | Green |
| 35 | Railway | Jaffna Railway Station | — |
| 36 | Event *(card draw)* | National Event Card | — |
| 37 | Property | Nuwara Eliya | Dark Blue |
| 38 | Bank | Bank of Ceylon | — |
| 39 | Property | Galle Face | Dark Blue |

**Special squares with no defined financial effect:** Free Parking (20) and Jail/Just Visiting (10, when merely visiting) have no stated transaction — a player simply ends their turn there.

### 4.2 Property Groups Summary

| Group | Properties | Count |
|---|---|---|
| 1 – Brown | Pettah, Maradana | 2 |
| 2 – Light Blue | Bambalapitiya, Wellawatte, Mount Lavinia | 3 |
| 3 – Pink | Maharagama, Nugegoda, Kottawa | 3 |
| 4 – Orange | Negombo, Katunayake, Ja-Ela | 3 |
| 5 – Red | Kandy City, Peradeniya, Katugastota | 3 |
| 6 – Yellow | Galle Fort, Unawatuna, Hikkaduwa | 3 |
| 7 – Green | Jaffna Town, Nallur, Trincomalee | 3 |
| 8 – Dark Blue | Nuwara Eliya, Galle Face | 2 |
| **Total** | | **22 properties** |

Per-property attributes required (Section 1.1.1 of the brief; all initially owned by the Bank): purchase price, mortgage value, base rental, house construction cost, hotel construction cost, current owner, mortgage status, insurance status, number of buildings.

---

## 5. Functional Requirements

### 5.1 Turn Sequence & Core Gameplay

| ID | Requirement | Source |
|---|---|---|
| FR-TURN-01 | Each of the 4 players starts with LKR 30,000 cash, no properties, no loans, no insurance, no railways, no utilities. | Rule 1 |
| FR-TURN-02 | Turn order set by each player rolling 2 dice; highest total goes first; ties reroll among tied players only; play proceeds clockwise thereafter. | Rule 2 |
| FR-TURN-03 | Each turn executes, in order: (1) resolve outstanding penalties, (2) roll two dice, (3) move clockwise, (4) resolve landing action, (5) purchase property if eligible, (6) construct buildings if eligible, (7) complete financial transactions, (8) end turn. | Rule 3 |
| FR-TURN-04 | Passing or landing on GO awards LKR 2,000. | Rule 4 |
| FR-TURN-05 | Landing on an unowned purchasable square offers a purchase at listed price; declining sends it immediately to auction. | Rule 5 |
| FR-TURN-06 | Landing on an owned, unmortgaged property requires paying rent to the owner; mortgaged properties collect no rent. | Rule 7 |
| FR-TURN-07 | A round = one complete pass of all 4 players. Simulation ends at 1 solvent player remaining, or 500 rounds — whichever first. | Rule 15 |
| FR-TURN-08 | If the round cap is reached with multiple solvent players, the highest net worth wins (see §5.19). | Rule 15 |

### 5.2 Properties, Purchases & Construction

| ID | Requirement | Source |
|---|---|---|
| FR-PROP-01 | Owning every property in a colour group = monopoly; construction is only legal on monopoly groups. | Rule 8 |
| FR-PROP-02 | Buildings must be constructed **evenly** across a group's properties. Max per property: 4 houses, or 1 hotel (never both simultaneously). | Rule 9, 10 |
| FR-PROP-03 | A hotel replaces 4 houses. | Rule 10 |
| FR-PROP-04 | Rent for a developed residential property = base rental × development multiplier (Appendix C, Table 6): No buildings ×1, 1 house ×2, 2 houses ×3, 3 houses ×5, 4 houses ×7, hotel ×10. | Appendix C |
| FR-PROP-05 | Actual rent paid is further scaled by: active building condition band (§5.10), any active market/regional/economic/inflation modifiers (§5.8, §5.11, §5.12, §5.14), subject to the stacking approach chosen per OI-8. | Rule-LK 13, 26, 31, 32, Regional cards |
| FR-PROP-06 | Purchase price and base rent are **per-property** values from the Rent file (Appendix F), not the group-level Appendix B figures — see clarification doc and OI-4/OI-16. | Clarification doc |
| FR-PROP-07 | Every property tracks: purchase price, mortgage value, base rental, house cost, hotel cost, current owner, mortgage status, insurance status, number of buildings, condition, age. | §1.1.1 |

### 5.3 Railway Stations

| ID | Requirement | Source |
|---|---|---|
| FR-RAIL-01 | 4 stations exist: Colombo Fort, Kandy, Jaffna, Galle. They cannot be developed with houses/hotels and cannot be insured. | §1.1.2 |
| FR-RAIL-02 | Stations may be mortgaged to the Bank like properties. | §1.1.2 |
| FR-RAIL-03 | Rent depends on how many stations **the same owner** holds: 1→LKR 250, 2→LKR 500, 3→LKR 1,000, 4→LKR 2,000. | Table 2 / Appendix C |

### 5.4 Utility Companies

| ID | Requirement | Source |
|---|---|---|
| FR-UTIL-01 | 2 utilities exist: Ceylon Electricity Board (CEB), National Water Supply and Drainage Board (NWSDB). Mortgageable, never developable. | §1.1.3 |
| FR-UTIL-02 | Rent = dice value rolled by the visiting player × 4 (one utility owned) or × 10 (both utilities owned by the same player). | §1.1.3 / Appendix C |

### 5.5 Commercial Bank & Loans

| ID | Requirement | Source |
|---|---|---|
| FR-BANK-01 | One Bank exists (Bank of Ceylon, square 38). Landing there allows exactly **one** transaction per visit: obtain loan, repay part, repay in full, refinance, or increase loan amount. | §1.1.4, Rule-LK 5 |
| FR-BANK-02 | A player may hold only **one active loan** at a time. | §1.1.4, §2.1 |
| FR-BANK-03 | Eligible collateral = Properties, Railway Stations, Utility Companies. Buildings are **never** counted as collateral. | Rule-LK 1 |
| FR-BANK-04 | Maximum loan = 75% of the total mortgage value of all pledged, currently-unmortgaged eligible collateral. Worked example: (2,000 + 3,000 + 4,000) × 75% = LKR 6,750. | Rule-LK 2 |
| FR-BANK-05 | Approved loan amount is credited to cash immediately. Pledged properties become **Loan-Locked**: cannot be sold, traded, auctioned, or additionally mortgaged; they *continue to earn rent* and *may still be developed*. | Rule-LK 3 |
| FR-BANK-06 | Initial loan duration: 20 rounds. At the end of every complete round, interest accrues: `outstandingBalance += outstandingBalance × interestRate`. | Rule-LK 4 |
| FR-BANK-07 | Failure to fully repay within the loan's duration triggers **default**: all pledged collateral transfers to the Bank, any houses/hotels on it are demolished, insurance on that collateral is cancelled, the outstanding debt is cleared, and the player continues with remaining (non-pledged) assets. | Rule-LK 6 |
| FR-BANK-08 | If, after default, the player has no remaining assets, they are declared bankrupt. | Rule-LK 7 |

### 5.6 Insurance

| ID | Requirement | Source |
|---|---|---|
| FR-INS-01 | 2 insurers exist (Sri Lanka Insurance, Ceylinco Insurance); insurance can only be bought/renewed by landing on one of their squares. Each policy covers exactly one property. | §1.2, §2.2 |
| FR-INS-02 | Three policy types (see Appendix E for premiums/compensation): Basic Property, Comprehensive, Business Interruption (hotels only). | Rule-LK 8 |
| FR-INS-03 | Policies are valid for 20 rounds; a renewal reminder fires 3 rounds before expiry; an expired policy provides zero protection. | Rule-LK 9 |
| FR-INS-04 | Every 10 rounds, a random disaster (fire, flood, riot, building collapse, or electrical failure) may strike **one randomly-selected developed property**. If insured, compensation is credited immediately; otherwise the owner pays full repair cost. | Rule-LK 10 |
| FR-INS-05 | A damaged building collects no rent until repaired. Repairs happen automatically once the owner has sufficient funds. | Rule-LK 11 |

> **Repair cost is never numerically defined anywhere in the source material** — see OI-1, and note the mismatch between the disaster list and the insured-peril list in OI-2.

### 5.7 Taxation

| ID | Requirement | Source |
|---|---|---|
| FR-TAX-01 | Landing on Income Tax (square 4) requires immediate payment; base rate is **15%** at game start, itself subject to market-condition adjustment over time. Failure to pay follows the normal debt-recovery process. | Rule 11; Clarification doc |
| FR-TAX-02 | Square 2, "Community Development Fund," is **not** a random-card Event square — it always levies a fixed tax of **10% of the player's total assets**, defined as the current market value of owned properties only (buildings excluded). This 10% rate is itself subject to market fluctuation. | Clarification doc |

> The taxable base for Income Tax (FR-TAX-01) is not restated as explicitly as it is for the Community Development Fund — see OI-15 for the recommended reading (by analogy, property market value only).

### 5.8 Inflation

| ID | Requirement | Source |
|---|---|---|
| FR-INFL-01 | Every 10 rounds, an inflation rate is drawn from: −3%, 0%, 2%, 5%, 8%, 12% (negative = deflation). | Rule-LK 12 |
| FR-INFL-02 | Inflation modifies: property prices, building/hotel costs, rental values, insurance premiums, repair costs, and loan interest rates **for new loans only** — already-issued loans keep their original rate. | Rule-LK 13 |
| FR-INFL-03 | Adjustment formula: `New Value = Previous Value × (1 + Inflation Rate)`. Inflation is stated to have a **compounding** effect over successive cycles. | Rule-LK 14, §2.3 intro |

### 5.9 Property Depreciation (Age-Based)

| ID | Requirement | Source |
|---|---|---|
| FR-DEPR-01 | Every property tracks an age counter, incremented every complete round. | Rule-LK 15 |
| FR-DEPR-02 | Once age exceeds 50 rounds without renovation, the property loses 1% of value every 5 rounds, capped at 30% total depreciation. | Rule-LK 16 |
| FR-DEPR-03 | Landing on one's own property allows renovation: restores depreciation, increases rental, resets age to 0. Cost = 10% of the property's current market value. | Rule-LK 17 |

> Whether this depreciation reduces collectible **rent** or only the property's **valuation** (net worth/insurance/tax basis) is not stated — see OI-13.

### 5.10 Building Condition & Maintenance

| ID | Requirement | Source |
|---|---|---|
| FR-BLDG-01 | Every house/hotel has a Condition Rating starting at 100%, decreasing 2% per round. | Rule-LK 25 |
| FR-BLDG-02 | Condition gates rent collection (Appendix C repeats this as Table 3): 90–100%→100% rent, 75–89%→90%, 50–74%→75%, 25–49%→50%, below 25%→**building closed, no rent**. | Rule-LK 26 |
| FR-BLDG-03 | Maintenance is only performable at the start of a player's own turn; it restores condition to 100%. Cost: house = 5% of construction cost, hotel = 8% of construction cost. Any number of buildings may be maintained if funds allow. | Rule-LK 27 |
| FR-BLDG-04 | Maintenance neglected for more than 20 consecutive rounds triggers structural damage: −15% property value, −25% max rent, +50% future maintenance cost. | Rule-LK 28 |
| FR-BLDG-05 | A structurally-damaged building may be renovated (distinct from FR-DEPR-03!) to restore property value, rental value, and condition, at a cost of 25% of the building's replacement value. | Rule-LK 29 |

### 5.11 Dynamic Property Market

| ID | Requirement | Source |
|---|---|---|
| FR-MKT-01 | Every 10 rounds the market is reviewed: one random property group gets a Market Boom, a different random group gets a Market Decline. The same group cannot repeat the same event on consecutive reviews. | Rule-LK 30 |
| FR-MKT-02 | Market Boom (active 10 rounds): purchase price +15%, mortgage value +15%, rental income +25%, construction cost +10%, property value +20%. | Rule-LK 31 |
| FR-MKT-03 | Market Decline (active 10 rounds): property value −15%, rental income −20%, mortgage value −10%, auction starting price −25%. | Rule-LK 32 |
| FR-MKT-04 | A group hit by a boom or decline cannot be selected again until at least 30 rounds have elapsed. | Rule-LK 33 |
| FR-MKT-05 | Simultaneous events on the same group stack **cumulatively**. | Rule-LK 34 |

### 5.12 Regional Development Cards

| ID | Requirement | Source |
|---|---|---|
| FR-REG-01 | Every 15 rounds, one Regional Development Card is drawn (Appendix E lists 12), active for 15 rounds, affecting only the named region's properties/rents. | §2.10 |
| FR-REG-02 | On expiry, affected values return to their normal market-adjusted baseline unless another active event still applies to them. | Rule-LK 35 |
| FR-REG-03 | The simulation displays currently-active regional market conditions at the end of every round. | Rule-LK 36 |

### 5.13 National Event Cards

| ID | Requirement | Source |
|---|---|---|
| FR-EVT-01 | A 20-card deck exists (Appendix A). Landing on an Event square (7, 22, or 36 — **not** square 2, see FR-TAX-02/OI-14) draws the top card, which is then returned to the bottom of the deck. | Appendix A |
| FR-EVT-02 | The drawn effect applies to the drawing player for 15 rounds, stacking with other active modifiers. | Appendix A |

### 5.14 Global Economic Events

| ID | Requirement | Source |
|---|---|---|
| FR-ECO-01 | Every 15 rounds, one national economic event fires automatically and affects **all** players (distinct mechanism from FR-EVT-01 — see §1.3 terminology table). | Rule-LK 18 |
| FR-ECO-02 | Possible events: Tourism Boom, Fuel Crisis, Heavy Monsoon, Economic Recession, Stock Market Boom, Government Housing Programme, Foreign Investment, Political Unrest — full effect list in §10 reference / Rule-LK 18. | Rule-LK 18 |

### 5.15 Government Regulations

| ID | Requirement | Source |
|---|---|---|
| FR-GOV-01 | Every 20 rounds, one regulation is randomly selected from 8 options (Increase Property Tax, Reduce Loan Interest, Housing Subsidy, Luxury Property Tax, Railway Modernization, Electricity Tariff Revision, Insurance Regulation, Anti-Speculation Act). | Rule-LK 24 |
| FR-GOV-02 | Anti-Speculation Act specifically caps undeveloped-property holdings at 3 per player and requires new purchases beyond that to be developed within 5 rounds. | Rule-LK 24 |

### 5.16 Auctions

| ID | Requirement | Source |
|---|---|---|
| FR-AUC-01 | Triggered when: a purchase is declined, a bankrupt player's assets are liquidated, or foreclosed collateral returns to the Bank. | §2.6 |
| FR-AUC-02 | All solvent players participate; **starting bid = 50% of market value** (this refines/supersedes Rule 6, which specified only the increment). | Rule-LK 19 |
| FR-AUC-03 | Minimum bid increment: LKR 250. | Rule 6, Rule-LK 20 |
| FR-AUC-04 | Declining to bid withdraws a player from that auction permanently; the highest remaining bidder wins. | Rule-LK 21 |
| FR-AUC-05 | A player cannot bid more cash than they currently hold; loans cannot be taken out mid-auction. | Rule-LK 22 |
| FR-AUC-06 | If nobody bids, ownership stays with the Bank. | Rule-LK 23 |

### 5.17 Jail

| ID | Requirement | Source |
|---|---|---|
| FR-JAIL-01 | Landing on "Go To Jail" (square 30) sends the player directly to Jail (square 10) without collecting GO money, even if they would otherwise pass GO. | Rule 12 |
| FR-JAIL-02 | A jailed player is released by: paying LKR 300 bail, rolling doubles, or after 3 turns imprisoned (whichever comes first as the player chooses/qualifies). | Rule 13 |

### 5.18 Bankruptcy

| ID | Requirement | Source |
|---|---|---|
| FR-BANKR-01 | A player becomes bankrupt when liabilities exceed available assets. | Rule 14 |
| FR-BANKR-02 | On bankruptcy: all buildings are removed, insurance policies expire, loans become immediately due, and remaining assets are transferred/liquidated (via the auction mechanism, §5.16). | Rule 14, §2.6 |

> Rule 14 references "bankruptcy rules defined later in this specification" — no separate, more detailed section exists beyond foreclosure (§5.5) and auction liquidation (§5.16) in the supplied material; see OI-17.

### 5.19 Win Conditions & Net Worth

| ID | Requirement | Source |
|---|---|---|
| FR-WIN-01 | Game ends when only one solvent player remains, or when 500 rounds are reached. | Rule 15 |
| FR-WIN-02 | If ended by round cap, the player with the highest net worth wins. | Rule 15 |
| FR-WIN-03 | Net worth = **Cash + Property Value + Building Value + Railway Value + Utility Value + Insurance Claims Receivable − Outstanding Loans − Accrued Interest − Taxes Due.** | Rule 15 (authoritative formula) |

---

## 6. Player Strategy Requirements

All four strategies implement the same `PlayerStrategy` interface (§3.4) and are consulted whenever a decision point is reached during a player's turn; each strategy independently evaluates all legal actions and executes the one that best satisfies its own behavioural profile.

### 6.1 Aggressive Investor (PS-AGG)

1. Always purchases an unowned property if at least one future rent payment remains affordable.
2. Always participates in auctions; bids aggressively up to 120% of estimated market value.
3. Prioritizes completing monopolies over isolated purchases.
4. Builds the maximum possible houses immediately upon securing a monopoly; converts to hotels as soon as legally permitted.
5. Takes loans whenever the funds would increase projected rental income; only repays once excess cash exceeds 2× the outstanding loan.
6. Insures houses with Basic; hotels with Comprehensive.
7. Never voluntarily sells unless bankruptcy is otherwise unavoidable.
8. Prioritizes premium properties (e.g., Galle Face, Nuwara Eliya).

### 6.2 Conservative Banker (PS-CON)

1. Purchases a property only if ≥50% of current cash remains afterward.
2. Bids in auctions only when the price is below market value.
3. Avoids loans unless bankruptcy is imminent; repays immediately whenever visiting the Bank with sufficient funds.
4. Always buys Comprehensive Insurance for every developed property.
5. Never builds hotels while any loan is outstanding.
6. Prefers railway stations and utilities for predictable income.
7. Avoids new investment during economic recessions.
8. Renovates as soon as depreciation exceeds 10%.
9. Maintains the largest cash reserve of any player.

### 6.3 Risk Taker (PS-RISK)

1. Purchases every available property whenever legally possible.
2. Always borrows the maximum loan permitted, and refinances frequently for more capital.
3. Bids aggressively in every auction, up to the limit of available cash.
4. Builds hotels as early as possible.
5. Buys insurance only *after* suffering a financial loss (reactive, not preventative).
6. Ignores depreciation until repair is unavoidable.
7. Sells lower-value holdings to fund premium developments.
8. Keeps investing through downturns; prioritizes expensive groups over a balanced portfolio.

### 6.4 Opportunistic Trader (PS-OPP)

1. Buys only when projected appreciation exceeds construction cost.
2. Prefers discounted auction buys over direct purchases.
3. Takes loans only when projected return exceeds borrowing cost.
4. Buys Comprehensive Insurance only for high-value developments.
5. Delays construction during inflationary periods; accelerates during Government Housing Subsidy periods.
6. Renovates once depreciation exceeds 15%.
7. Sells properties expected to decline in value following an economic event.
8. Favours investments that benefit from the currently-active government regulation.
9. Maintains a balanced portfolio across residential, railway, and utility holdings.
10. Always computes expected return before any financial decision.

### 6.5 Strategy Dispatch Design Note

Because all four strategies share the same decision points (purchase, bid, loan, insurance, construction, renovation, sale), a clean implementation assigns each `Player` a `StrategyType` at creation and either (a) a struct of function pointers (§3.4) populated per type, or (b) a single dispatcher in `players.c` that switches on `StrategyType` at each decision point. Either satisfies the diagram's `PlayerStrategy` interface.

---

## 7. Non-Functional Requirements

| ID | Requirement | Source |
|---|---|---|
| NFR-01 | Simulates exactly four autonomous players; zero runtime user interaction after launch. | §4 of brief |
| NFR-02 | All decisions are generated programmatically from the assigned strategy — no hardcoded/scripted per-run outcomes. | §4 of brief |
| NFR-03 | All randomness (dice, card draws, disaster occurrence, event/regulation selection) uses an appropriate PRNG. | §4 of brief |
| NFR-04 | All monetary values are integers throughout. | §4 of brief |
| NFR-05 | Every board space is backed by an appropriate data structure (§3–4). | §4 of brief |
| NFR-06 | Each player maintains independent financial records (cash, properties, loan, policies). | §4 of brief |
| NFR-07 | Each property tracks ownership, development, insurance, mortgage, depreciation, and valuation state. | §4 of brief |
| NFR-08 | Simulation terminates at 1 solvent player or 500 rounds. | §4 of brief |
| NFR-09 | Minimum file set: `types.h`, `board.c`, `players.c`, `finance.c`, `events.c`, `game.c`, `main.c`; additional files permitted. | Table 5 |
| NFR-10 | Global variables avoided wherever possible. | §4 of brief |
| NFR-11 | Dynamic memory allocation used only where justified. | §4 of brief |
| NFR-12 | **Must compile cleanly with `gcc *.c -o monopoly`** — zero compiler errors is a hard acceptance gate. | §4 of brief |

**Acceptance criteria** for the build as a whole are effectively NFR-01 through NFR-12 taken together, plus: the program runs unattended to completion and produces the output categories listed in Section 8 for a representative run.

---

## 8. Output Requirements

### 8.1 Required Message Categories

The simulation must produce a detailed, human-readable transcript covering every one of the following event categories as they occur:

Startup banner & player roster · Turn-order determination · Dice roll · Player movement · Passing/landing on GO · Property purchase · Rent payment · House construction · Hotel construction · Loan origination · Loan repayment · Loan default · Insurance purchase · Disaster occurrence & claim outcome · Auction (open → bids → resolution) · Economic event · Government regulation · Property depreciation · Insurance expiry warning · Bankruptcy · End-of-round summary (per player) · End-of-game summary · Current market-conditions block (Rule-LK 36).

### 8.2 Illustrative Output Formats

The brief provides literal example transcripts for each category. These are reproduced below as **formatting references** — the specific names/numbers in each example are illustrative sample data, not fixed requirements; your actual run will differ every execution.

**Startup**
```
MONOPOLY-LK Simulation

Player 1 : Aggressive Investor
Player 2 : Conservative Banker
Player 3 : Risk Taker
Player 4 : Opportunistic Trader

Each player begins with LKR 30,000.
```

**Turn-order determination**
```
Aggressive Investor rolls 9.
Conservative Banker rolls 6.
Risk Taker rolls 11.
Opportunistic Trader rolls 5.

Risk Taker will begin the game.

Turn order:
Risk Taker
Opportunistic Trader
Aggressive Investor
Conservative Banker
```

**Dice roll / movement / GO**
```
Aggressive Investor rolled 8.
Aggressive Investor moves from Square 12 to Square 20.
Aggressive Investor passed GO.
Collected LKR 2,000.
Current Balance : LKR 24,500.
```

**Purchase / rent**
```
Aggressive Investor purchased Galle Fort for LKR 4,500.
Remaining Balance : LKR 18,000.

Risk Taker landed on Galle Fort.
Rent Paid : LKR 750.
Owner : Aggressive Investor.
```

**Construction**
```
Aggressive Investor constructed one house on Galle Fort.
Construction Cost : LKR 1,500.

Aggressive Investor upgraded Galle Fort to a Hotel.
```

**Loan lifecycle**
```
Aggressive Investor obtained a secured loan.
Loan Amount : LKR 15,000.
Collateral :
Galle Fort
Unawatuna
Interest Rate : 8%
Duration : 20 Rounds

Aggressive Investor repaid LKR 5,000.
Outstanding Balance : LKR 11,200.

Aggressive Investor has defaulted.
Collateral has been foreclosed.
Outstanding debt cleared.
```

**Insurance / disaster**
```
Comprehensive Insurance purchased.
Property : Galle Fort
Premium : LKR 450.

Flood occurred.
Affected Property : Unawatuna.
Insurance Claim Approved.
Compensation Paid : LKR 2,500.
```

**Auction**
```
Auction Started.
Property : Nuwara Eliya
Opening Bid : LKR 3,500.

Risk Taker bids LKR 3,750.
Aggressive Investor bids LKR 4,000.
Conservative Banker withdraws.
Opportunistic Trader withdraws.
Aggressive Investor wins the auction.
```

**Economic event / regulation / depreciation / insurance expiry / bankruptcy**
```
Economic Event
Tourism Boom
Southern Province properties increase in value by 15%.

Government Regulation
Housing Subsidy Introduced.
Construction costs reduced by 30%.

Property
Maharagama
has depreciated by 5%.
Current Value
LKR 4,750.

Insurance policy on Galle Fort expires in 3 rounds.

Risk Taker has been declared bankrupt.
Remaining assets transferred to the Bank.
```

**End-of-round summary** (repeated per player)
```
=============================================
Round 37 Summary
=============================================
Aggressive Investor
Cash : LKR 12,300
Net Worth : LKR 82,500
Properties : 12
Hotels : 3
Outstanding Loan : LKR 6,500
---------------------------------------------
[... one block per player ...]
=============================================
```

**End-of-game**
```
=============================================
GAME OVER
Winner
Aggressive Investor
Total Cash
LKR 65,400
Total Property Value
LKR 210,700
Outstanding Loans
None
Net Worth
LKR 276,100
=============================================
```

**Rule-LK 36 market-conditions block** (end of every round)
```
=========================================
Current Market Conditions
=========================================
Market Boom
-------------
Southern Province (+20%)
Rounds Remaining : 7

Market Decline
----------------
Western Residential (-15%)
Rounds Remaining : 4

Regional Development
-----------------------
Northern Development Programme (+30%)
Rounds Remaining : 12

Inflation
------------
+5%

Current Loan Interest
-----------------------
9%
=========================================
```

> The 9% "Current Loan Interest" shown above does not match any of the five discrete rates in Appendix D (5/8/10/12/15%) — see OI-9.

---

## 9. Open Issues, Ambiguities & Assumptions Requiring Confirmation

The source material is thorough but was authored across two documents (brief + clarifications) plus a separately-supplied class diagram, and a few areas aren't fully pinned down. Each item below has a recommended default so implementation isn't blocked, but confirming with the instructor is worth the cost given the assignment's grade weight.

| ID | Area | Issue | Recommended default (confirm before relying on it) |
|---|---|---|---|
| OI-1 | Insurance | "Repair cost" (basis for 80%/100% compensation, Basic/Comprehensive) is never numerically defined anywhere in the source. | Define repair cost as a fixed % of the property's current market value (or building replacement value) and document the chosen % explicitly in your own design notes. |
| OI-2 | Insurance vs. disasters | Rule-LK 10's disaster pool (Fire, Flood, Riot, Building Collapse, Electrical Failure) doesn't fully align with insured perils (Fire/Flood for Basic; Fire/Flood/Riot/Vandalism[/Earthquake per Appendix E] for Comprehensive). Building Collapse and Electrical Failure aren't named as covered by anything. | Treat any active policy as covering *any* disaster on that property (simplest, avoids uncovered-disaster dead ends), and note this as a documented assumption. |
| OI-3 | Loans vs. mortgaging | "Mortgaged" (Rule 7, forfeits rent) and "Loan-Locked" (Rule-LK 3, keeps earning rent) are structurally similar but behave differently; the property attribute list only names one "Mortgage status" field. | Model as two independent boolean flags on `Property` (see §3.4 struct). |
| OI-4 | Loan collateral valuation | Rule-LK 2's worked example uses per-item mortgage values, but the clarification says the CSV supplies only per-property purchase price and base rent — implying mortgage value still comes from the group-level Appendix B table. | Every property in a group uses that group's Appendix B mortgage value for collateral purposes, regardless of its individual purchase price. |
| OI-5 | Loans | "Refinance" and "Extend the loan period" are named as selectable Bank actions (Rule-LK 5) with no defined mechanics (what changes — rate, duration, both?). | Define "refinance" as resetting `roundsElapsed` to 0 at the then-current prevailing rate; define "extend" as adding a fixed number of rounds (e.g., +10) to `duration`. Document your choice. |
| OI-6 | Loans | Whether *traditional* mortgaging (as opposed to loan collateral) is restricted to a Bank-square visit or can happen anytime during "complete financial transactions" (turn step 7) is unstated. | Allow standalone mortgage/unmortgage during a player's own turn step 7, independent of Bank-square landing. |
| OI-7 | Inflation | Rule-LK 14 gives a single-step formula; §2.3 says inflation "has a compounding effect" — unclear if each new rate compounds on the already-inflated running value or resets each cycle. | Compound cumulatively: each new inflation rate applies to the current (already-adjusted) value, not the original base. |
| OI-8 | Cross-system stacking | Rule-LK 34 states cumulative stacking *within* the Dynamic Market subsystem; no global rule states how inflation + market boom/decline + regional card + economic event + government regulation combine when several are active on the same property at once. | Apply all active percentage modifiers **additively** to the base value, in a fixed documented order (e.g., inflation → market → regional → economic event → regulation), each turn/round. Document the order chosen. |
| OI-9 | Loan interest | Appendix D ties interest to 5 named "Economic Conditions" (5/8/10/12/15%), but Rule-LK 12's inflation values are different numbers, and the sample output shows a loan at 8% and later a "Current Loan Interest" of 9% — a value absent from Appendix D entirely. | Define an explicit mapping from current inflation rate (and/or active economic event) to one of the 5 named conditions, and treat the "current" rate shown in the Rule-LK 36 block as the rate that would apply to a *new* loan issued that round (existing loans keep their origination rate per Rule-LK 13). Flag the 9%-vs-table mismatch to your instructor. |
| OI-10 | Regional/category tags | Terms like "Commercial properties," "Southern coastal," "Western Residential," "low-lying coastal" (used across events, market reviews, and sample output) aren't formally mapped to the 8 colour groups or specific squares. | Add an explicit `region` tag per property (e.g., Coastal-South, Coastal-West, Hill-Country, Northern, Commercial) alongside its colour group, and define the mapping once in `types.h`/board init. |
| OI-11 | Naming collisions | National Event Cards (Appendix A) and Global Economic Events (Rule-LK 18) both contain "Tourism"-themed entries with different effects; Government Regulations and Event Cards both have a "Housing Subsidy"-style −30% construction cost effect. | Keep these as entirely separate structs/decks/timers — do not merge or let one accidentally overwrite the other's active-effect tracking. |
| OI-12 | "Renovate" terminology | Rule-LK 17 (restores age depreciation, costs 10% of current market value) and Rule-LK 29 (restores structural damage, costs 25% of replacement value) are both called "renovate." | Name them distinctly in code, e.g. `renovateForDepreciation()` vs `repairStructuralDamage()`. |
| OI-13 | Depreciation scope | Rule-LK 16 states age-based depreciation reduces "value" — unclear whether this also reduces collectible rent, or only net-worth/valuation. | Treat it as reducing valuation (net worth, insurance basis, tax basis) only, leaving rent driven purely by FR-PROP-04/05, unless you find evidence otherwise. |
| OI-14 | Square 2 typing | "Community Development Fund" is typed `Event` in Table 1, but per the clarification doc it has a fixed wealth-tax behaviour, distinct from the random-draw National Event Card squares (7, 22, 36). | Special-case square 2 in `board.c`/`finance.c` rather than routing it through the National Event Card deck. |
| OI-15 | Income Tax base | The clarification gives a 15% base rate but doesn't explicitly restate the taxable base the way it does for the Community Development Fund. | By analogy with FR-TAX-02, use 15% of the current market value of the player's owned properties (buildings excluded), subject to market-rate adjustment. |
| OI-16 | Rent file (CSV) | Could not be read in this session — see Appendix F. | Populate Appendix F directly from your copy of the Rent CSV before finalising `types.h` data. |
| OI-17 | Bankruptcy detail | Rule 14 references "bankruptcy rules defined later in this specification," but no separate detailed section exists beyond foreclosure (§5.5) and bankrupt-asset auctions (§5.16) in the supplied PDF. | Treat those two mechanisms as the complete bankruptcy procedure unless your copy of the brief has an additional page not included in this upload. |

---

## 10. Reference Data (Appendices)

### Appendix A — National Event Card Deck (20 cards)

*Drawn when landing on squares 7, 22, or 36 (see OI-14 re: square 2). Effect applies to the drawing player for 15 rounds.*

| Card | Effect |
|---|---|
| Tourism Hype | Hotels earn double rent for 5 rounds |
| Fuel Shortage | Railway rent doubles for 5 rounds |
| Heavy Floods | Random coastal property damaged |
| Political Rally | One random property closed for 2 rounds |
| Stock Market Rise | All property values increase by 10% |
| Economic Downturn | Property values decrease by 15% |
| Housing Subsidy | House construction cost reduced by 30% |
| Interest Rate Cut | Loan interest reduced by 2% |
| Interest Rate Increase | Loan interest increased by 2% |
| Tax Amnesty | Each player receives LKR 2,000 |
| Power Failure | Utility income halved for 3 rounds |
| Foreign Funding | Commercial property values increase by 15% |
| Port Expansion | Railway station values increase by 20% |
| Festival Season | Hotels receive 50% additional rent |
| Labour Strike | Construction suspended for 2 rounds |
| Insurance Discount | Premiums reduced by 20% |
| Property Revaluation | Random property group appreciates by 15% |
| Currency Depreciation | Construction costs increase by 10% |
| Government Grant | Random player receives LKR 5,000 |
| National Disaster | Random developed property damaged |

### Appendix B — Property Values by Group

*Per clarification doc: **used for loan/collateral calculations** (mortgage value) and as the source of house/hotel construction costs. Actual per-property purchase price and rent come from Appendix F instead.*

| Group | Purchase Price | House Cost | Hotel Cost | Mortgage Value |
|---|---|---|---|---|
| Brown | 1,500 | 500 | 2,000 | 750 |
| Light Blue | 2,500 | 750 | 3,000 | 1,250 |
| Pink | 3,500 | 1,000 | 4,000 | 1,750 |
| Orange | 4,500 | 1,250 | 5,000 | 2,250 |
| Red | 5,500 | 1,500 | 6,000 | 2,750 |
| Yellow | 6,500 | 2,000 | 8,000 | 3,250 |
| Green | 8,000 | 2,500 | 10,000 | 4,000 |
| Dark Blue | 10,000 | 3,000 | 12,000 | 5,000 |

### Appendix C — Rent Tables

**Residential development multiplier** (applied to base rental — FR-PROP-04)

| Development | Rent Multiplier |
|---|---|
| No Buildings | ×1 (base rent) |
| 1 House | ×2 |
| 2 Houses | ×3 |
| 3 Houses | ×5 |
| 4 Houses | ×7 |
| Hotel | ×10 |

**Railway stations**

| Stations Owned | Rent (LKR) |
|---|---|
| 1 | 250 |
| 2 | 500 |
| 3 | 1,000 |
| 4 | 2,000 |

**Utility companies**

| Utilities Owned | Rent |
|---|---|
| One Utility | 4 × dice value |
| Both Utilities | 10 × dice value |

### Appendix D — Interest Rate Table

*See OI-9 for the unresolved mapping from game state to these named conditions.*

| Economic Condition | Annual Interest Rate |
|---|---|
| Economic Boom | 5% |
| Stable Economy | 8% |
| Moderate Inflation | 10% |
| High Inflation | 12% |
| Economic Recession | 15% |

### Appendix E — Insurance Policy Table

| Policy | Premium | Coverage |
|---|---|---|
| Basic Property Insurance | 5% of property value | Fire and Flood (80% of repair cost) |
| Comprehensive Insurance | 10% of property value | Fire, Flood, Riot, Vandalism, Earthquake (100% of repair cost) |
| Business Interruption Insurance | 15% of property value | Repair cost + lost hotel rental income for 5 rounds |

### Appendix F — Per-Property Purchase Price & Base Rent ⚠ Needs Data From Rent CSV

Per the clarification document, this file supersedes Appendix B's group-level purchase price for actual transactions, and supplies the base rent that Appendix B never had. **The uploaded `Rent_....csv` could not be read in this working session** (it did not sync to the file workspace, unlike the three PDFs). The table below has the correct board order, names, and groups pre-filled — copy the two right-hand columns straight out of your CSV to complete it.

| # | Property | Group | Purchase Price (LKR) | Base Rent (LKR) |
|---|---|---|---|---|
| 1 | Pettah | Brown | *from CSV* | *from CSV* |
| 3 | Maradana | Brown | *from CSV* | *from CSV* |
| 6 | Bambalapitiya | Light Blue | *from CSV* | *from CSV* |
| 8 | Wellawatte | Light Blue | *from CSV* | *from CSV* |
| 9 | Mount Lavinia | Light Blue | *from CSV* | *from CSV* |
| 11 | Nugegoda | Pink | *from CSV* | *from CSV* |
| 13 | Maharagama | Pink | *from CSV* | *from CSV* |
| 14 | Kottawa | Pink | *from CSV* | *from CSV* |
| 16 | Negombo | Orange | *from CSV* | *from CSV* |
| 18 | Katunayake | Orange | *from CSV* | *from CSV* |
| 19 | Ja-Ela | Orange | *from CSV* | *from CSV* |
| 21 | Kandy City | Red | *from CSV* | *from CSV* |
| 23 | Peradeniya | Red | *from CSV* | *from CSV* |
| 24 | Katugastota | Red | *from CSV* | *from CSV* |
| 26 | Galle Fort | Yellow | *from CSV* | *from CSV* |
| 27 | Unawatuna | Yellow | *from CSV* | *from CSV* |
| 29 | Hikkaduwa | Yellow | *from CSV* | *from CSV* |
| 31 | Jaffna Town | Green | *from CSV* | *from CSV* |
| 32 | Nallur | Green | *from CSV* | *from CSV* |
| 34 | Trincomalee | Green | *from CSV* | *from CSV* |
| 37 | Nuwara Eliya | Dark Blue | *from CSV* | *from CSV* |
| 39 | Galle Face | Dark Blue | *from CSV* | *from CSV* |

*(Sample-output cross-check: the brief's own example transcript shows "Aggressive Investor purchased Galle Fort for LKR 4,500" and "Rent Paid : LKR 750" for Galle Fort — these two figures are illustrative example values from the brief, not confirmed as your actual CSV's Galle Fort row; verify against your file rather than assuming they match.)*

---

## 11. Revision History

| Version | Date | Notes |
|---|---|---|
| 0.1 (this document) | August 15, 2026 | Initial compilation from the assignment brief, clarifications doc, and class diagram. Appendix F pending CSV data (OI-16). Open items in Section 9 pending instructor confirmation. |
