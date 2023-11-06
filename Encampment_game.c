#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <math.h> //sin(), cos()を使用するためのヘッダファイルの読み込み
#include <string.h> //文字表示用
#include <time.h>

#pragma warning (disable: 4996)
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 300
#define SPEED 0.04
#define PENALTY 0.7
#define N 999
#define LIMIT 0.8

struct area {
	int area_type;
	double pos[2];
	double size;
	double color[3];
	int type;
	struct area* next;
};

//描画するエリア
struct area* newArea = NULL;

// 最初の構造体のポインタrootの宣言と初期化
struct area* root = NULL;

//ピンの本数
int Pin = 12;

//ピンの座標を格納する変数配列
double posPin[12][2];

// ピンの座標（ウィンドウ内ピクセル座標）
GLint posPin_wp[5][2];

//ピンの色
double colPin[3] = { 1.0, 1.0, 0.0 };

//クリックされた座標
double posclick[2];

// クリックされた座標（ウィンドウ内ピクセル座標）
GLint posclick_wp[2];

//クリックされているか
int click = 0;

// クリックされている時間
int click_T = 0;

// 1Pの色
double Col_1P[3] = { 0.8, 0.0, 0.0 };

//2Pの色
double Col_2P[3] = { 0.0, 0.0, 0.8 };

//背景色
double Col_Back[3] = { 0.0, 0.0, 0.0 };

//1Pの面積
double Area_1P;

//2Pの面積
double Area_2P;

//非陣地エリアの面積
double Area_Free;

//非陣地エリアが一定の面積以下のときは"Finish"とするためのフラグ
int flag_Finish = 0;

//現在のプレーヤー
int Player = 1;

// 押されたキーを表すグローバル変数
int KEY;

//特殊キーが押された時の処理を行う関数を指定
void checkNumberKeyPressed(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
		KEY = 1;
		break;
	case '2':
		KEY = 2;
		break;
	case '3':
		KEY = 3;
		break;
	case '4':
		KEY = 4;
		break;
	case '5':
		KEY = 5;
		break;
	}
	printf("%d", KEY);
}

//円を描画
void drawCircle(struct area* r)
{
	int i;
	glColor3d(r->color[0], r->color[1], r->color[2]);
	glBegin(GL_POLYGON); //ポリゴンの描画開始

	for (i = 0; i < 360; i += 360.0 / 36) {
		glVertex3f(
			r->pos[0] + r->size * cos(3.14 * i / 180),
			r->pos[1] + r->size * sin(3.14 * i / 180), 0.0);
	}
	glEnd();

	if (r->type == 1) {
		for (int i = 0; i < Pin; i++) {
			glColor3d(colPin[0], colPin[1], colPin[2]);
			glPointSize(10);
			glBegin(GL_POINTS);
			glVertex2d(posPin[i][0], posPin[i][1]);
			glEnd();
		}
	}
}

//正三角形の描画
void drawEq_Tri(struct area* r)
{
	double posVertex[3][3];
	double size2 = r->size / 2;
	double size3 = size2 * sqrt(3);

	if (r->type == 0) {
		posVertex[0][0] = r->pos[0];
		posVertex[0][1] = r->pos[1] + r->size;
		posVertex[0][2] = 0;
		posVertex[1][0] = r->pos[0] - size3;
		posVertex[1][1] = r->pos[1] - size2;
		posVertex[1][2] = 0;
		posVertex[2][0] = r->pos[0] + size3;
		posVertex[2][1] = r->pos[1] - size2;
		posVertex[2][2] = 0;
	}
	if (r->type == 1) {
		posVertex[0][0] = r->pos[0];
		posVertex[0][1] = r->pos[1] - r->size;
		posVertex[0][2] = 0;
		posVertex[1][0] = r->pos[0] - size3;
		posVertex[1][1] = r->pos[1] + size2;
		posVertex[1][2] = 0;
		posVertex[2][0] = r->pos[0] + size3;
		posVertex[2][1] = r->pos[1] + size2;
		posVertex[2][2] = 0;
	}

	int i;
	glColor3d(r->color[0], r->color[1], r->color[2]);
	glBegin(GL_TRIANGLES);
	for (i = 0; i < 3; i++) {
		glVertex3f(posVertex[i][0], posVertex[i][1], posVertex[i][2]);
	}
	glEnd();
}

