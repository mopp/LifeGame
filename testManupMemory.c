#include<stdio.h>
#include<stdlib.h>

int main(void){
	int i, j, *bord, initBordSizeX, initBordSizeY, *p;
	initBordSizeX = 5;
	initBordSizeY = 10;

	bord = (int*)malloc(sizeof(int) * initBordSizeY * initBordSizeX);
	if(NULL == bord){
		fprintf(stderr, "*ERROE*\nMemory Allocation Error!\n");
		return -1;
	}

	// 初期化
	int size = initBordSizeX*initBordSizeY;
	while(0<=--size){
		*(bord + size) = size;
	}

	size = initBordSizeX*initBordSizeY;
	
	// 以下3つのアクセスは同値
	for(i=0;i<size;++i){
		printf("%2d - %p - %2d\n", i, (bord + i), *(bord + i));
	}

	for(i=0;i<initBordSizeY;++i){
		for(j=0;j<initBordSizeX;++j){
			p = (bord + i*initBordSizeX + j);
			/* printf("%2d,%2d - %p - %2d\n", i, j, p, *p); */
			printf("%2d - %p - %2d\n", i, p, *p);
		}
	}

	for(i=0;i<(initBordSizeX*initBordSizeY);i+=initBordSizeX){
		for(j=0;j<initBordSizeX;++j){
			p = (bord + i + j);
			/* printf("%2d,%2d - %p - %2d\n", i, j, p, *p); */
			printf("%2d - %p - %2d\n", i, p, *p);
		}
	}

	return 0;
}
