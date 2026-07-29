//#include "types.h"
#include <stdlib.h>

int dice(char *duble){
	int a = rand();
	int b = rand();
	if (a == b){
		*(duble) = 1;
	}else{
		*(duble) = 0;
	}
	return a+b;
}