//正方形の描画
void drawSqu(struct area* r)
{
	double posVertex[4][3];
	double size2 = r->size / sqrt(2);

	if (r->type == 0) {
		posVertex[0][0] = r->pos[0] + size2;
		posVertex[0][1] = r->pos[1] + size2;
		posVertex[0][2] = 0;
		posVertex[1][0] = r->pos[0] - size2;
		posVertex[1][1] = r->pos[1] + size2;
		posVertex[1][2] = 0;
		posVertex[2][0] = r->pos[0] - size2;
		posVertex[2][1] = r->pos[1] - size2;
		posVertex[2][2] = 0;
		posVertex[3][0] = r->pos[0] + size2;
		posVertex[3][1] = r->pos[1] - size2;
		posVertex[3][2] = 0;
	}
	if (r->type == 1) {
		posVertex[0][0] = r->pos[0];
		posVertex[0][1] = r->pos[1] + r->size;
		posVertex[0][2] = 0;
		posVertex[1][0] = r->pos[0] - r->size;
		posVertex[1][1] = r->pos[1];
		posVertex[1][2] = 0;
		posVertex[2][0] = r->pos[0];
		posVertex[2][1] = r->pos[1] - r->size;
		posVertex[2][2] = 0;
		posVertex[3][0] = r->pos[0] + r->size;
		posVertex[3][1] = r->pos[1];
		posVertex[3][2] = 0;
	}

	int i;
	glColor3d(r->color[0], r->color[1], r->color[2]);
	glBegin(GL_TRIANGLE_FAN);
	for (i = 0; i < 4; i++) {
		glVertex3d(posVertex[i][0], posVertex[i][1], posVertex[i][2]);
	}
	glEnd();
}

//エリア描画関数
void drawArea(struct area* d)
{
	printf("\n%d\n{%f, %f}\n%f\n{%f, %f, %f}\n%d\n%p\n",
		d->area_type, d->pos[0], d->pos[1], d->size, d->color[0], d->color[1], d->color[2], d->type, d->next);
	
	switch (d->area_type) {
	case 1:
		drawCircle(d);
		break;
	case 2:
		drawEq_Tri(d);
		break;
	case 3:
		drawEq_Tri(d);
		break;
	case 4:
		drawSqu(d);
		break;
	case 5:
		drawSqu(d);
		break;
	}

	if (d->next != NULL) {
		drawArea(d->next);
	}
}

// 新しいエリアをエリアリストに追加する関数
void insert(struct area* p)
{
	struct area* curArea; // 構造体ポインタcurAreaを宣言

	curArea = (struct area*)malloc(sizeof(struct area)); // メモリ確保
	if (root == NULL) {
		root = p;  // 関数に渡されたアドレスをrootにコピー
		return;
	}

	curArea = root;
	while (curArea->next != NULL) {
		curArea = curArea->next;
	}

	curArea->next = p;
	return;
}

// 新しいノードを作成する関数
void make_node()
{
	int i;
	newArea = (struct area*)malloc(sizeof(struct area));
	newArea->size = SPEED * click_T;

	switch (KEY) {
	case 1:
		newArea->area_type = 1;
		newArea->type = 0;
		break;
	case 2:
		newArea->area_type = 2;
		newArea->type = 0;
		break;
	case 3:
		newArea->area_type = 3;
		newArea->type = 1;
		break;
	case 4:
		newArea->area_type = 4;
		newArea->type = 0;
		break;
	case 5:
		newArea->area_type = 5;
		newArea->type = 1;
		break;
	}

	for (i = 0; i < 2; i++) {
		newArea->pos[i] = posclick[i];
	}

	for (i = 0; i < 3; i++) {
		if (Player == 1) {
			newArea->color[i] = Col_1P[i];
		}
		else {
			newArea->color[i] = Col_2P[i];
		}
	}

	// 次の構造体へのリンクを初期化
	newArea->next = NULL;

	// 新しい構造体newCirのアドレスを関数insertに渡す
	insert(newArea);
}

