/****************************************
 * Conway's Game of Life
 *
 * 誕生
 *  死んでいるセルに隣接する生きたセルがちょうど3つあれば、次の世代が誕生する。
 * 生存
 *  生きているセルに隣接する生きたセルが2つか3つならば、次の世代でも生存する。
 * 過疎
 *  生きているセルに隣接する生きたセルが1つ以下ならば、過疎により死滅する。
 * 過密
 *  生きているセルに隣接する生きたセルが4つ以上ならば、過密により死滅する。 
 ****************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define DEBUG
#define PUT_CELL_DEAD_CHAR printf("□")
#define PUT_CELL_LIVE_CHAR printf("■")
#define WAIT_TIME 1
#define CELL_SIZE 40
#define NUM_STEP 100

// 定数群
typedef enum{
	CELL_UPDATE_ERROR,
	CELL_UPDATE_SUCCESS,
	CELL_STATE_DEAD,
	CELL_STATE_LIVE,
	CELL_STATE_STILL_LIFE,
}Constants;

/****************************************
 * 指定された盤を出力
 * @param bord 1次元でメモリ確保された盤
 * @param sizeX 盤の横方向のサイズ
 * @param sizeY 盤の縦方向のサイズ
 * @param cnt 世代数
 ****************************************/
void printBord(int **, int , int, int);

/****************************************
 * 盤を調べ世代更新をする
 * @param bord 1次元でメモリ確保された更新される盤
 * @param sizeX 盤の横方向のサイズ
 * @param sizeY 盤の縦方向のサイズ
 * @return 成功した場合CELL_UPDATE_SUCCESS
 ****************************************/
int inspectBord(int **, int , int);

/****************************************
 * セルの生存確認
 * 生存していれば1、死滅していれば0
 * @param cellContent セルの値
 ****************************************/
int ifCellLiveAddCnt(int);

/****************************************
 * 動的確保したメモリをint数値で初期化
 *
 * @param mem 1次元で動的確保されたメモリ
 * @param n 埋める数値
 * @param size 確保したメモリサイズ
 ****************************************/
int* initMemSetNum(int**, int, int);

/****************************************
 * 指定された秒間待つ
 * @param t 指定する秒
 ****************************************/
void waitSecond(double);

/****************************************
 * 次の世代のセルの状態を返す
 * @param nowCellState 現在の世代のセル状態
 * @param liveCellNum 周囲の生存セル数
 * @return セル状態
 ****************************************/
int judgeNextCellState(int, int);


