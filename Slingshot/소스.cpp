#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <mmsystem.h>
#define PIE 3.141592
#define BackGround_R 600
#define GRAVITY 0.4
#define GAUGEWIDTH 600
#define GAUGEHEIGHT 30
#define SCREENWIDTH 1000
#define SCREENHEIGHT 800

GLubyte *LoadIBitmap(const char *filename, BITMAPINFO **info);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void TimerFunction(int value);
void Keyboard(unsigned char key, int x, int y);
void upKeyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);
void Mouseview(int x, int y);
void SetupRC(void);


int windchange = 0;//200�� �Ǹ� �ٶ��� �ٲ�
int armangle = 0;
bool armway = 0;
bool control[2][2] = { 0, };
bool ypowergauge = 0;
double player_x = 0, player_y = -BackGround_R / 4 - 80, player_z = BackGround_R * 2, player_r = 10;
int screen_move_x = player_x, screen_move_y = player_y, screen_move_z = player_z, rotate_x = 0, rotate_y = 0, rotate_z = 0;
int screen_click = 0;
int wave_y = 0, wave_flag = 0, i_see_you = 1;
double viewangle = 0;
bool left_down = 0;
int is_fire = 0;//3�̸� ���� 4�� ����
double x_acc = 0, y_acc = 20, z_acc = 10;//���� ���� ���ӵ���
double power = 0;//ó�� ���ư��� ��
double ypower = 0;
bool power_way = 0;//�Ŀ� �����̴� ����
double x_wind = 0, z_wind = 0;
double x_target = 0, z_target = 10;
int vi = -50;//y�� view
double huangle = 0;//�㸮���� ��ġ��

struct HURRICANE
{
	double xpos = 0, zpos = 0;
	int rotate = 0;
};

HURRICANE hurricane[2];
struct SNOW
{
	double xpos = (rand() % 1500) - 750;
	double zpos = (rand() % 1500) - 750;
	double ypos = (rand() % 1200) - 600;
};
SNOW snow[30];
GLfloat a = 0.2f, b = 0.5f, c = 1.0f;
GLuint textures[3];
GLubyte *pBytes;
BITMAPINFO *info;
GLUquadricObj * glu;

GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat AmbientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat DiffuseLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat SpecularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // �������
GLfloat lightPos_0[] = { 0, BackGround_R, BackGround_R, 1.0 };
GLfloat specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };

int start_flag = 0;