//新しいペナルティノードを作成する関数
void make_node_p(double pos_p[])
{
	int i;

	// 新しいノードの作成
	newArea = (struct area*)malloc(sizeof(struct area));
	newArea->area_type = 1;
	newArea->type = 1;

	for (i = 0; i < 2; i++) {
		newArea->pos[i] = pos_p[i];
	}

	if (Player == 2) {
		newArea->color[0] = 0.8;
		newArea->color[1] = 0.0;
		newArea->color[2] = 0.0;
	}
	else {
		newArea->color[0] = 0.0;
		newArea->color[1] = 0.0;
		newArea->color[2] = 0.8;
	}

	newArea->size = PENALTY;

	// 次の構造体へのリンクを初期化
	newArea->next = NULL;

	// 新しい構造体newAreaのアドレスを関数insertに渡す
	insert(newArea);
}

//画面を描画
void display(void)
{
	int i;

	//バッファをクリア
	glClear(GL_COLOR_BUFFER_BIT);

	// ピンの描画
	for (i = 0; i < Pin; i++) {
		glColor3d(colPin[0], colPin[1], colPin[2]);
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex2d(posPin[i][0], posPin[i][1]);
		glEnd();
	}

	// エリアリストを描画
	if (root != NULL) {
		drawArea(root);
	}

	//ゲーム終了時に表示する文字列
	char str_finish[20];
	unsigned int j;
	if (flag_Finish == 1) {
		glClear(GL_COLOR_BUFFER_BIT);
		sprintf(str_finish, "RED  Win!!");
		glColor3d(Col_1P[0], Col_1P[1], Col_1P[2]);
		glRasterPos2d(-0.3, 0.0);
		for (j = 0; j < strlen(str_finish); j++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str_finish[j]);
		}
	}
	if (flag_Finish == 2) {
		glClear(GL_COLOR_BUFFER_BIT);
		sprintf(str_finish, "BLUE  Win!!");
		glColor3d(Col_2P[0], Col_2P[1], Col_2P[2]);
		glRasterPos2d(-0.3, 0.0);
		for (j = 0; j < strlen(str_finish); j++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str_finish[j]);
		}
	}
	if (flag_Finish == 3) {
		glClear(GL_COLOR_BUFFER_BIT);
		sprintf(str_finish, "Draw...");
		glColor3d(1.0, 1.0, 1.0);
		glRasterPos2d(-0.2, 0.0);
		for (j = 0; j < strlen(str_finish); j++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str_finish[j]);
		}
	}

	glFlush();
	glutSwapBuffers(); //ダブルブッファを交換
}

//面積算出関数
double cal_Area(double col[3])
{
	int i, A = 0;
	double B;
	GLfloat c[3];

	srand((unsigned)time(NULL));
	for (i = 0; i < N; i++) {
		int x = rand() % (WINDOW_WIDTH + 1);
		int y = rand() % (WINDOW_HEIGHT + 1);
		glReadBuffer(GL_FRONT);
		glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, c);
		if (floor((double)100 * c[0]) == 100 * col[0] && floor((double)100 * c[1]) == 100 * col[1] && floor((double)100 * c[2]) == 100 * col[2]) {
			A++;
		}
	}

	B =(double)100 * A / N;
	return B;
}

