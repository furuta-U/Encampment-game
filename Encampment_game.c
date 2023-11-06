#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <math.h> //sin(), cos()���g�p���邽�߂̃w�b�_�t�@�C���̓ǂݍ���
#include <string.h> //�����\���p
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

//�`�悷��G���A
struct area* newArea = NULL;

// �ŏ��̍\���̂̃|�C���^root�̐錾�Ə�����
struct area* root = NULL;

//�s���̖{��
int Pin = 12;

//�s���̍��W���i�[����ϐ��z��
double posPin[12][2];

// �s���̍��W�i�E�B���h�E���s�N�Z�����W�j
GLint posPin_wp[5][2];

//�s���̐F
double colPin[3] = { 1.0, 1.0, 0.0 };

//�N���b�N���ꂽ���W
double posclick[2];

// �N���b�N���ꂽ���W�i�E�B���h�E���s�N�Z�����W�j
GLint posclick_wp[2];

//�N���b�N����Ă��邩
int click = 0;

// �N���b�N����Ă��鎞��
int click_T = 0;

// 1P�̐F
double Col_1P[3] = { 0.8, 0.0, 0.0 };

//2P�̐F
double Col_2P[3] = { 0.0, 0.0, 0.8 };

//�w�i�F
double Col_Back[3] = { 0.0, 0.0, 0.0 };

//1P�̖ʐ�
double Area_1P;

//2P�̖ʐ�
double Area_2P;

//��w�n�G���A�̖ʐ�
double Area_Free;

//��w�n�G���A�����̖ʐψȉ��̂Ƃ���"Finish"�Ƃ��邽�߂̃t���O
int flag_Finish = 0;

//���݂̃v���[���[
int Player = 1;

// �����ꂽ�L�[��\���O���[�o���ϐ�
int KEY;

//����L�[�������ꂽ���̏������s���֐����w��
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

//�~��`��
void drawCircle(struct area* r)
{
	int i;
	glColor3d(r->color[0], r->color[1], r->color[2]);
	glBegin(GL_POLYGON); //�|���S���̕`��J�n

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

//���O�p�`�̕`��
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

//�����`�̕`��
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

//�G���A�`��֐�
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

// �V�����G���A���G���A���X�g�ɒǉ�����֐�
void insert(struct area* p)
{
	struct area* curArea; // �\���̃|�C���^curArea��錾

	curArea = (struct area*)malloc(sizeof(struct area)); // �������m��
	if (root == NULL) {
		root = p;  // �֐��ɓn���ꂽ�A�h���X��root�ɃR�s�[
		return;
	}

	curArea = root;
	while (curArea->next != NULL) {
		curArea = curArea->next;
	}

	curArea->next = p;
	return;
}

// �V�����m�[�h���쐬����֐�
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

	// ���̍\���̂ւ̃����N��������
	newArea->next = NULL;

	// �V�����\����newCir�̃A�h���X���֐�insert�ɓn��
	insert(newArea);
}

//�V�����y�i���e�B�m�[�h���쐬����֐�
void make_node_p(double pos_p[])
{
	int i;

	// �V�����m�[�h�̍쐬
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

	// ���̍\���̂ւ̃����N��������
	newArea->next = NULL;

	// �V�����\����newArea�̃A�h���X���֐�insert�ɓn��
	insert(newArea);
}

//��ʂ�`��
void display(void)
{
	int i;

	//�o�b�t�@���N���A
	glClear(GL_COLOR_BUFFER_BIT);

	// �s���̕`��
	for (i = 0; i < Pin; i++) {
		glColor3d(colPin[0], colPin[1], colPin[2]);
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex2d(posPin[i][0], posPin[i][1]);
		glEnd();
	}

	// �G���A���X�g��`��
	if (root != NULL) {
		drawArea(root);
	}

	//�Q�[���I�����ɕ\�����镶����
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
	glutSwapBuffers(); //�_�u���u�b�t�@������
}

//�ʐώZ�o�֐�
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

