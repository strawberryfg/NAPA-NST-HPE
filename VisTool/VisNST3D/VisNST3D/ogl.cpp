#pragma comment (lib, "libpng16.lib")	
#pragma comment (lib, "zlib.lib")
#include <iostream>
#include <algorithm>
#include "basic.h"
#include "png.h"
#include "gl_inc.h"
//#include "glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <opencv2/opencv.hpp> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "HumanModel_define.h"

using namespace cv;
extern char directory[maxlen]; //directory\\gt_2d_modified_

extern int win_w, win_h;
extern float win_fovy;

extern glm::mat4 transform_camera;
extern glm::mat4 transform_model;
extern glm::vec4 position_light0;
extern float mouse_speed;

extern void save_memory();


extern void LoadImageInfo(int id);


extern double spherex[2][JointNum], spherey[2][JointNum], spherez[2][JointNum];
extern int current_sphere;

void draw_frame(float len)
{
	GLfloat color[] = { .0f, .0f, .0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, color);

	glMatrixMode(GL_MODELVIEW);
	// O
	color[0] = 1.0f; color[1] = 0.5f; color[2] = .0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glPushMatrix();
	glutSolidSphere(len / 40, 50, 50);
	glPopMatrix();
	// X
	color[0] = 1.0f; color[1] = .0f; color[2] = .0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glPushMatrix();
	glTranslatef(len / 2, 0, 0);
	glScalef(1.0f, 0.01f, 0.01f);
	glutSolidCube(len);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(len, 0, 0);
	glRotatef(90, 0, 1, 0);
	glutSolidCone(len / 30, len / 8, 50, 50);
	glPopMatrix();
	// Y
	color[0] = .0f; color[1] = 1.0f; color[2] = .0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glPushMatrix();
	glTranslatef(0, len / 2, 0);
	glScalef(0.01f, 1.0f, 0.01f);
	glutSolidCube(len);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, len, 0);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(len / 30, len / 8, 50, 50);
	glPopMatrix();
	// Z
	color[0] = .0f; color[1] = .0f; color[2] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glPushMatrix();
	glTranslatef(0, 0, len / 2);
	glScalef(0.01f, 0.01f, 1.0f);
	glutSolidCube(len);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0, len);
	glutSolidCone(len / 30, len / 8, 50, 50);
	glPopMatrix();
	// unit
	color[0] = 1.0f; color[1] = 0.0f; color[2] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glPushMatrix();
	glTranslatef(1, 0, 0);
	glutSolidCube(len / 50);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 1, 0);
	glutSolidCube(len / 50);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0, 1);
	glutSolidCube(len / 50);
	glPopMatrix();
}


void draw_world(float len, float tex_repeat, bool yoz_plane, bool zox_plane, bool xoy_plane)
{
	//draw_frame(len / 3);
}

void ogl_init()
{
	// projection matrix init
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(win_fovy, float(win_w) / win_h, 1.0, 1.0e10);
	// model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// color and lighting
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glColor4f(.5f, .5f, .5f, 1.0f);
	glShadeModel(GL_SMOOTH);

	GLfloat vec4f[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, vec4f);
	glLightfv(GL_LIGHT0, GL_SPECULAR, vec4f);
	vec4f[0] = .2f; vec4f[1] = .2f; vec4f[2] = .2f;
	glLightfv(GL_LIGHT0, GL_AMBIENT, vec4f);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glDisable(GL_CULL_FACE);
	vec4f[0] = .0f; vec4f[1] = .0f; vec4f[2] = .0f;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vec4f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_NORMALIZE);
}

void reshape_func(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(win_fovy, float(w) / h, 0.5, 1.0e10);
	win_w = w; win_h = h;
}

static bool l_button_down = false, r_button_down = false, mid_button_down = false;
static int last_x = -1, last_y = -1;
#define  GLUT_WHEEL_UP		3 // ¹öÂÖ²Ù×÷  
#define  GLUT_WHEEL_DOWN	4