//再描画関数（一定時間ごと）
void updatePos(int value)
{
	if (flag_Finish == 0) {
		if (click == 1) {
			click_T++;
			// 新しいノードの作成
			make_node();
		}
		glutPostRedisplay(); //画面の再描画

		GLfloat c[3];
		for (int i = 0; i < Pin; i++) {
			//ピンの座標をウィンドウ内ピクセルX座標へ変換
			posPin_wp[i][0] = (GLint)WINDOW_WIDTH * (1 + posPin[i][0]) / 2;  //X座標
			   posPin_wp[i][1] = (GLint)WINDOW_HEIGHT * (1 + posPin[i][1]) / 2;  //Y座標
			//ピンのピクセル座標の色の取得と判定
			glReadPixels(posPin_wp[i][0], posPin_wp[i][1], 1, 1, GL_RGB, GL_FLOAT, c);
			if (floor((double)100 * c[0]) == 100 * Col_1P[0] || floor((double)100 * c[2]) == 100 * Col_2P[2]) {
				make_node_p(posPin[i]);
				click = 0;    //クリック状態のリセット
				click_T = 0;  //クリック時間のリセット
				glutPostRedisplay(); //画面の再描画
			}
		}

		// クリックされていないときに面積の計算を行う
		if (click == 0) {
			Area_1P = cal_Area(Col_1P);
			Area_2P = cal_Area(Col_2P);
			Area_Free = 100 - Area_1P - Area_2P;
			printf("                  \n\n\nA1: %lf      A2:%lf         AF:%lf\n\n", Area_1P, Area_2P, Area_Free);
			// 非陣地エリアが一定面積以下でゲーム終了フラグ
			if (Area_Free < LIMIT) {
				if (Area_1P > Area_2P) {
					flag_Finish = 1;
				}
				if (Area_1P < Area_2P) {
					flag_Finish = 2;
				}
				if (Area_1P == Area_2P) {
					flag_Finish = 3;
				}
			}
		}
	}

	glutTimerFunc(20, updatePos, 0); // 一定時間ごとに、この関数を実行
	return;
}

void mouse(int button, int state, int x, int y)
{
	int i;

	//左ボタン以外の操作は無視
	if (button != GLUT_LEFT_BUTTON) {
		return;
	}

	//左ボタンが押された場合
	if (state == GLUT_DOWN) { 
		posclick[0] = (double)x / WINDOW_WIDTH * 2 - 1,    //空間x座標の格納
			posclick[1] = -(double)y / WINDOW_HEIGHT * 2 + 1; //空間y座標の格納

		posclick_wp[0] = (GLint)WINDOW_WIDTH * (1 + posclick[0]) / 2; //ウィンドウ内ピクセルX座標へ変換
		    posclick_wp[1] = (GLint)WINDOW_HEIGHT * (1 + posclick[1]) / 2; //ウィンドウ内ピクセルy座標へ変換

		GLfloat c[3];
		glReadBuffer(GL_FRONT);
		glReadPixels(posclick_wp[0], posclick_wp[1], 1, 1, GL_RGB, GL_FLOAT, c);

		// クリックした座標が背景色だった場合のみ実行
		if (c[0] == Col_Back[0] && c[1] == Col_Back[1] && c[2] == Col_Back[2]) {
			click = 1;
			if (Player == 2) {
				Player = 1;
			}
			else {
				Player = 2;
			}
		}
	}

	//左ボタンが放された場合
	if (state == GLUT_UP) { 
		click = 0;
		click_T = 0;
	}
}

//初期化
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);

	int i;
	srand((unsigned)time(NULL));

	//ピンのランダム配置
	for (i = 0; i < 12; i++) {
		posPin[i][0] = (double)((rand() % 201) - 100) / 100;
		posPin[i][1] = (double)((rand() % 201) - 100) / 100;
	}

	for (i = 0; i < Pin; i++) {
		printf("%d: ( %f, %f )\n", i + 1, posPin[i][0], posPin[i][1]);
	}
}

//メイン関数
int main(int argc, char* argv[])
{
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInit(&argc, argv); //GLUTライブラリを初期化

	//表示モードをダブルブッファリングに設定
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow(argv[0]); //ウィンドウを生成
	glutDisplayFunc(display); //画面描画を行う関数を指定

	//キーが押された（戻された）ときの処理を行う関数を指定
	glutKeyboardFunc(checkNumberKeyPressed);

	//一定時間ごとに繰り返す関数
	glutTimerFunc(500, updatePos, 0);

	//マウスがクリックされた時の処理を行う関数
	glutMouseFunc(mouse);

	init(); //初期化
	glutMainLoop(); //メインループ

	return EXIT_SUCCESS;
}