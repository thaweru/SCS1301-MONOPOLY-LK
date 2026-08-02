#include "types.h"
#include <stdlib.h>
#include <time.h>

bd init_game(int n, char **arr){
	unsigned int seed;
	if (n == 2){
		char *str = arr[1];
		char *endptr;
		seed = strtol(str, &endptr, 10) % UINT_MAX;
	}else{
		seed = time(NULL);
	}
	srand(seed);
	printf("Seed : %u\n", seed);

	bd bord;
	FILE *fp = fopen("square_info.csv", "r");
	char li[128];
	fscanf(fp, " %[^\n]", li);
	for (int i=0; i < 40; i++){
		fscanf(fp, "%[^,],", li);
		fscanf(fp, "%[^\n]\n", li);
		puts(li);
	}
//	bord.s[0] = {0, "GO"};

	return bord;
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

void start_game(){
}