void mouse_click_func(int button, int state, int x, int y)
{
	y = win_h - y;
	switch (button){
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			r_button_down = true;
			last_x = x; last_y = y;
			glutSetCursor(GLUT_CURSOR_CYCLE);

		}
		else {
			r_button_down = false;
			last_x = -1; last_y = -1;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		break;
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			l_button_down = true;
			last_x = x; last_y = y;
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		}
		else {
			l_button_down = false;
			last_x = -1; last_y = -1;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		break;
	case GLUT_WHEEL_UP:
		if (state == GLUT_UP){
			char mousespeedname[maxlen];
			sprintf(mousespeedname, "%s%s", directory, "\\configuration\\mouse_speed.in");
			FILE *fin_mousespeed;
			fin_mousespeed = fopen(mousespeedname, "r");
			float mouse_speed;
			mouse_speed = 0.5;
			if (fin_mousespeed != NULL)
			{
				fscanf(fin_mousespeed, "%f", &mouse_speed);
				fclose(fin_mousespeed);
			}
			transform_camera *= glm::translate(glm::mat4(1.0f), mouse_speed * glm::vec3(0, 0, 1));
			glutPostRedisplay();
		}
		break;
	case GLUT_WHEEL_DOWN:
		if (state == GLUT_UP) {
			char mousespeedname[maxlen];
			sprintf(mousespeedname, "%s%s", directory, "\\configuration\\mouse_speed.in");
			FILE *fin_mousespeed;
			fin_mousespeed = fopen(mousespeedname, "r");
			float mouse_speed;
			mouse_speed = 0.5;
			if (fin_mousespeed != NULL)
			{
				fscanf(fin_mousespeed, "%f", &mouse_speed);
				fclose(fin_mousespeed);
			}
			transform_camera *= glm::translate(glm::mat4(1.0f), mouse_speed * glm::vec3(0, 0, -1));
			glutPostRedisplay();
		}
		break;
	}
}


void mouse_move_func(int x, int y)
{
	y = win_h - y;
	if (last_x >= 0 && last_y >= 0 && (last_x != x || last_y != y))
	{
		char mousespeedname[maxlen];
		sprintf(mousespeedname, "%s%s", directory, "\\configuration\\mouse_speed.in");
		FILE *fin_mousespeed;
		fin_mousespeed = fopen(mousespeedname, "r");
		float mouse_speed;
		mouse_speed = 0.5;
		if (fin_mousespeed != NULL)
		{
			fscanf(fin_mousespeed, "%f", &mouse_speed);
			fclose(fin_mousespeed);
		}
		GLfloat dx = GLfloat(x - last_x), dy = GLfloat(y - last_y);
		if (r_button_down)
		{
			if (current_sphere == -1)
			{
				transform_camera *= glm::translate(glm::mat4(1.0f), glm::vec3(-mouse_speed * 0.1 * dx, -mouse_speed * 0.1 * dy, 0));
				glutPostRedisplay();
			}
			else
			{
				if (x > last_x)
				{
					spherez[0][current_sphere] -= mouse_speed * 0.3 * (x - last_x);
					spherez[1][current_sphere] -= mouse_speed * 0.3 * (x - last_x);
					glutPostRedisplay();
				}
				else if (last_x > x)
				{
					spherez[0][current_sphere] += mouse_speed * 0.3 * (last_x - x);
					spherez[1][current_sphere] += mouse_speed * 0.3 * (last_x - x);
					glutPostRedisplay();
				}
			}

		}
		else if (l_button_down)
		{
			transform_camera *= glm::rotate(glm::mat4(1.0f), mouse_speed * dy, glm::vec3(1, 0, 0));
			glm::vec3 v = glm::vec3(glm::affineInverse(transform_camera) * glm::vec4(0, 1, 0, 0));
			transform_camera *= glm::rotate(glm::mat4(1.0f), -mouse_speed * dx, v);
			glutPostRedisplay();
		}
	}
	last_x = x; last_y = y;
}

void ChangeSphere()
{
	
}

void keyboard_func(unsigned char key, int x, int y)
{
	switch (key)
	{

	case '0':
		current_sphere = 0;
		ChangeSphere();
		break;
	case '1':
		current_sphere = 1;
		ChangeSphere();
		break;
	case '2':
		current_sphere = 2;
		ChangeSphere();
		break;
	case '3':
		current_sphere = 3;
		ChangeSphere();
		break;
	case '4':
		current_sphere = 4;
		ChangeSphere();
		break;
	case '5':
		current_sphere = 5;
		ChangeSphere();
		break;
	case '6':
		current_sphere = 6;
		ChangeSphere();
		break;
	case '7':
		current_sphere = 7;
		ChangeSphere();
		break;
	case '8':
		current_sphere = 8;
		ChangeSphere();
		break;
	case '9':
		current_sphere = 9;
		ChangeSphere();
		break;
	case 'a':
		current_sphere = 10;
		ChangeSphere();
		break;
	case 'b':
		current_sphere = 11;
		ChangeSphere();
		break;
	case 'c':
		current_sphere = 12;
		ChangeSphere();
		break;
	case 'd':
		current_sphere = 13;
		ChangeSphere();
		break;
	case 'e':
		current_sphere = 14;
		ChangeSphere();
		break;
	case 'f':
		current_sphere = 15;
		ChangeSphere();
		break;

	case 27:
		current_sphere = -1;
	}
	glutPostRedisplay();

}

