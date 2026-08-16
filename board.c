#include "types.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

sqrgrp property_group(char *str){
	if (strcmp(str, "Brown") == 0) return BROWN;
	if (strcmp(str, "Light Blue") == 0) return LIGHT_BLUE;
	if (strcmp(str, "Pink") == 0) return PINK;
	if (strcmp(str, "Orange") == 0) return ORANGE;
	if (strcmp(str, "Red") == 0) return RED;
	if (strcmp(str, "Yellow") == 0) return YELLOW;
	if (strcmp(str, "Green") == 0) return GREEN;
	if (strcmp(str, "Dark Blue") == 0) return DARK_BLUE;
	if (strcmp(str, "Jail / Just Visiting") == 0) return JAIL;
	if (strcmp(str, "Go To Jail") == 0) return GO_TO_JAIL;
	if (strcmp(str, "National Event Card") == 0) return EVENT_CARD;
	if (strcmp(str, "Community Development Fund") == 0) return CD_FUND;
	return NONE;
}

sqrTyp square_type(char *str){
	if (strcmp(str, "START") == 0) return START;
	if (strcmp(str, "PROPERTY") == 0) return PROPERTY;
	if (strcmp(str, "RAILWAY") == 0) return RAILWAY;
	if (strcmp(str, "UTILITY") == 0) return UTILITY;
	if (strcmp(str, "EVENT") == 0) return EVENT;
	if (strcmp(str, "SPECIAL") == 0) return SPECIAL;
	if (strcmp(str, "INSURANCE") == 0) return INSURANCE;
	if (strcmp(str, "BANK") == 0) return BANK;
	if (strcmp(str, "TAX") == 0) return TAX;
	return EMPTY;
}

game init_game(int n, char **arr){
	unsigned int seed;
	if (n == 2){
		char *str = arr[1];
		seed = (unsigned int)(strtol(str, NULL, 10) % UINT_MAX);
	}else{
		seed = time(NULL);
	}
	srand(seed);
	printf("Seed : %u\n", seed);

	game g;
	memset(&g, 0, sizeof(game));
	FILE *fp = fopen("square_info.csv", "r");
	if (!fp){
		perror("Error opening square_info.csv");
		exit(1);
	}
	char typ[16];
	char name[48];
	fscanf(fp, " %*[^\n]");
	for (int i=0; i < SQUARES; i++){
		fscanf(fp, " %[^,],%[^,],", typ, name);
		g.square[i].type = square_type(typ);
		strncpy(g.square[i].name, name, sizeof(g.square[i].name) - 1);
		g.square[i].name[sizeof(g.square[i].name) - 1] = '\0';
		g.square[i].owner = NULL;
		g.square[i].prevBuy = NULL;
		g.square[i].nextofgroup = NULL;
		g.square[i].houses = 0;
		g.square[i].isMort = 0;
		g.square[i].buyPrice = 0;
		g.square[i].baseRent = 0;
		g.square[i].houseCost = 0;
		g.square[i].hotelCost = 0;
		g.square[i].group = NONE;
		switch (g.square[i].type){
			case PROPERTY:
				fscanf(fp,"%d,%d,%15[^,],%d,%d\n",
				&g.square[i].buyPrice, 
				&g.square[i].baseRent, 
				typ, 
				&g.square[i].houseCost, 
				&g.square[i].hotelCost
				);
				g.square[i].group = property_group(typ);
				break;
			case RAILWAY:
				fscanf(fp,"%d,%*[^\n]\n", &g.square[i].buyPrice);
				g.square[i].group = NONE;
				break;
			case UTILITY:
				fscanf(fp,"%d,%*[^\n]\n", &g.square[i].buyPrice);
				g.square[i].group = NONE;
				break;
			default:
				fscanf(fp, "%*[^\n]\n");
				g.square[i].group = property_group(name);
				break;
		}
	}
	fclose(fp);
	return g;
}

int dice(char *duble){
	int a = rand() % 6 + 1;
	int b = rand() % 6 + 1;
	if (a == b){
		*(duble) = 1;
	}else{
		*(duble) = 0;
	}
	return a+b;
}

void start_game(plyr *plyrs, int n, int cash){
	struct keyval{
		plyr a;
		int k;
	} p[n], tmp;
	char d;
	for (int i=0; i < n; i++){
		p[i].a = spawn_player(i);
		p[i].k = dice(&d);
	}

	for (int i=0; i < n; i++) printf("Player %d : %s\n", i+1,  p[i].a.name);
	printf("\nEach player begins with LKR %d\n\n", cash);
	for (int i=0; i < n; i++) printf("%s rolls %d\n", p[i].a.name, p[i].k);

	int has_tie = 1;
	while (has_tie){
		for (int i=0; i < n-1; i++){
			for (int j=0; j < n-1-i; j++){
				if (p[j].k < p[j+1].k){
					tmp = p[j];
					p[j] = p[j+1];
					p[j+1] = tmp;
				}
			}
		}
		has_tie = 0;
		for (int i=0; i < n-1; i++){
			if (p[i].k == p[i+1].k){
				has_tie = 1;
				p[i].k = dice(&d);
				p[i+1].k = dice(&d);
				printf("%s re-rolls %d\n", p[i].a.name, p[i].k);
				printf("%s re-rolls %d\n", p[i+1].a.name, p[i+1].k);
			}
		}
	}

	puts("");
	printf("%s will begin the game\n", p[0].a.name);
	puts("\nTurn order:");
	for (int i=0; i < n; i++){
		printf("%s\n", p[i].a.name);
		*(plyrs+i) = p[i].a;
	}
}

char isMonopoly(game *g, plyr *p, sqr *s){
	if (s->type != PROPERTY || s->group == NONE) return 0;
	for(int i=0; i < SQUARES; i++){
		if((g->square[i].group == s->group) && (g->square[i].type == PROPERTY)){
			if (g->square[i].owner != p){
				return 0;
			}
		}
	}
	return 1;
}

char canBuild(game *g, sqr *s){
	if (s->type != PROPERTY || s->group == NONE || s->houses >= 5) return 0;
	for(int i=0; i < SQUARES; i++){
		if((g->square[i].group == s->group) && (g->square[i].type == PROPERTY)){
			if (s->houses > g->square[i].houses){
				return 0;
			}
		}
	}
	return 1;
}