int main(void){
	int i, *bord, initBordSizeX, initBordSizeY, geneCnt;
	Constants state;

	// ターミナルクリア
	system("clear");

	// TODO:読み込みの関数化
#ifndef DEBUG
	// 盤のサイズ入力
	do{
		printf("Please Input Cell Size (X,Y > 1) X Y > ");

		if(2 != scanf(" %d %d", &initBordSizeX, &initBordSizeY)){
			// 読み飛ばし
			scanf("%*s");
		}
	}while(initBordSizeX < 2 || initBordSizeY < 2);

	// 計算する世代数入力
	do{
		printf("Please Input Calculation Generation > ");

		if(1 != scanf(" %d", &geneCnt)){
			// 読み飛ばし
			scanf("%*s");
		}
	}while(geneCnt < 1);
#else
	initBordSizeX = 10;
	initBordSizeY = 10;
	geneCnt = 20;
#endif

	// メモリ確保
	bord = (int*)malloc(sizeof(int) * initBordSizeY * initBordSizeX);
	if(NULL == bord){
		fprintf(stderr, "*ERROE*\nMemory Allocation Error!\n");
		return -1;
	}

	/*
	 * 盤を初期化
	 * *(先頭アドレス + 縦 + 横)
	 * *(bord + n*initBordSizeY + m) で n行目のm列を指す
	 */
	initMemSetNum(&bord, CELL_STATE_DEAD, initBordSizeX*initBordSizeY);
	/*
	 * for(i=0;i<initBordSizeY;++i){
	 *     *(bord + (initBordSizeY*(initBordSizeY/2)) + i) = CELL_STATE_LIVE;
	 *     *(bord + (i*initBordSizeY) + initBordSizeX/2) = CELL_STATE_LIVE;
	 *     *(bord + i*initBordSizeY + i) = *(bord + i*initBordSizeY + (initBordSizeX-i-1)) = CELL_STATE_LIVE;
	 * }
	 */

	// 左上角
	*(bord + 0*initBordSizeY + 0) = CELL_STATE_LIVE;
	// 左下角
	*(bord + (initBordSizeY-1)*initBordSizeY + (0)) = CELL_STATE_LIVE;
	// 右上角
	*(bord + 0*initBordSizeY + (initBordSizeX-1)) = CELL_STATE_LIVE;
	// 右下角
	*(bord + (initBordSizeY-1)*initBordSizeY + (initBordSizeX-1)) = CELL_STATE_LIVE;
	// 上端
	/* *(bord + (0)*initBordSizeY + (initBordSizeX/2)) = CELL_STATE_LIVE; */
	// 下端
	/* *(bord + (initBordSizeY-1)*initBordSizeY + (initBordSizeX/2)) = CELL_STATE_LIVE; */
	// 左端
	/* *(bord + (initBordSizeY/2)*initBordSizeY + (0)) = CELL_STATE_LIVE; */
	// 右端
	/* *(bord + (initBordSizeY/2)*initBordSizeY + (initBordSizeX-1)) = CELL_STATE_LIVE; */

	/* *(bord + 1*initBordSizeY + 1) = CELL_STATE_LIVE; */

	/*
	 *(bord + 5*initBordSizeY + 5) = *(bord + 6*initBordSizeY + 5) = *(bord + 7*initBordSizeY + 5) = CELL_STATE_LIVE;
	 *(bord + 8*initBordSizeY + 5) = *(bord + 8*initBordSizeY + 7) = *(bord + 9*initBordSizeY + 5) = CELL_STATE_LIVE;
	 *(bord + 10*initBordSizeY + 6) = *(bord + 10*initBordSizeY + 7) = *(bord + 10*initBordSizeY + 5) = CELL_STATE_LIVE;
	 *(bord + 12*initBordSizeY + 10) = *(bord + 11*initBordSizeY + 6) = *(bord + 11*initBordSizeY + 8) = CELL_STATE_LIVE;
	 *(bord + 11*initBordSizeY + 10) = *(bord + 12*initBordSizeY + 6) = *(bord + 12*initBordSizeY + 9) = CELL_STATE_LIVE;
	 */

	// 盤の捜査開始
	for(i=1;i<=geneCnt;++i){
		// 現在の盤を表示
		printBord(&bord, initBordSizeX, initBordSizeY, i);

		// 盤を更新
		state = inspectBord(&bord, initBordSizeX, initBordSizeY);

		// 中断処理
		if(CELL_STATE_STILL_LIFE == state){
			printf("%d Generation\nCell State is Still Lifes!\n", i);
			break;
		}else if(CELL_UPDATE_ERROR == state){
			fprintf(stderr, "%d Generation\nCell Update Error!\n", i);
			break;
		}
	}

	// メモリ解放
	free(bord);

	return 0;
}


void printBord(int** bord, int sizeX, int sizeY, int generation){
	int i, j, nowY;

	// ターミナルクリア
	system("clear");

	// 世代と盤のサイズ出力
	printf("%dx%d Bord - %dGeneration\n", sizeX, sizeY, generation);

	// 上端
	printf(" **");
	for(i=0;i<sizeX;++i){
		printf("**");
	}
	printf("**\n");

	// 盤を出力
	for(i=0;i<sizeY;++i){
		printf(" * ");
		nowY = i*sizeY;
		for(j=0;j<sizeX;++j){
			(CELL_STATE_LIVE == *(*(bord) + nowY + j))?(PUT_CELL_LIVE_CHAR):(PUT_CELL_DEAD_CHAR);
		}
		printf(" *\n");
	}

	// 下端
	printf(" *");
	for(i=0;i<sizeX;++i){
		printf("**");
	}
	printf("***\n");

	// WAIT_TIME秒間待つ
	waitSecond(WAIT_TIME);
}