//�ĕ`��֐��i��莞�Ԃ��Ɓj
void updatePos(int value)
{
	if (flag_Finish == 0) {
		if (click == 1) {
			click_T++;
			// �V�����m�[�h�̍쐬
			make_node();
		}
		glutPostRedisplay(); //��ʂ̍ĕ`��

		GLfloat c[3];
		for (int i = 0; i < Pin; i++) {
			//�s���̍��W���E�B���h�E���s�N�Z��X���W�֕ϊ�
			posPin_wp[i][0] = (GLint)WINDOW_WIDTH * (1 + posPin[i][0]) / 2;  //X���W
			   posPin_wp[i][1] = (GLint)WINDOW_HEIGHT * (1 + posPin[i][1]) / 2;  //Y���W
			//�s���̃s�N�Z�����W�̐F�̎擾�Ɣ���
			glReadPixels(posPin_wp[i][0], posPin_wp[i][1], 1, 1, GL_RGB, GL_FLOAT, c);
			if (floor((double)100 * c[0]) == 100 * Col_1P[0] || floor((double)100 * c[2]) == 100 * Col_2P[2]) {
				make_node_p(posPin[i]);
				click = 0;    //�N���b�N��Ԃ̃��Z�b�g
				click_T = 0;  //�N���b�N���Ԃ̃��Z�b�g
				glutPostRedisplay(); //��ʂ̍ĕ`��
			}
		}

		// �N���b�N����Ă��Ȃ��Ƃ��ɖʐς̌v�Z���s��
		if (click == 0) {
			Area_1P = cal_Area(Col_1P);
			Area_2P = cal_Area(Col_2P);
			Area_Free = 100 - Area_1P - Area_2P;
			printf("                  \n\n\nA1: %lf      A2:%lf         AF:%lf\n\n", Area_1P, Area_2P, Area_Free);
			// ��w�n�G���A�����ʐψȉ��ŃQ�[���I���t���O
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

	glutTimerFunc(20, updatePos, 0); // ��莞�Ԃ��ƂɁA���̊֐������s
	return;
}

void mouse(int button, int state, int x, int y)
{
	int i;

	//���{�^���ȊO�̑���͖���
	if (button != GLUT_LEFT_BUTTON) {
		return;
	}

	//���{�^���������ꂽ�ꍇ
	if (state == GLUT_DOWN) { 
		posclick[0] = (double)x / WINDOW_WIDTH * 2 - 1,    //���x���W�̊i�[
			posclick[1] = -(double)y / WINDOW_HEIGHT * 2 + 1; //���y���W�̊i�[

		posclick_wp[0] = (GLint)WINDOW_WIDTH * (1 + posclick[0]) / 2; //�E�B���h�E���s�N�Z��X���W�֕ϊ�
		    posclick_wp[1] = (GLint)WINDOW_HEIGHT * (1 + posclick[1]) / 2; //�E�B���h�E���s�N�Z��y���W�֕ϊ�

		GLfloat c[3];
		glReadBuffer(GL_FRONT);
		glReadPixels(posclick_wp[0], posclick_wp[1], 1, 1, GL_RGB, GL_FLOAT, c);

		// �N���b�N�������W���w�i�F�������ꍇ�̂ݎ��s
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

	//���{�^���������ꂽ�ꍇ
	if (state == GLUT_UP) { 
		click = 0;
		click_T = 0;
	}
}

//������
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);

	int i;
	srand((unsigned)time(NULL));

	//�s���̃����_���z�u
	for (i = 0; i < 12; i++) {
		posPin[i][0] = (double)((rand() % 201) - 100) / 100;
		posPin[i][1] = (double)((rand() % 201) - 100) / 100;
	}

	for (i = 0; i < Pin; i++) {
		printf("%d: ( %f, %f )\n", i + 1, posPin[i][0], posPin[i][1]);
	}
}

//���C���֐�
int main(int argc, char* argv[])
{
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInit(&argc, argv); //GLUT���C�u������������

	//�\�����[�h���_�u���u�b�t�@�����O�ɐݒ�
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow(argv[0]); //�E�B���h�E�𐶐�
	glutDisplayFunc(display); //��ʕ`����s���֐����w��

	//�L�[�������ꂽ�i�߂��ꂽ�j�Ƃ��̏������s���֐����w��
	glutKeyboardFunc(checkNumberKeyPressed);

	//��莞�Ԃ��ƂɌJ��Ԃ��֐�
	glutTimerFunc(500, updatePos, 0);

	//�}�E�X���N���b�N���ꂽ���̏������s���֐�
	glutMouseFunc(mouse);

	init(); //������
	glutMainLoop(); //���C�����[�v

	return EXIT_SUCCESS;
}