void main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	srand(time(NULL));
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // ���÷��� ��� ����
	glutInitWindowPosition(400, 50); // �������� ��ġ����
	glutInitWindowSize(SCREENWIDTH, SCREENHEIGHT); // �������� ũ�� ����
	glutCreateWindow("Example"); // ������ ���� (������ �̸�)
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutMouseFunc(Mouse);
	glutMotionFunc(Mouseview);
	glutKeyboardUpFunc(upKeyboard);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(specialKeyboard);
	glutTimerFunc(20, TimerFunction, 1);
	SetupRC();
	glEnable(GL_DEPTH_TEST);
	glutReshapeFunc(Reshape);

	PlaySound("2.wav", NULL, SND_ASYNC | SND_LOOP);

	glutMainLoop();
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1:
		//printf("%f %f %f\n", player_x, player_y, player_z);
		huangle++;
		if (huangle >= 360) huangle = 0;
		hurricane[0].xpos = 300 * sin(huangle / 180 * PIE);
		hurricane[0].zpos = 300 * cos(huangle / 180 * PIE);
		hurricane[1].xpos = 300 * sin((huangle - 180) / 180 * PIE);
		hurricane[1].zpos = 300 * cos((huangle - 180) / 180 * PIE);
		for (int i = 0; i < 2; i++)//�㸮����
		{
			hurricane[i].rotate += 50;
			if (hurricane[i].rotate >= 360) hurricane[i].rotate = 0;
		}
		for (int i = 0; i < 20; i++)//��
		{
			snow[i].ypos--;
			if (snow[i].ypos < -400) snow[i].ypos = 600;
			snow[i].xpos += x_wind;
			snow[i].zpos += z_wind;
		}

		windchange++;
		if (windchange == 100)//100�Ǹ� �ٶ� �ٲ�
		{
			x_wind = ((rand() % 16) * 0.1) - 0.8;
			z_wind = ((rand() % 16) * 0.1) - 0.8;
			windchange = 0;
		}

		if (is_fire > 0 && is_fire < 3)//�ٶ�
		{
			player_x += x_wind;
			player_z += z_wind;
		}
		if (is_fire == 2)//���ϻ궧 ĳ���� �̵�
		{
			if (control[0][0]) player_x--;
			if (control[0][1]) player_x++;
			if (control[1][0]) player_z++;
			if (control[1][1]) player_z--;
		}

		if (wave_flag == 0) wave_y++;   //�ĵ�
		if (wave_y > 20)   wave_flag = 1;
		if (wave_flag == 1)wave_y--;
		if (wave_y < -20)wave_flag = 0;

		if (is_fire == 0)//ó�� �� ���ϱ�
		{
			if (ypowergauge == 0)
			{
				if (power_way == 0) {
					power += 0.1;
					if (power >= 9.9) power_way = 1;
				}
				else {
					power -= 0.1;
					if (power < 0.1) power_way = 0;
				}
			}
			else
			{
				if (power_way == 0) {
					ypower += 0.1;
					if (ypower >= 9.9) power_way = 1;
				}
				else {
					ypower -= 0.1;
					if (ypower < 0.1) power_way = 0;
				}
			}
		}
		if (is_fire == 1) {
			y_acc -= GRAVITY;
			player_x += x_acc;
			player_y += y_acc;
			player_z -= z_acc;
		}
		if (is_fire == 2) {
			player_y -= 1;
		}

		if ((sqrt(((x_target - player_x) * (x_target - player_x)) + ((z_target - player_z) * (z_target - player_z))) < 50) && ((-BackGround_R / 2 - 20 < player_y - 35) && (player_y - 35 < -BackGround_R / 2 + 12))) {
			is_fire = 3;
			player_y = -BackGround_R / 2 + 17 + 11;   //17 �÷��̾�y / 10��ǥ �β�
		}
		for (int i = 0; i < 2; i++)
		{
			if (((sqrt(((hurricane[i].xpos - player_x) * (hurricane[i].xpos - player_x)) + ((hurricane[i].zpos - player_z) * (hurricane[i].zpos - player_z)))) < 30) && player_y < 50)
			{
				is_fire = 0;
				player_x = 0;
				player_y = -BackGround_R / 4 - 80;
				player_z = BackGround_R * 2;
				player_r = 10;
				ypowergauge = 0;
				ypower = 0;
				power = 0;
			}
		}
		if (player_y <= -BackGround_R / 2 - 18) {
			player_y = -BackGround_R / 2 - 18;
			is_fire = 4;
		}

		if (armway == 0) {   //��,�ٸ�
			armangle += 8;
			if (armangle > 20) armway = 1;
		}
		else {
			armangle -= 8;
			if (armangle < -20) armway = 0;
		}

		glutTimerFunc(25, TimerFunction, 1); // Ÿ�̸��Լ� �� ����
		break;
	}
	glutPostRedisplay(); // ȭ�� �� ���
}

GLubyte *LoadIBitmap(const char *filename, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;

	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		fclose(fp);
		return NULL;
	}

	if (header.bfType != 'MB') {
		fclose(fp);
		return NULL;
	}

	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	// ��Ʈ�� �̹��� �����͸� ���� �޸� �Ҵ��� �Ѵ�.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
		fclose(fp);
		exit(0);
		return NULL;
	}

	// ��Ʈ�� ���� ����� �д´�.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize) {
		free(*info);
		fclose(fp);
		return NULL;
	}

	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth*(*info)->bmiHeader.biBitCount + 7) / 8.0 * abs((*info)->bmiHeader.biHeight);
	// ��Ʈ���� ũ�⸸ŭ �޸𸮸� �Ҵ��Ѵ�.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp);
		return NULL;
	}
	// ��Ʈ�� �����͸� bit(GLubyte Ÿ��)�� �����Ѵ�.
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize) {
		free(*info); free(bits);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return bits;
}

void upKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'X':
	case 'x':
		if (is_fire == 1) is_fire = 2;
		break;
	case 'w':
	case 'W':
		control[1][1] = 0;
		break;
	case 's':
	case 'S':
		control[1][0] = 0;
		break;
	case 'a':
	case 'A':
		control[0][0] = 0;
		break;
	case 'd':
	case 'D':
		control[0][1] = 0;
		break;
	}
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 32:   //space��
		if (i_see_you == 0)i_see_you = 1;
		else if (i_see_you == 1)i_see_you = 0;
		break;
	case 'X':
	case 'x':
		if (is_fire == 1) is_fire = 2;
		break;
	case 'Z':
	case 'z':
		if (is_fire == 0)
		{
			if (ypowergauge == 0)
			{
				ypowergauge = 1;
				z_acc = power;
			}
			else if (ypowergauge == 1)
			{
				y_acc = ypower * 2.3;
				x_acc = power * viewangle * 0.02;
				is_fire = 1;
			}
		}

		break;
	case 'Y':
	case 'y':
		rotate_y++;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'w':
	case 'W':
		control[1][1] = 1;
		break;
	case 's':
	case 'S':
		control[1][0] = 1;
		break;
	case 'a':
	case 'A':
		control[0][0] = 1;
		break;
	case 'd':
	case 'D':
		control[0][1] = 1;
		break;
	case 'i':
	case 'I':
		is_fire = 0;
		player_x = 0;
		player_y = -BackGround_R / 4 - 80;
		player_z = BackGround_R * 2;
		player_r = 10;
		ypowergauge = 0;
		ypower = 0;
		power = 0;
	}
	glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		z_target -= 5;
		break;
	case GLUT_KEY_DOWN:
		z_target += 5;
		break;
	case GLUT_KEY_RIGHT:
		x_target += 5;
		break;
	case GLUT_KEY_LEFT:
		x_target -= 5;
		break;
	}
	glutPostRedisplay();
}


void Mouseview(int x, int y)
{
	if (left_down == 1)
	{
		if (is_fire == 0) {
			if ((x - (SCREENWIDTH / 2)) / 5 < -22)
				viewangle = -22;
			else if ((x - SCREENWIDTH / 2) / 5 > 22)
				viewangle = 22;
			else
				viewangle = (x - SCREENWIDTH / 2) / 5;
		}
		else {
			viewangle = (x - SCREENWIDTH / 2) / 5;
		}
		if (y - SCREENHEIGHT / 2 > 100) vi = 100;
		else if (y - SCREENHEIGHT / 2 < -50) vi = -50;
		else vi = y - SCREENHEIGHT / 2;
	}

}

void Mouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		left_down = 1;
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		left_down = 0;
}


