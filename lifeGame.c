/**
 * @file lifeGame.c
 * @brief Conway's Game of Life
 * @par 規則
 *  誕生 -- 死んでいるセルに隣接する生きたセルがちょうど3つあれば、次の世代が誕生する.\n
 *  生存 -- 生きているセルに隣接する生きたセルが2つか3つならば、次の世代でも生存する.\n
 *  過疎 -- 生きているセルに隣接する生きたセルが1つ以下ならば、過疎により死滅する.\n
 *  過密 -- 生きているセルに隣接する生きたセルが4つ以上ならば、過密により死滅する.\n
 * @version 0.1
 * @date 2012/11/08
 * @author mopp
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

/* #define DEBUG */
#define PUT_CELL_DEAD_CHAR printf("□")
#define PUT_CELL_LIVE_CHAR printf("■")
#define WAIT_TIME 1
#define CELL_SIZE 40
#define NUM_STEP 100

void printBord(int* const*, const int, const int, const int);

int inspectBord(int**, const int, const int);

int ifCellLiveAddCnt(const int);

int judgeNextCellState(const int, const int);

int judgeAllCellDead(int* const*, const int);

int* initMemSetNum(int**, const int, int);

void waitSecond(const double);

/**
 * 定数群.
 * セルの状態や検査時の結果などを表す
 */
typedef enum{
	CELL_UPDATE_ERROR,
	CELL_UPDATE_SUCCESS,
	CELL_STATE_DEAD,
	CELL_STATE_LIVE,
	CELL_STATE_STILL_LIFE,
	CELL_STATE_ALL_DEAD,
}StateCode;

int main(void){
	int i, *bord, initBordSizeX, initBordSizeY, geneCnt;
	StateCode state;

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
	initBordSizeX = 5;
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
	 * ex:initBordSizeX = 5, initBordSizeY = 10
	 * |00|01|02|03|04|
	 * |05|06|07|08|09|
	 * |10|11|12|13|14|
	 * ~~~~~~~~~~~~~~~~
	 * *(先頭アドレス + 行 + 列)
	 * *(bord + n*initBordSizeX + m) で n行目のm列を指す
	 */
	initMemSetNum(&bord, CELL_STATE_DEAD, initBordSizeX*initBordSizeY);

	/* 盤への入力 */
	for(i=0;i<initBordSizeY;++i){
		*(bord + (initBordSizeX*(initBordSizeY/2)) + i) = CELL_STATE_LIVE;
		/* *(bord + (i*initBordSizeY) + initBordSizeX/2) = CELL_STATE_LIVE; */
		*(bord + i*initBordSizeX + i) = *(bord + i*initBordSizeX + (initBordSizeX-i-1)) = CELL_STATE_LIVE;
	}

	// 左上角
	/* *(bord + 0*initBordSizeX + 0) = CELL_STATE_LIVE; */
	// 左下角
	/* *(bord + (initBordSizeY-1)*initBordSizeX + (0)) = CELL_STATE_LIVE; */
	// 右上角
	/* *(bord + 0*initBordSizeX + (initBordSizeX-1)) = CELL_STATE_LIVE; */
	// 右下角
	/* *(bord + (initBordSizeY-1)*initBordSizeX + (initBordSizeX-1)) = CELL_STATE_LIVE; */
	// 上端
	/* *(bord + (0)*initBordSizeX + (initBordSizeX/2)) = CELL_STATE_LIVE; */
	// 下端
	/* *(bord + (initBordSizeY-1)*initBordSizeX + (initBordSizeX/2)) = CELL_STATE_LIVE; */
	// 左端
	/* *(bord + (initBordSizeY/2)*initBordSizeX + (0)) = CELL_STATE_LIVE; */
	// 右端
	/* *(bord + (initBordSizeY/2)*initBordSizeX + (initBordSizeX-1)) = CELL_STATE_LIVE; */

	// 盤の捜査開始
	for(i=1;i<=geneCnt;++i){
		// 現在の盤を表示
		printBord(&bord, initBordSizeX, initBordSizeY, i);

		// 盤を更新
		state = inspectBord(&bord, initBordSizeX, initBordSizeY);

		// 終了処理
		if(CELL_STATE_STILL_LIFE == state){
			printf("%d Generation\nCell State is Still Lifes!\n", i);
			break;
		}else if(CELL_STATE_ALL_DEAD == state){
			printf("%d Generation\nCell is All Dead!\n", i);
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


/**
 * @brief 受け取った盤を出力
 * @param bord 動的に確保された盤
 * @param sizeX 盤の横方向の要素数
 * @param sizeY 盤の縦方向の要素数
 * @param generation 表示させる盤の世代数
 * @retrn void
 */
void printBord(int* const * bord, const int sizeX, const int sizeY, const int generation){
	int i, j, looplimit = sizeX * sizeY;

	// ターミナルクリア
	system("clear");

	// 世代と盤のサイズ出力
	printf("%dx%d Bord - %d Generation\n", sizeX, sizeY, generation);

	// 上端
	printf(" **");
	for(i=0;i<sizeX;++i){
		printf("**");
	}
	printf("**\n");

	// 盤を出力
	for(i=0;i<looplimit;i+=sizeX){
		printf(" * ");
		for(j=0;j<sizeX;++j){
			(CELL_STATE_LIVE == *(*(bord) + i + j))?(PUT_CELL_LIVE_CHAR):(PUT_CELL_DEAD_CHAR);
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


/**
 * @brief 受け取った盤を調べて次の世代を代入する
 * @param bord 動的に確保された盤
 * @param sizeX 盤の横方向の要素数
 * @param sizeY 盤の縦方向の要素数
 * @return bordState 検査結果
 */
int inspectBord(int** bord, const int sizeX, const int sizeY){
	int i, j, liveCellCnt, *nextBord, bordState = CELL_UPDATE_SUCCESS;
	const int endX = sizeX-1, endY = sizeY-1;
	const size_t bordMomerySize = sizeof(int) * sizeX * sizeY;

	// メモリ確保
	nextBord = (int*)malloc(bordMomerySize);
	if(NULL == nextBord){
		// メモリ確保失敗
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
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeX + j));
			}else{
				// 上端を下端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + endY*sizeX + j));
			}

			if(i < endY){
				// 下
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeX + j));
			}else{
				// 下端を上端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeX + j));
			}

			if(0 < j){
				// 左
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + i*sizeX + (j-1)));
			}else{
				// 左端を右端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + i*sizeX + endX));
			}

			if(j < endX){
				// 右
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + i*sizeX + (j+1)));
			}else{
				// 右端を左端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + i*sizeX + (0)));
			}

			// 左上のセルを確認
			if(i == 0 && j == 0){
				// 左上角の場合、右下角へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + endY*sizeX + endX));
			}else if(j == 0){
				// 左端の場合、一つ上の右端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeX + endX));
			}else if(i == 0){
				// 上端の場合、一つ左の下端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + endY*sizeX + (j-1)));
			}else{
				// そのまま左上へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeX + (j-1)));
			}

			// 右上のセルを確認
			if(0 == i && endX == j){
				// 右上角の場合、左下角へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + endY*sizeX + (0)));
			}else if(0 == i){
				// 上端の場合、一つ右の下端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + endY*sizeX + (j+1)));
			}else if(endX == j){
				// 右端の場合、一つ上の左端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeX + (0)));
			}else{
				// そのまま右上へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i-1)*sizeX + (j+1)));
			}

			// 左下のセルを確認
			if(i == endY && j == 0){
				// 左下角の場合、右上角へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeX + endX));
			}else if(endY == i){
				// 下端の場合、一つ左の上端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeX + (j-1)));
			}else if(0 == j){
				// 左端の場合、一つ下の右端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeX + endX));
			}else{
				// そのまま左下へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeX + (j-1)));
			}

			// 右下のセルを確認
			if(i == endY && j == endX){
				// 右下角の場合、左上角へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeX + (0)));
			}else if(endY == i){
				// 下端の場合、一つ右の上端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (0)*sizeX + (j+1)));
			}else if(endX == j){
				// 右端の場合、一つ下の左端へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeX + (0)));
			}else{
				// そのまま右下へ
				liveCellCnt += ifCellLiveAddCnt(*(*(bord) + (i+1)*sizeX + (j+1)));
			}

			// 現在セルの状態に応じて死滅と生存を判別
			*(nextBord + i*sizeX + j) = judgeNextCellState(*(*(bord) + i*sizeX + j), liveCellCnt);

