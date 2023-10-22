/*********
CTIS164 - Template Source Program
----------
STUDENT : ANASTASYA DUYGU KILIC
SECTION : 001
HOMEWORK: HOMEWORK3
----------
PROBLEMS: -If you drag out the bunny with the carrot(mouse) the bunny glitches and goes out of the border however this problem is solved when you move
          your mouse back in the range of the border.
----------
ADDITIONAL FEATURES: -When reflected off a border the bunny changes color in between 5 different colors that I have defined in an array.
                     -When the bunny is NOT moving "Press space bar to RESUME" message is displayed but when the bunny is moving "Press
                     spacebar to PAUSE" message is displayed.
					 -There is an additional "SURVIVAL" mode where the bunny loses hearts when it bumps into a border, and the default
					 amount of hearts is set to 5.
					 -In SURVIVAL mode you can use your up and down arrow keys to adjust how many hearts you want to have. It can only be in between
					 0-10 hearts.
					 -The player is only allowed to adjust their heart numbers while they are in SURVIVAL mode.
					 -Your hearts will be displayed at the bottom left corner of the window, as well as your heart counter.
					 -When you reach zero hearts you will automatically be out of SURVIVAL mode and if you would like to re-enter SURVIVAL mode
					 you will first have to press down arrow key to reset your hearts and then press F1 to re-enter SURVIVAL.
					 -Also when you reach zero hearts a message that explains how to re-enter SURVIVAL mode will pop-up to aid the player.
					 -The coordinates of the bunny will be displayed at the right bottom corner of the screen at all times.
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MOVE 0
#define PAUSE 1

#define BUNNYSPEED 2

typedef struct {
	vec_t pos;
	vec_t vel;
	float angle;
}bunny_t;

typedef struct {
	vec_t pos;
}carrot_t;

typedef struct {
	vec_t pos;
}mouse_t;


#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false, reflectright = false, reflectleft = false, reflecttop = false, reflectbot = false, F1 = false, reflect = false;
int  winWidth, winHeight; // current Window width and height
bool inRangeX = false;
bool inRangeY = false;
bunny_t bunny = { { -100,-100 },{ 0,0 },{ 90 } };
carrot_t carrot;
mouse_t mouse;
polar_t p;
polar_t store;
int mode = PAUSE;
int bunnyColors[5][3] = { 255, 255, 255, //bunny colors
255, 224, 199,
236, 240, 210,
172, 246, 251,
199, 255, 237
}, colorIndex = 0;

int magnitude;
float heartCounter = 5;
bool first = false;


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

//
// To display onto window using OpenGL commands
//

void changeColor() {
	switch (colorIndex) {
	case 0: colorIndex = 1; break;
	case 1: colorIndex = 2; break;
	case 2: colorIndex = 3; break;
	case 3: colorIndex = 4; break;
	case 4: colorIndex = 0; break;
	}
}

void vertex(vec_t P, vec_t Tr, double angle) {
	double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
	double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
	glVertex2d(xp, yp);
}

void drawHeart() {
	for (int i = 0; i < heartCounter; i++) {
		glColor3ub(247, 109, 109);
		circle(90+i*30, -100-100, 10);
		circle(110 + i * 30, -100-100, 10);
		glBegin(GL_TRIANGLES);
		glVertex2d(121 + i * 30, -100 - 2-100);
		glVertex2d(79 + i * 30, -100 - 2-100);
		glVertex2d(100+i * 30, -100 - 18-100);
		glEnd();
		glColor3ub(253, 142, 142);
		circle(108 + i * 30, -100-100, 7);
		circle(92 + i * 30, -100-100, 7);
		glBegin(GL_TRIANGLES);
		glVertex2d( 116 + i * 30, -100-100);
		glVertex2d(84 + i * 30, -100-100);
		glVertex2d( 100+i * 30, -100 - 16-100);
		glEnd();
	}
}

void drawBunny() {
	double angle = bunny.angle * D2R;
	//ears
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	vertex({ 16,15 }, bunny.pos, angle);
	vertex({ 35,15 }, bunny.pos, angle);
	vertex({ 35,70 }, bunny.pos, angle);
	vertex({ 16,70 }, bunny.pos, angle);
	glEnd();
	glBegin(GL_POLYGON);
	vertex({ -16,15 }, bunny.pos, angle);
	vertex({ -35,15 }, bunny.pos, angle);
	vertex({ -35,70 }, bunny.pos, angle);
	vertex({ -16,70 }, bunny.pos, angle);
	glEnd();
	glColor3ub(bunnyColors[colorIndex][0], bunnyColors[colorIndex][1], bunnyColors[colorIndex][2]);
	glBegin(GL_POLYGON);
	vertex({ 18,15 }, bunny.pos, angle);
	vertex({ 33,15 }, bunny.pos, angle);
	vertex({ 33,68 }, bunny.pos, angle);
	vertex({ 18,68 }, bunny.pos, angle);
	glEnd();
	glBegin(GL_POLYGON);
	vertex({ -18,15 }, bunny.pos, angle);
	vertex({ -33,15 }, bunny.pos, angle);
	vertex({ -33,68 }, bunny.pos, angle);
	vertex({ -18,68 }, bunny.pos, angle);
	glEnd();
	glColor3ub(251, 186, 226);
	glBegin(GL_POLYGON);
	vertex({ 20,15 }, bunny.pos, angle);
	vertex({ 31,15 }, bunny.pos, angle);
	vertex({ 31,66 }, bunny.pos, angle);
	vertex({ 20,66 }, bunny.pos, angle);
	glEnd();
	glBegin(GL_POLYGON);
	vertex({ -20,15 }, bunny.pos, angle);
	vertex({ -31,15 }, bunny.pos, angle);
	vertex({ -31,66 }, bunny.pos, angle);
	vertex({ -20,66 }, bunny.pos, angle);
	glEnd();
	////head
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	vertex({ -35,17 }, bunny.pos, angle);
	vertex({ 35,17 }, bunny.pos, angle);
	vertex({ 35,-30 }, bunny.pos, angle);
	vertex({ -35,-30 }, bunny.pos, angle);
	glEnd();
	glColor3ub(bunnyColors[colorIndex][0], bunnyColors[colorIndex][1], bunnyColors[colorIndex][2]);
	glBegin(GL_POLYGON);
	vertex({ -33,15 }, bunny.pos, angle);
	vertex({ 33,15 }, bunny.pos, angle);
	vertex({ 33,-28 }, bunny.pos, angle);
	vertex({ -33,-28 }, bunny.pos, angle);
	glEnd();
	////eyes
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	vertex({ -28,-5 }, bunny.pos, angle);
	vertex({ -18,-5}, bunny.pos, angle);
	vertex({ -18,-15 }, bunny.pos, angle);
	vertex({ -28,-15 }, bunny.pos, angle);
	glEnd();
	glBegin(GL_POLYGON);
	vertex({ 28,-5 }, bunny.pos, angle);
	vertex({ 18,-5 }, bunny.pos, angle);
	vertex({ 18,-15 }, bunny.pos, angle);
	vertex({ 28,-15 }, bunny.pos, angle);
	glEnd();
	////blush
	glColor3ub(250, 195, 215);
	glBegin(GL_POLYGON);
	vertex({ 29,-16 }, bunny.pos, angle);
	vertex({ 18,-16 }, bunny.pos, angle);
	vertex({ 18,-24 }, bunny.pos, angle);
	vertex({ 29,-24 }, bunny.pos, angle);
	glEnd();
	glBegin(GL_POLYGON);
	vertex({ -29,-16 }, bunny.pos, angle);
	vertex({ -18,-16 }, bunny.pos, angle);
	vertex({ -18,-24 }, bunny.pos, angle);
	vertex({ -29,-24 }, bunny.pos, angle);
	glEnd();

}

void drawCarrot() {
	glColor3ub(255, 128, 0);
	for (int i = 0; i < 7; i++)
		circle(carrot.pos.x, carrot.pos.y - i * 4, 12 - i);
	glColor3ub(255, 153, 51);
	for (int i = 0; i < 7; i++)
		circle(carrot.pos.x, carrot.pos.y - i * 4, 10 - i);
	glColor3ub(102, 204, 0);
	for (int i = 0; i < 7; i++)
	{
		circle(carrot.pos.x + i / 4, carrot.pos.y + i * 2 + 10, 4 - i / 4);
		circle(carrot.pos.x + 4 + i / 2, carrot.pos.y + i * 2 + 10, 4 - i / 4);
		circle(carrot.pos.x - 3 - i / 2, carrot.pos.y + i * 2 + 10, 4 - i / 4);
	}
}

void displayBorder() {
	glColor3f(1, 1, 1);
	vprint(-winWidth / 2 + 40, winHeight / 2 - 50, GLUT_BITMAP_9_BY_15, "Anastasya Duygu KILIC");
	vprint(-winWidth / 2 + 320, winHeight / 2 - 50, GLUT_BITMAP_TIMES_ROMAN_24, "CARROT CHASE");
	vprint(winWidth / 2 - 130, -winHeight / 2 + 30, GLUT_BITMAP_9_BY_15, "Angle = %d", (int)bunny.angle);
	if (mode == PAUSE)
		vprint(winWidth / 2 - 490, -winHeight / 2 + 30, GLUT_BITMAP_9_BY_15, "Press spacebar to RESUME");
	else vprint(winWidth / 2 - 490, -winHeight / 2 + 30, GLUT_BITMAP_9_BY_15, "Press spacebar to PAUSE");
	if (inRangeX && inRangeY)
		vprint(-winWidth / 2 + 30, -winHeight / 2 + 30, GLUT_BITMAP_9_BY_15, "mouse is inside");
	else vprint(-winWidth / 2 + 30, -winHeight / 2 + 30, GLUT_BITMAP_9_BY_15, "mouse is outside");

	glRectf(-winWidth / 2 + 38, -winHeight / 2 + 68, winWidth / 2 - 38, winHeight / 2 - 98);
	glColor3f(196 / 255., 161 / 255., 243 / 255.);
	glRectf(-winWidth / 2 + 40, -winHeight / 2 + 70, winWidth / 2 - 40, winHeight / 2 - 100);
}

void display() {
	//
	// clear window to black
	//
	glClearColor(145 / 255., 145 / 255., 145 / 255., 0);
	glClear(GL_COLOR_BUFFER_BIT);

	displayBorder();

	drawBunny();
	if (F1)
		drawHeart();
	
	if (inRangeX && inRangeY)
		drawCarrot();
	glColor3f(1, 0, 0);
	//vprint(0, 0, GLUT_BITMAP_9_BY_15, "%d", colorIndex);
	glColor3f(1, 1, 1);
	vprint(-winWidth / 2 + 50, -winHeight / 2 + 105, GLUT_BITMAP_9_BY_15, "bunny pos x: %d", (int)bunny.pos.x);
	vprint(-winWidth / 2 + 50, -winHeight / 2 + 85, GLUT_BITMAP_9_BY_15, "bunny pos y: %d", (int)bunny.pos.y);
	if (!F1) {
		vprint(winWidth / 2 - 545, winHeight / 2 - 85, GLUT_BITMAP_9_BY_15, "press <F1> to enter SURVIVAL mode");
		glColor3ub(255, 153, 51);
		vprint(winWidth / 2 - 545, winHeight / 2 - 85, GLUT_BITMAP_9_BY_15, "      <F1>");
	}
	else {
		glColor3f(1, 1, 1);
		vprint(winWidth / 2 - 565, winHeight / 2 - 85, GLUT_BITMAP_9_BY_15, "press <F1> again to exit SURVIVAL mode");
		vprint(winWidth / 2 - 695, winHeight / 2 - 125, GLUT_BITMAP_9_BY_15, "press <up> and <down> arrow keys to increase/decrease heart counter");
		glColor3ub(255, 153, 51);
		vprint(winWidth / 2 - 565, winHeight / 2 - 85, GLUT_BITMAP_9_BY_15, "      <F1>");
		vprint(winWidth / 2 - 695, winHeight / 2 - 125, GLUT_BITMAP_9_BY_15, "      <up>     <down>");
		glColor3f(1, 1, 1);
		vprint(winWidth / 2 - 320, -winHeight / 2 + 120, GLUT_BITMAP_9_BY_15, "heart count: %d", (int)heartCounter);
		if ((int)heartCounter == 0)
			F1 = false;
	}
	if (heartCounter == 0) {
		glColor3f(1, 1, 1);
		vprint(winWidth / 2 - 685, winHeight / 2 - 145, GLUT_BITMAP_9_BY_15, "(if your heart counter drops to zero press <down> arrow key");
		vprint(winWidth / 2 - 590, winHeight / 2 - 165, GLUT_BITMAP_9_BY_15, "and then <F1> to re-enter SURVIVAL mode)");
		glColor3ub(255, 153, 51);
		vprint(winWidth / 2 - 685, winHeight / 2 - 145, GLUT_BITMAP_9_BY_15, "                                           <down>");
		vprint(winWidth / 2 - 590, winHeight / 2 - 165, GLUT_BITMAP_9_BY_15, "         <F1>");
	}

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	if (key == ' ')
	{
		if (spacebar == false)
			spacebar = true;
		else spacebar = false;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true;
		if (heartCounter <= 9 && F1)
			heartCounter++;
		break;
	case GLUT_KEY_DOWN: down = true; 
		if (heartCounter > 0 && F1)
			heartCounter--;
		if (heartCounter == 0 && !F1)
			heartCounter = 5;
	
		break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	case GLUT_KEY_F1: 
		if (F1 == true)
			F1 = false;
		else F1 = true;
		break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.
	mouse.pos.x = x - winWidth / 2;
	mouse.pos.y = winHeight / 2 - y;

	if (inRangeX)
		carrot.pos.x = mouse.pos.x;
	if (inRangeY)
		carrot.pos.y = mouse.pos.y;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	//pause/resume
	if (spacebar == false)
		mode = PAUSE;
	else mode = MOVE;

	if (mouse.pos.x < winWidth / 2 - 40 && mouse.pos.x > -winWidth / 2 + 40)
		inRangeX = true;
	else inRangeX = false;
	if (mouse.pos.y < winHeight / 2 - 100 && mouse.pos.y > -winHeight / 2 + 70)
		inRangeY = true;
	else inRangeY = false;

	store = rec2pol(bunny.vel);
	bunny.angle = store.angle;
	if (bunny.angle < 0)
		bunny.angle += 360;

	//survival mode
	if (F1 && reflect)
		heartCounter--;

	if (mode == PAUSE)
	{
		bunny.angle = atan2(carrot.pos.y - bunny.pos.y, carrot.pos.x - bunny.pos.x) / D2R;
		if (bunny.angle < 0)
			bunny.angle += 360;
	}


	if (mode == MOVE) {
		if (inRangeX && inRangeY)
		{
			bunny.vel = mulV(BUNNYSPEED, unitV(subV(carrot.pos, bunny.pos)));
			first = true;
			vec_t prevPos = bunny.pos;
			//chase
			bunny.pos = addV(bunny.pos, bunny.vel);
		}
		if (first)
		{
			bunny.vel = mulV(BUNNYSPEED, bunny.vel);
			first = false;
		}
		bunny.pos = addV(bunny.pos, bunny.vel);

		//reflection
		if (bunny.pos.x >= winWidth / 2 - 65 || bunny.pos.x <= -winWidth / 2 + 65)
			bunny.vel.x *= -1;
		if (bunny.pos.y >= winHeight / 2 - 140 || bunny.pos.y <= -winHeight / 2 + 95)
			bunny.vel.y *= -1;

		//change color after reflection
		if ((int)bunny.pos.x >= winWidth / 2 - 65 || (int)bunny.pos.x <= -winWidth / 2 + 65 || (int)bunny.pos.y >= winHeight / 2 - 140 || (int)bunny.pos.y <= -winHeight / 2 + 95)
			reflect = true;
		else reflect = false;

		if (reflect)
			changeColor();

		
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Carrot Chase HW3");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}