GLvoid drawScene(GLvoid)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // ������ ����

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(NULL));

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.0);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos_0);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 64);

	glEnable(GL_DEPTH_TEST);   //��������
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);      //����
	glEnable(GL_LIGHT0);
	glEnable(GL_BLEND);
	//����

	if (start_flag == 0) {
		glGenTextures(2, textures);

		glBindTexture(GL_TEXTURE_2D, textures[0]);

		pBytes = LoadIBitmap("1.bmp", &info);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, 400, 400, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, textures[1]);

		pBytes = LoadIBitmap("2.bmp", &info);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, 400, 400, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		start_flag = 1;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 800 / 600, 1.0, 3000.0);

	glMatrixMode(GL_MODELVIEW);

	glLineWidth(3.0);

	GLfloat ctrlpoints[3][3][3] = {
		{ { -100,-wave_y,-100 },{ 0,wave_y,-100 },{ 100,-wave_y,-100 } },
		{ { -100,wave_y,0 },{ 0,-wave_y,0 },{ 100,wave_y,0 } },
		{ { -100,-wave_y,100 },{ 0,wave_y,100 },{ 100,-wave_y,100 } } };

	glPushMatrix();

	if (i_see_you == 0) {
		glTranslatef(0, 0, -1500);   //-1500����
		gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1, 0, 1, 0);//ī�޶���ġ.
		glRotatef(10, 1, 0, 0);
	}
	if (i_see_you == 1) {
		gluLookAt(player_x + (200 * cos((viewangle + 90) / 180 * PIE)), player_y + vi, player_z + (200 * sin((viewangle + 90) / 180 * PIE)), player_x, player_y, player_z, 0, 1, 0);//ī�޶���ġ.
	}

	for (int i = 0; i < 20; i++)//��
	{
		glPushMatrix();
		glColor3f(1, 1, 1);
		glTranslated(snow[i].xpos, snow[i].ypos, snow[i].zpos);
		glutSolidSphere(10, 20, 20);
		glPopMatrix();
	}
	for (int i = 0; i < 2; i++)//�㸮����
	{
		glPushMatrix();
		glTranslated(hurricane[i].xpos, 0, hurricane[i].zpos);
		glRotated(90, 1, 0, 0);
		glRotated(hurricane[i].rotate, 0, 0, 1);
		glScaled(1, 0.5, 1);
		glutSolidCone(30, 400, 30, 30);
		glPopMatrix();
	}

	glRotatef(rotate_x, 1, 0, 0);
	glRotatef(rotate_y, 0, 1, 0);
	glRotatef(rotate_z, 0, 0, 1);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glBegin(GL_QUADS);   //���
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-BackGround_R, BackGround_R, -BackGround_R);   //��
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-BackGround_R, -BackGround_R, -BackGround_R);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(BackGround_R, -BackGround_R, -BackGround_R);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(BackGround_R, BackGround_R, -BackGround_R);

	if (player_z < BackGround_R * 2 - 200) {
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-BackGround_R, BackGround_R, BackGround_R * 2);   //��
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-BackGround_R, -BackGround_R, BackGround_R * 2);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(BackGround_R, -BackGround_R, BackGround_R * 2);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(BackGround_R, BackGround_R, BackGround_R * 2);
	}

	glTexCoord2f(1.0f, 1.0f); glVertex3f(-BackGround_R, BackGround_R, BackGround_R * 2);   //��
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-BackGround_R, BackGround_R, -BackGround_R * 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-BackGround_R, -BackGround_R, -BackGround_R * 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-BackGround_R, -BackGround_R, BackGround_R * 2);

	glTexCoord2f(1.0f, 1.0f); glVertex3f(BackGround_R, BackGround_R, BackGround_R * 2);   //��
	glTexCoord2f(0.0f, 1.0f); glVertex3f(BackGround_R, BackGround_R, -BackGround_R * 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(BackGround_R, -BackGround_R, -BackGround_R * 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(BackGround_R, -BackGround_R, BackGround_R * 2);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glBegin(GL_QUADS);   //���

	glTexCoord2f(1.0f, 1.0f); glVertex3f(BackGround_R, BackGround_R, BackGround_R * 2);   //��
	glTexCoord2f(0.0f, 1.0f); glVertex3f(BackGround_R, BackGround_R, -BackGround_R * 2);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-BackGround_R, BackGround_R, -BackGround_R * 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-BackGround_R, BackGround_R, BackGround_R * 2);

	glVertex3f(BackGround_R, -BackGround_R, BackGround_R * 2);   //��
	glVertex3f(BackGround_R, -BackGround_R, -BackGround_R * 2);
	glVertex3f(-BackGround_R, -BackGround_R, -BackGround_R * 2);
	glVertex3f(-BackGround_R, -BackGround_R, BackGround_R * 2);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();   // �ĵ�
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3, 0.0, 1.0, 9, 3, &ctrlpoints[0][0][0]);
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(10, 0, 1, 10, 0, 1);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor4f(0, 0, 1, 1);
	for (int i = 0; i < BackGround_R / 100; i++) {
		for (int j = 0; j < BackGround_R / 100 + (BackGround_R / 200); j++) {
			glPushMatrix();
			glTranslatef(i * 200 - (BackGround_R)+100, -BackGround_R / 2 - 20, j * 200 - (BackGround_R));
			glEvalMesh2(GL_FILL, 0, 10, 0, 10);
			glPopMatrix();
		}
	}
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();   //ĳ����

	glTranslatef(player_x, player_y, player_z);

	glPushMatrix();   // �Ӹ�
	glColor3f(1, 1, 0);
	glTranslatef(0, player_r + 1, 0);
	glutSolidCube(player_r - 3);
	glPopMatrix();

	glPushMatrix();   // ��
	glColor3f(1, 0, 0);
	glScalef(1, 1.5, 1);
	glutSolidCube(player_r);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 2, 0);
	for (int i = -1; i < 2; i++)
	{
		glPushMatrix();
		if (i != 0)   //��
		{
			glColor3f(0, 0.3, 0);
			glRotated(armangle * i, 0, 0, 1);
			glTranslated(i * 8, 0, 0);

			glScaled(2, 0.8, 0.8);
			glutSolidCube(player_r / 2);
		}
		glPopMatrix();
	}
	glPopMatrix();

	for (int i = -1; i < 2; i++) {
		if (i != 0)   //�ٸ�
		{
			glPushMatrix();
			glTranslated(0, -10, 0);
			glColor3f(0, 0.3, 0);
			glRotated(armangle * i, 1, 0, 0);
			glTranslated(i * 2.3, 0, 0);

			glScaled(0.8, 3, 0.8);
			glutSolidCube(player_r / 2);
			glPopMatrix();
		}
	}

	glColor3f(1, 1, 0);
	if (is_fire == 2)//���ϻ�
	{
		glBegin(GL_LINE_STRIP);
		glVertex3d(0, 0, 0);
		glVertex3d(50, 150, 50);
		glVertex3d(-50, 150, 50);
		glVertex3d(0, 0, 0);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex3d(0, 0, 0);
		glVertex3d(50, 150, -50);
		glVertex3d(-50, 150, -50);
		glVertex3d(0, 0, 0);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex3d(0, 0, 0);
		glVertex3d(50, 150, 0);
		glVertex3d(-50, 150, 0);
		glVertex3d(0, 0, 0);
		glEnd();
		glBegin(GL_POLYGON);
		glVertex3d(-50, 150, -50);
		glVertex3d(50, 150, -50);
		glVertex3d(50, 150, 50);
		glVertex3d(-50, 150, 50);
		glEnd();
	}
	glPopMatrix();

	glPushMatrix();   // ĳ����+��
	glColor3f(1, 1, 1);
	if (player_z < BackGround_R * 2 - 100) {
		glBegin(GL_LINE_STRIP);
		glVertex3f(-40, -BackGround_R / 2 + 90, BackGround_R * 2 - 100);
		glVertex3f(40, -BackGround_R / 2 + 90, BackGround_R * 2 - 100);
		glEnd();
	}
	else {
		glBegin(GL_LINE_STRIP);
		glVertex3f(-40, -BackGround_R / 2 + 90, BackGround_R * 2 - 100);
		glVertex3f(player_x - player_r / 2, player_y, player_z + player_r / 2 + 1);
		glVertex3f(player_x + player_r / 2, player_y, player_z + player_r / 2 + 1);
		glVertex3f(40, -BackGround_R / 2 + 90, BackGround_R * 2 - 100);
		glEnd();
	}
	glPopMatrix();

	glPushMatrix();   //��߽� ��
	glColor3f(1, 1, 1);
	glTranslatef(-40, -BackGround_R / 2 + 90, BackGround_R * 2 - 100);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 360; i++)
		glVertex3f(11 * cos(i * 180 / PIE), 0, 11 * sin(i * 180 / PIE));
	glEnd();

	glPushMatrix();
	glRotatef(16, 0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 360; i++)
		glVertex3f(11 * cos(i * 180 / PIE), 0, 11 * sin(i * 180 / PIE));
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-16, 0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 360; i++)
		glVertex3f(11 * cos(i * 180 / PIE), 0, 11 * sin(i * 180 / PIE));
	glEnd();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();   //��߽� ��
	glColor3f(1, 1, 1);
	glTranslatef(40, -BackGround_R / 2 + 90, BackGround_R * 2 - 100);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 360; i++)
		glVertex3f(11 * cos(i * 180 / PIE), 0, 11 * sin(i * 180 / PIE));
	glEnd();

	glPushMatrix();
	glRotatef(16, 0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 360; i++)
		glVertex3f(11 * cos(i * 180 / PIE), 0, 11 * sin(i * 180 / PIE));
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-16, 0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 360; i++)
		glVertex3f(11 * cos(i * 180 / PIE), 0, 11 * sin(i * 180 / PIE));
	glEnd();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();   // �������
	glTranslatef(0, -BackGround_R, BackGround_R * 2);
	glTranslatef(0, BackGround_R / 4, -100);

	glPushMatrix();
	glu = gluNewQuadric();
	glColor3f(0.0f, 0.8f, 0.2f);
	glTranslatef(0, BackGround_R / 4, 0);
	gluQuadricDrawStyle(glu, GLU_FILL);
	gluQuadricNormals(glu, GLU_SMOOTH);
	gluQuadricOrientation(glu, GLU_OUTSIDE);

	glPushMatrix();
	glTranslatef(40, 50, 0);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(glu, 10, 10, 50, 20, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-40, 50, 0);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(glu, 10, 10, 50, 20, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(50, 50, 0);
	glRotatef(-90, 0, 1, 0);
	gluCylinder(glu, 10, 10, 100, 20, 8);
	glPopMatrix();

	glRotatef(-90, 1, 0, 0);
	gluCylinder(glu, 10, 10, 50, 20, 8);
	glPopMatrix();

	glColor3f(0.6f, 0.0f, 0.2f);
	glScalef(BackGround_R * 2, BackGround_R / 2, 200);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();   // ��ǥ
	glTranslatef(x_target, -BackGround_R / 2, z_target);
	glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++)
		glVertex3f(50 * cos(i * 180 / PIE), 10, 50 * sin(i * 180 / PIE));
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++)
		glVertex3f(40 * cos(i * 180 / PIE), 10.1, 40 * sin(i * 180 / PIE));
	glEnd();

	glColor3f(0, 0, 1);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++)
		glVertex3f(30 * cos(i * 180 / PIE), 10.2, 30 * sin(i * 180 / PIE));
	glEnd();

	glColor3f(1, 0, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++)
		glVertex3f(20 * cos(i * 180 / PIE), 10.3, 20 * sin(i * 180 / PIE));
	glEnd();

	glColor3f(1, 1, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++)
		glVertex3f(10 * cos(i * 180 / PIE), 10.4, 10 * sin(i * 180 / PIE));
	glEnd();
	glRotatef(-90, 1, 0, 0);
	glColor3f(1, 1, 1);
	gluCylinder(glu, 50, 50, 10, 20, 8);
	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glPushMatrix();   // ���۽� �����Ÿ�
	glColor4f(1, 0, 0, 0.7);
	glTranslatef(10, -BackGround_R / 4 - 80, BackGround_R * 2);
	glBegin(GL_QUADS);
	glVertex3f(0, 10, -10);
	glVertex3f(0, -10, -10);
	glVertex3f(20, -30, -50);
	glVertex3f(20, 30, -50);

	glVertex3f(-20, 10, -10);
	glVertex3f(-20, -10, -10);
	glVertex3f(-40, -30, -50);
	glVertex3f(-40, 30, -50);

	glVertex3f(-40, 30, -50);
	glVertex3f(20, 30, -50);
	glVertex3f(00, 10, -10);
	glVertex3f(-20, 10, -10);

	glVertex3f(-40, -30, -50);
	glVertex3f(20, -30, -50);
	glVertex3f(00, -10, -10);
	glVertex3f(-20, -10, -10);
	glEnd();
	glPopMatrix();

	gluDeleteQuadric(glu);
	glPopMatrix();
	if (is_fire == 0)
	{
		glDisable(GL_BLEND);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, 800, 0, 600, 0, 100);
		glColor4f(1, 0, 0, 1);
		glBegin(GL_LINE_STRIP);
		glVertex2f(100, 20);
		glVertex2f(100 + GAUGEWIDTH, 20);
		glVertex2f(100 + GAUGEWIDTH, 20 + GAUGEHEIGHT);
		glVertex2f(100, 20 + GAUGEHEIGHT);
		glVertex2f(100, 20);
		glEnd();
		glColor3f(0.3, 0.4, 0.6);
		glBegin(GL_POLYGON);
		glVertex2f(100, 20);
		glVertex2f(100 + (GAUGEWIDTH * power / 10.0), 20);
		glVertex2f(100 + (GAUGEWIDTH * power / 10.0), 20 + GAUGEHEIGHT);
		glVertex2f(100, 20 + GAUGEHEIGHT);
		glVertex2f(100, 20);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex2f(20, 100);
		glVertex2f(20, 100 + GAUGEWIDTH - 200);
		glVertex2f(20 + GAUGEHEIGHT, 100 + GAUGEWIDTH - 200);
		glVertex2f(20 + GAUGEHEIGHT, 100);
		glVertex2f(20, 100);
		glEnd();
		glBegin(GL_POLYGON);
		glVertex2f(20, 100);
		glVertex2f(20, 100 + ((GAUGEWIDTH - 200) * ypower / 10.0));
		glVertex2f(20 + GAUGEHEIGHT, 100 + ((GAUGEWIDTH - 200) * ypower / 10.0));
		glVertex2f(20 + GAUGEHEIGHT, 100);
		glEnd();
	}
	glRasterPos3f(400, 400, 0);
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, w / h, 1.0, 3000.0);
}

void SetupRC()
{
	hurricane[0].xpos = -300;
	hurricane[1].xpos = 300;
}