void specialkey_func(int key, int x, int y)
{
	GLfloat dx = 0, dy = 0, dz = 0;
	char keyboardspeedname_3d[maxlen], keyboardspeedname_2d[maxlen];
	sprintf(keyboardspeedname_3d, "%s%s", directory, "\\configuration\\keyboard_speed_3d.in");
	sprintf(keyboardspeedname_2d, "%s%s", directory, "\\configuration\\keyboard_speed_2d.in");
	FILE *fin_keyboardspeed_3d, *fin_keyboardspeed_2d;
	fin_keyboardspeed_3d = fopen(keyboardspeedname_3d, "r");
	fin_keyboardspeed_2d = fopen(keyboardspeedname_2d, "r");
	float keyboard_speed_3d, keyboard_speed_2d;    //2d: in 224*224
	keyboard_speed_3d = 1.0;
	if (fin_keyboardspeed_3d != NULL)
	{
		fscanf(fin_keyboardspeed_3d, "%f", &keyboard_speed_3d);
		fclose(fin_keyboardspeed_3d);
	}
	keyboard_speed_2d = 1.0;
	if (fin_keyboardspeed_2d != NULL)
	{
		fscanf(fin_keyboardspeed_2d, "%f", &keyboard_speed_2d);
		fclose(fin_keyboardspeed_2d);
	}
	int mod;
	float ratio;
	bool change_sphere = false;
	switch (key)
	{
	case GLUT_KEY_UP:
		ratio = 1.0;
		mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT) //adjust gt
		{
			if (current_sphere != -1)
			{
				//init_set.jointgt[current_sphere * 2 + 1] -= keyboard_speed_2d / 224;
			}
			change_sphere = true;
			break;
		}
		if (mod == (GLUT_ACTIVE_ALT | GLUT_ACTIVE_SHIFT))   //micro adjust gt
		{
			if (current_sphere != -1)
			{
				//init_set.jointgt[current_sphere * 2 + 1] -= keyboard_speed_2d * shift_ratio(directory) / 224;
			}
			change_sphere = true;
			break;
		}
		if (mod == GLUT_ACTIVE_SHIFT) //if "shift": micro adjustment
		{
			ratio = shift_ratio(directory);
		}
		if (current_sphere == -1)
		{
			dy -= keyboard_speed_3d * ratio;
		}
		else //adjust one specific joint 
		{
			change_sphere = true;
			spherey[0][current_sphere] += 0.1 * keyboard_speed_3d * ratio;
			spherey[1][current_sphere] += 0.1 * keyboard_speed_3d * ratio;
			glutPostRedisplay();
		}
		break;
	
	
	case GLUT_KEY_PAGE_DOWN:
		ratio = 1.0;
		mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_SHIFT) //if "shift": micro adjustment
		{
			ratio = shift_ratio(directory);
		}
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			
			return;
		} //next image
		if (current_sphere == -1)
		{
			dz += keyboard_speed_3d * ratio;
		}
		else
		{
			change_sphere = true;
			spherez[0][current_sphere] += 0.1 * keyboard_speed_3d * ratio;
			spherez[1][current_sphere] += 0.1 * keyboard_speed_3d * ratio;
			glutPostRedisplay();
		}
		break;
	
	}
	if (change_sphere)
	{
		ChangeSphere();
	}
	if (abs(dx) > 0 || abs(dz) > 0 || abs(dy)>0) {
		ratio = 1.0;
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) //if "shift": micro adjustment
		{
			ratio = 0.8;
		}
		transform_camera *= glm::translate(glm::mat4(1.0f), keyboard_speed_3d * ratio * glm::vec3(dx, dy, -dz));
		glutPostRedisplay();
	}
}