int inspectBord(int **bord, const int sizeX, const int sizeY){
	int i, j, liveCellCnt, *nextBord, endX = sizeX-1, endY = sizeY-1;
	size_t bordMomerySize = sizeof(int) * sizeX * sizeY;

	// メモリ確保
	nextBord = (int*)malloc(bordMomerySize);
	if(NULL == nextBord){
		return CELL_UPDATE_ERROR;
	}

	//初期化
	initMemSetNum(&nextBord, CELL_STATE_DEAD, sizeX * sizeY);

	/*
	 * 隣接セルを検査する
	 * 端ならば反対側へ接続
	 * i : 縦
	 * j : 横
	 */
	for(i=0;i<sizeY;++i){
		for(j=0;j<sizeX;++j){
			liveCellCnt = 0;

			if(0 < i){
				// 上
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeY + j));
			}else{
				// 上端を下端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (sizeY-1)*sizeY + j));
			}

			if(i < (sizeY-1)){
				// 下
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeY + j));
			}else{
				// 下端を上端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeY + j));
			}

			if(0 < j){
				// 左
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + i*sizeY + (j-1)));
			}else{
				// 左端を右端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + i*sizeY + (sizeX-1)));
			}

			if(j < sizeX-1){
				// 右
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + i*sizeY + (j+1)));
			}else{
				// 右端を左端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + i*sizeY + (0)));
			}

			// 左上のセルを確認
			if(i == 0 && j == 0){
				// 左上角の場合、右下角へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (sizeY-1)*sizeY + (sizeX-1)));
			}else if(j == 0){
				// 左端の場合、一つ上の右端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeY + (sizeX-1)));
			}else if(i == 0){
				// 上端の場合、一つ左の下端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (sizeY-1)*sizeY + (j-1)));
			}else{
				// そのまま左上へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeY + (j-1)));
			}

			// 右上のセルを確認
			if(0 == i && (sizeX-1) == j){
				// 右上角の場合、左下角へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (sizeY-1)*sizeY + (0)));
			}else if(0 == i){
				// 上端の場合、一つ右の下端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (sizeY-1)*sizeY + (j+1)));
			}else if((sizeX-1) == j){
				// 右端の場合、一つ上の左端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeY + (0)));
			}else{
				// そのまま右上へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeY + (j+1)));
			}

			// 左下のセルを確認
			if(i == (sizeY-1) && j == 0){
				// 左下角の場合、右上角へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeY + (sizeX-1)));
			}else if((sizeY-1) == i){
				// 下端の場合、一つ左の上端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeY + (j-1)));
			}else if(0 == j){
				// 左端の場合、一つ下の右端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeY + (sizeX-1)));
			}else{
				// そのまま左下へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeY + (j-1)));
			}

			// 右下のセルを確認
			if(i == (sizeY-1) && j == (sizeX-1)){
				// 右下角の場合、左上角へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeY + (0)));
			}else if((sizeY-1) == i){
				// 下端の場合、一つ右の上端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeY + (j+1)));
			}else if((sizeX-1) == j){
				// 右端の場合、一つ下の左端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeY + (0)));
			}else{
				// そのまま右下へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeY + (j+1)));
			}

			// 現在セルの状態に応じて死滅と生存を判別
			*(nextBord + i*sizeY + j) = judgeNextCellState(*(*(bord) + i*sizeY + j), liveCellCnt);

			if(0<liveCellCnt){
				printf("h-%d,w-%d, liveCellCnt is %d\n", i, j, liveCellCnt);
				waitSecond(1.0);
			}
		}
	}

	// 次の世代と現在の世代の盤が一致すれば中断
	if(!memcmp(*bord, nextBord, bordMomerySize)){
		// メモリ解放
		free(nextBord);

		return CELL_STATE_STILL_LIFE;
	}else{
		// 更新した盤をコピー
		memcpy(*bord, nextBord, bordMomerySize);

		// メモリ解放
		free(nextBord);

		return CELL_UPDATE_SUCCESS;
	}
}


int ifCellLiveAddCnt(int cellContent){
	return (cellContent == CELL_STATE_LIVE)?(1):(0);
}


void waitSecond(double t){
	size_t waitBeginTime = time(NULL);

	while(difftime(time(NULL), waitBeginTime) < t);
}


int* initMemSetNum(int **mem, int n, int size){
	while(0<=--size){
		*(*mem + size) = n;
	}

	return *mem;
}

int judgeNextCellState(int nowCellState, int liveCellNum){
	int nextState = nowCellState;

	// 現在セルの状態で分岐
	switch(nowCellState){
		case CELL_STATE_LIVE:
			if(2 == liveCellNum || 3 == liveCellNum){
				// 生存 - 生きているセルに隣接する生きたセルが2つか3つならば、次の世代でも生存する。
				nextState =  CELL_STATE_LIVE;
			}else if(liveCellNum <= 1){
				// 過疎 - 生きているセルに隣接する生きたセルが1つ以下ならば、過疎により死滅する。
				nextState =  CELL_STATE_DEAD;
			}else if(4 <= liveCellNum){
				// 過密 - 生きているセルに隣接する生きたセルが4つ以上ならば、過密により死滅する。
				nextState =  CELL_STATE_DEAD;
			}
			break;
		case CELL_STATE_DEAD:
			if(3 == liveCellNum){
				// 誕生 - 死んでいるセルに隣接する生きたセルがちょうど3つあれば、次の世代が誕生する。
				nextState =  CELL_STATE_LIVE;
			}
			break;
	}

	return nextState;
}