#ifdef DEBUG
			if(0<liveCellCnt){
				printf("h-%d,w-%d, liveCellCnt is %d\n", i, j, liveCellCnt);
				waitSecond(1.0);
			}
#endif
		}
	}

	// 次の世代と現在の世代の盤が一致すれば中断
	if(!memcmp(*bord, nextBord, bordMomerySize)){
		if(CELL_STATE_DEAD == judgeAllCellDead(&nextBord, sizeX*sizeY)){
			// 全て死滅
			bordState = CELL_STATE_ALL_DEAD;
		}else{
			// セル状態変化なし
			bordState = CELL_STATE_STILL_LIFE;
		}
	}else{
		// 更新した盤をコピー
		memcpy(*bord, nextBord, bordMomerySize);

		// 通常の世代更新
		bordState = CELL_UPDATE_SUCCESS;
	}

	// メモリ解放
	free(nextBord);

	return bordState;
}


/**
 * @brief 受け取ったセルの生存確認 生存-1, 死滅-0
 * @param cellContent セルの値
 * @return 1or0 加算のため1か0のみ返す
 */
int ifCellLiveAddCnt(const int cellContent){
	return (cellContent == CELL_STATE_LIVE)?(1):(0);
}


/**
 * @brief 次の世代のセルの状態を判別する
 * @param nowCellState 現在のセル状態
 * @param liveCellNum 周囲の生存セル数
 * @return StateCode セルの状態
 */
int judgeNextCellState(const int nowCellState, const int liveCellNum){
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


/**
 * @brief セルがすべて死滅しているか否か判定
 * @param mem 動的確保された盤
 * @param セルの全要素数
 * @return StateCode
 */
int judgeAllCellDead(int* const* bord, const int size){
	int i;
	for(i=0;i<size;++i){
		if(CELL_STATE_LIVE == *(*bord + i)){
			return CELL_STATE_LIVE;
		}
	}

	return CELL_STATE_DEAD;
}


/**
 * @brief 受け取ったメモリをnで初期化する
 * @param mem 動的確保されたメモリ
 * @param n 埋める数値
 * @param size メモリの要素数
 */
int* initMemSetNum(int** mem, const int n, int size){
	while(0<=--size){
		*(*mem + size) = n;
	}
	return *mem;
}


/**
 * @brief t秒間待つ
 * @param t 停止する秒
 * @return void
 */
void waitSecond(const double t){
	time_t waitBeginTime = time(NULL);
	while(difftime(time(NULL), waitBeginTime) < t);
}
