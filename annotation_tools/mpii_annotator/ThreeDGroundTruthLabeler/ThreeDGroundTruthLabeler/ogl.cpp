#pragma comment (lib, "libpng16.lib")	
#pragma comment (lib, "zlib.lib")
#include <iostream>
#include <algorithm>
#include "png.h"
#include "gl_inc.h"
#include "basic.h"
#include "cv_related.h"
#include <cmath>
#include "InitConfiguration.h"
#include "DisplayJointOnImage.h"
#include "WeakPerspective.h"
#include "PinholeCamera.h"
#include "PoseConditionedJointAngleLimit_define.h"
using namespace HumanModel;
using namespace cv;
extern int win_w, win_h;
extern float win_fovy;

extern glm::mat4 transform_camera;
extern glm::mat4 transform_model;
extern glm::vec4 position_light0;
extern float mouse_speed;

//from main.cpp
extern double shape_convex_param[ParamNum];
extern double opt_location[JointNum * 3];
extern double opt_param[ParamNum];
extern double opt_angle[3];
extern double opt_trans[2];
extern double opt_scale;
extern int current_image;
extern char directory[maxlen]; //directory\\gt_2d_modified_
extern double origin_projection[JointNum * 2];
extern bool has_been_optimized;
extern double *closest_valid_joint_all;

extern void save_memory();

extern int memory_count;
extern double memory_location[max_memory][JointNum * 3];
extern double memory_projection[max_memory][JointNum * 2];
extern double memory_param[max_memory][ParamNum];
extern double memory_angle[max_memory][3];
extern double memory_trans[max_memory][2]; 
extern double memory_scale[max_memory];
extern Mat src_img;

extern void LoadImageInfo(int id);


//extern from PoseCondtionedJointAngleLimitConstraint.cpp
extern bool *isValid(double *Joint, bool find_closest);

//extern from ModelOptimization.cpp  (2d 3d different types of constraint: angle constraint; pose-conditioned joint angle limit constraint)
extern void Optimize_2d(InitConfiguration work, bool whether_to_add_angle_constraint);
extern void Optimize_2d_pinhole(InitConfiguration work, bool whether_to_add_angle_constraint, bool is_shapeconvex);
extern void Optimize_3d(InitConfiguration work, double *joint_3d, bool whether_to_add_angle_constraint);
extern void Optimize_3d_pinhole(InitConfiguration work, double *joint_3d, bool whether_to_add_angle_constraint);
extern InitConfiguration init_set;      //from main function

//extern from ShapeConvex.cpp
extern void prepare_2d_landmark(InitConfiguration t_init, int id); //prepare 2d gt landmark for shape convex code
extern void FitModelToShapeConvex();

//added:
extern double spherex[JointNum], spherey[JointNum], spherez[JointNum];
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
	draw_frame(len / 3);	
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
			transform_camera *= glm::translate(mouse_speed * glm::vec3(0, 0, 1));
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
			transform_camera *= glm::translate(mouse_speed * glm::vec3(0, 0, -1));
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
				transform_camera *= glm::translate(glm::vec3(-mouse_speed * 0.1 * dx, -mouse_speed * 0.1 * dy, 0));
				glutPostRedisplay();
			}
			else
			{
				if (x > last_x)
				{
					spherez[current_sphere] -= mouse_speed * 0.3 * (x - last_x);
					glutPostRedisplay();
				}
				else if (last_x > x)
				{
					spherez[current_sphere] += mouse_speed * 0.3 * (last_x - x);
					glutPostRedisplay();
				}				
			}

		}
		else if (l_button_down) 
		{
			transform_camera *= glm::rotate(mouse_speed * dy, glm::vec3(1, 0, 0));
			glm::vec3 v = glm::vec3(glm::affineInverse(transform_camera) * glm::vec4(0, 1, 0, 0));
			transform_camera *= glm::rotate(-mouse_speed * dx, v);
			glutPostRedisplay();
		}
	}
	last_x = x; last_y = y;
}

void SetSphereLocation()
{
	cout << "Setting Sphere Location Now\n";
	for (int i = 0; i < JointNum; i++)
	{
		spherex[i] = mul_factor(directory) * opt_location[i * 3]; spherey[i] = mul_factor(directory) * opt_location[i * 3 + 1]; spherez[i] = mul_factor(directory) * opt_location[i * 3 + 2];
		//mul_factor(directory) is just for the convenience of visualization
		printf("sphere %4d %8.2f %8.2f %8.2f\n", i, spherex[i], spherey[i], spherez[i]);		
	}
	glutPostRedisplay();
}

void ChangeSphere()
{
	double *concat_joint;
	concat_joint = concat_spherelocation(spherex, spherey, spherez);
#ifdef use_weak_perspective
	WeakPerspective projector;   // reprojection
	double *t_m_rot = (double *)malloc(sizeof(double)* 6);
	projector.Forward_cpu(concat_joint, t_m_rot, opt_trans, opt_scale, opt_angle);
#else
	PinholeCamera projector;
	projector.Forward_cpu(concat_joint);
#endif
	
	DisplayJoint disp_up;
	//both
	Mat mid_img_both = Mat::zeros(Size(224, 224), CV_8UC3);
	src_img.copyTo(mid_img_both);
	disp_up.Display(mid_img_both, projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj), 1, 1, 1, 1, init_set.visible, init_set.obj.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
	disp_up.DrawRing(mid_img_both, current_sphere, projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj), init_set.visible, init_set.obj.inobj, mul1, mul2, b1, b2, b3);
	resize(mid_img_both, mid_img_both, Size(show_image_size(directory), show_image_size(directory)));
	imshow("show_image", mid_img_both);	

	//only prediction
	Mat mid_img_pred = Mat::zeros(Size(224, 224), CV_8UC3);
	src_img.copyTo(mid_img_pred);
	disp_up.Display(mid_img_pred, projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj),1, 1, 0, 1, init_set.visible, init_set.obj.inobj, NULL, mul1, mul2, b1, b2, b3, directory);	
	disp_up.DrawRing(mid_img_pred, current_sphere, projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj), init_set.visible, init_set.obj.inobj, mul1, mul2, b1, b2, b3);
	resize(mid_img_pred, mid_img_pred, Size(show_image_size(directory), show_image_size(directory)));
	imshow("show_prediction", mid_img_pred);

	//only ground truth
	Mat mid_img_gt = Mat::zeros(Size(224, 224), CV_8UC3);
	src_img.copyTo(mid_img_gt);
	disp_up.Display(mid_img_gt, projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj), 1, 0, 1, 1, init_set.visible, init_set.obj.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
	disp_up.DrawRing(mid_img_gt, current_sphere, projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj), init_set.visible, init_set.obj.inobj, mul1, mul2, b1, b2, b3);
	resize(mid_img_gt, mid_img_gt, Size(show_image_size(directory), show_image_size(directory)));
	imshow("show_ground truth", mid_img_gt);
}

void save_memory() //happens when press the 'm' key
{
	for (int i = 0; i < JointNum * 3; i++) memory_location[memory_count][i] = opt_location[i];
	for (int i = 0; i < JointNum * 2; i++) memory_projection[memory_count][i] = opt_projection[i];
	for (int i = 0; i < ParamNum; i++) memory_param[memory_count][i] = opt_param[i];
#ifdef use_weak_perspective
	for (int i = 0; i < 3; i++) memory_angle[memory_count][i] = opt_angle[i];
	for (int i = 0; i < 2; i++) memory_trans[memory_count][i] = opt_trans[i];
	memory_scale[memory_count][i] = opt_scale;
#endif
	memory_count++;
	printf("Saving current state to memory now!\n");
	printf("Current available memory num: %5d\n", memory_count);
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
	case 'C': //pose conditioned joint angle limit constraint
		//generate test pose(according to the sequence in the paper pose conditioned joint angle limit constraint)
		double *test_pose;
		test_pose = (double *)malloc(sizeof(double)* 3 * joint_num);
		for (int i = 0; i < JointNum; i++) copy(test_pose + index(convert_std2pjcai[i]), opt_location + index(i));
		copy(test_pose + index(r_foot), test_pose + index(r_ankle)); //nonsense
		copy(test_pose + index(l_foot), test_pose + index(l_ankle)); //nonsense
		bool *valid_or_not;
		valid_or_not = (bool *)malloc(sizeof(bool)*bone_num);
		valid_or_not = isValid(test_pose, true);
		bool valid_all;
		valid_all = true;
		for (int i = 0; i < bone_num; i++)
		{
			if (edges_pjcal[i][0] == face || edges_pjcal[i][1] == face || edges_pjcal[i][0] == l_foot || edges_pjcal[i][1] == l_foot || edges_pjcal[i][0] == r_foot || edges_pjcal[i][1] == r_foot) continue;
			//i = 7 or 11 or 15
			if (!valid_or_not[i])
			{
				printf("%s is invalid!\n", edge_pjcal_name[i]);
				valid_all = false;
			}			
		}
		if (valid_all) printf("All bones are valid!\n");
		printf("Replacing it with the closest valid points under the pose-conditioned joint angle limit constraint now!\n");
		double new_joint_3d[JointNum * 3]; //prefix "t_" here stands for temporary				
		for (int i = 0; i < JointNum; i++) copy(new_joint_3d + index(i), opt_location + index(i));
		for (int i = 0; i < JointNum; i++) copy(new_joint_3d + index(i), closest_valid_joint_all + index(convert_std2pjcai[i]));
		for (int i = 0; i < JointNum; i++) copy(opt_location + index(i), new_joint_3d + index(i));
#ifdef use_weak_perspective
		Optimize_3d(init_set, new_joint_3d, false);
#else
		Optimize_3d_pinhole(init_set, new_joint_3d, false);
#endif
		SetSphereLocation();
		cout << "Closest Valid Joint\n";
		for (int i = 0; i < joint_num; i++)
		{
			printf("%35s ", joint_name[i]);
			for (int j = 0; j < 3; j++)
			{
				printf("%12.6f ", closest_valid_joint_all[i * 3 + j]);
			}
			printf("\n");
		}
		return;
	case 'j':
		char statename[maxlen];
		sprintf(statename, "%s%s", directory, "\\configuration\\memory_id.in");
		FILE *fin_stateid;
		fin_stateid = fopen(statename, "r");
		int t_id;
		fscanf(fin_stateid, "%d", &t_id);
		if (t_id < memory_count)  //is in the memory
		{
			for (int i = 0; i < JointNum * 3; i++)  opt_location[i] = memory_location[t_id][i];
			for (int i = 0; i < JointNum * 2; i++) opt_projection[i] = memory_projection[t_id][i];
			for (int i = 0; i < ParamNum; i++) opt_param[i] = memory_param[t_id][i];
			for (int i = 0; i < 3; i++) opt_angle[i] = memory_angle[t_id][i];
			for (int i = 0; i < 2; i++) opt_trans[i] = memory_trans[t_id][i];
			opt_scale = memory_scale[t_id];
			SetSphereLocation();  //redisplay
			DisplayJoint disp_jump;
			Mat mid_img=Mat::zeros(Size(224,224),CV_8UC3);
			src_img.copyTo(mid_img);			
			disp_jump.Display(mid_img, opt_projection, normalize_gt_2d(init_set.jointgt,init_set.obj), 1, 1, 1, 1, init_set.visible, init_set.obj.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
			//explicit cast
		}		
		fclose(fin_stateid);
		return;
	case 'm': //memory				
		save_memory();
		return;
	case 'h': //help
		printf("Current Image : %7d\n", current_image);
		//3d joint locations		
		printf("Current 3D Joint Locations(not from the last optimization):\n");
		for (int i = 0; i < JointNum; i++)
		{
			printf("%35s %12.6f %12.6f %12.6f\n", joint_name[i], spherex[i] / mul_factor(directory), spherey[i] / mul_factor(directory), spherez[i] / mul_factor(directory));
		}
		//angles
		printf("Angles:\n");				
		for (int i = 0; i < ParamNum; i++)
		{
			if (i>=3) printf("%35s %6.2f\n", param_name[i], rad2deg(opt_param[i]));
			else printf("%35s %6.2f\n", param_name[i], opt_param[i]);
		}
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++) printf("%35s %6.2f\n", pers_param_name[i], rad2deg(opt_angle[i]));
		for (int i = 0; i < 2; i++) printf("%35s %6.2f\n", pers_param_name[i + 3], opt_trans[i]);
		printf("%35s %6.2f\n", pers_param_name[5], opt_scale);
#endif
		//memory count
		printf("Current available memory num: %5d\n", memory_count);
		break;
	case 'r': //reset to shape convex but read from external file
		char shapeconvexparamname[maxlen];
		sprintf(shapeconvexparamname, "%s%s%d%s", directory, "\\shapeconvex\\opt_local_param_", current_image, ".txt");
		FILE *fin_convex_param;
		fin_convex_param = fopen(shapeconvexparamname, "r");
		for (int i = 0; i < ParamNum - 3; i++) fscanf(fin_convex_param, "%lf", &shape_convex_param[3 + i]);
		fclose(fin_convex_param);
		Optimize_2d_pinhole(init_set, false, true);
		SetSphereLocation();
		return;
	case 'R':
		prepare_2d_landmark(init_set, current_image);
		FitModelToShapeConvex(); //we get all the local parameters(degrees) of shape convex result
		Optimize_2d_pinhole(init_set, false, true);
		SetSphereLocation();
		return;
	case 'p':   //2d loss (no constraint)		
		printf("Optimizing 2d (no constraint)\n");
#ifdef use_weak_perspective
		Optimize_2d(init_set, false);		
#else
		Optimize_2d_pinhole(init_set, false, false);
#endif
		SetSphereLocation();
		return;
	case 'o': //3d loss (no constraint)
		printf("Optimizing 3d (no constraint)\n");
		double t_joint_3d[JointNum * 3]; //prefix "t_" here stands for temporary
		for (int i = 0; i < JointNum; i++)
		{
			t_joint_3d[i * 3] = spherex[i] / mul_factor(directory);
			t_joint_3d[i * 3 + 1] = spherey[i] / mul_factor(directory);
			t_joint_3d[i * 3 + 2] = spherez[i] / mul_factor(directory);
		}
#ifdef use_weak_perspective
		Optimize_3d(init_set, t_joint_3d, false);
#else
		Optimize_3d_pinhole(init_set, t_joint_3d, false);
#endif
		SetSphereLocation();
		return;
	case 'P': //2d loss(with angle constraint)
		printf("Optimizing 2d (with angle constraint)\n");
#ifdef use_weak_perspective
		Optimize_2d(init_set, true);
#else
		Optimize_2d_pinhole(init_set, true, false);
#endif
		SetSphereLocation();
		return;
	case 'O': //3d loss(with angle constraint)
		printf("Optimizing 3d (with angle constraint)\n");		
		for (int i = 0; i < JointNum; i++)
		{
			t_joint_3d[i * 3] = spherex[i] / mul_factor(directory);
			t_joint_3d[i * 3 + 1] = spherey[i] / mul_factor(directory);
			t_joint_3d[i * 3 + 2] = spherez[i] / mul_factor(directory);
		}
#ifdef use_weak_perspective
		Optimize_3d(init_set, t_joint_3d, true);
#else
		Optimize_3d_pinhole(init_set, t_joint_3d, true);
#endif
		SetSphereLocation();
		return;
	case 'l': //load image
		printf("Loadingimage ");
		memory_count = 0; //clear memory
		char id_file[maxlen];
		sprintf(id_file, "%s%s", directory, "\\configuration\\current_imageid.in");
		FILE *fin_id;
		fin_id = fopen(id_file, "r");
		int id;
		fscanf(fin_id, "%d", &id);
		current_image = id;
		printf("%6d now!\n", current_image);
		LoadImageInfo(current_image);
		if (has_been_optimized) save_memory();
		fclose(fin_id);
		break;
	case 'S':    //saving backup (not the final results)
		cout << "Saving Backup Now(As Intermediate Result)!\n";
		int save_id;
		save_id = GetBackupId(directory);
		//saving modified 2d ground truth
		if (has_difference(init_set.jointgt, origin_projection, JointNum * 2)) //init_set.jointgt may be modified
		{
			char gt_mod[maxlen];
			sprintf(gt_mod, "%s%s%d%s%d%s", directory, "\\intermediate\\mod_", current_image, "_", save_id, ".txt");
			FILE *fout_gt_mod;
			fout_gt_mod = fopen(gt_mod, "w");
			for (int i = 0; i < JointNum; i++)
			{
				fprintf(fout_gt_mod, "%16s %12.6f %12.6f\n", joint_name[i], init_set.jointgt[i * 2], init_set.jointgt[i * 2 + 1]);
			}
			fclose(fout_gt_mod);
		}
		//saving bonelen
		char bonelen_name[maxlen];
		sprintf(bonelen_name, "%s%s%d%s%d%s", directory, "\\intermediate\\bonelen_", current_image, "_", save_id, ".in");
		FILE *fout_bonelen;
		fout_bonelen = fopen(bonelen_name, "w");
		for (int i = 0; i < BoneNum; i++)
		{
			fprintf(fout_bonelen, "%35s%s%.2f\n", bone_name[i], "   ", init_set.obj.bonelen[i]);
		}
		fclose(fout_bonelen);
		//saving pred 2d
		char projfile[maxlen];
		sprintf(projfile, "%s%s%d%s%d%s", directory, "\\intermediate\\pred_2d_", current_image, "_", save_id, ".txt");
		FILE *fout_predproj;
		fout_predproj = fopen(projfile, "w");
		for (int i = 0; i < JointNum; i++) fprintf(fout_predproj, "%16s %12.6f %12.6f\n", joint_name[i], opt_projection[i * 2], opt_projection[i * 2 + 1]);		
		fclose(fout_predproj);
		//saving pred 3d locations
		char locfile[maxlen];
		sprintf(locfile, "%s%s%d%s%d%s", directory, "\\intermediate\\pred_3d_", current_image, "_", save_id, ".txt");
		FILE *fout_location;
		fout_location = fopen(locfile, "w");
		for (int i = 0; i < JointNum; i++) fprintf(fout_location, "%16s %12.6f %12.6f %12.6f\n", joint_name[i], opt_location[i * 3], opt_location[i * 3 + 1], opt_location[i * 3 + 2]);		
		fclose(fout_location);		
		//saving limit dof
		char islimitedfile[maxlen];
		sprintf(islimitedfile, "%s%s%d%s%d%s", directory, "\\intermediate\\islimited_", current_image, "_", save_id, ".in");
		FILE *fout_islimited;
		fout_islimited = fopen(islimitedfile, "w");
		for (int i = 0; i < ParamNum; i++) fprintf(fout_islimited, "%35s%s%d\n", param_name[i], "    ", init_set.obj.islimited[i]);		
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++)	fprintf(fout_islimited, "%35s%s%d\n", pers_param_name[i], "    ", init_set.obj.limit_pers_angle[i]);		
		for (int i = 0; i < 2; i++)	fprintf(fout_islimited, "%35s%s%d\n", pers_param_name[i + 3], "    ", init_set.obj.limit_pers_trans[i]);		
		fprintf(fout_islimited, "%35s%s%d\n", pers_param_name[5], "    ", init_set.obj.limit_pers_scale);
#endif
		fclose(fout_islimited);
		
		//saving initparam
		char initparamfile[maxlen];
		sprintf(initparamfile, "%s%s%d%s%d%s", directory, "\\intermediate\\initparam_", current_image, "_", save_id, ".in");
		FILE *fout_initparam;
		fout_initparam = fopen(initparamfile, "w");
		for (int i = 0; i < ParamNum; i++)
		{
			if (i<3) fprintf(fout_initparam, "%35s %12.6f\n", param_name[i], init_set.obj.initparam[i]); //translation parameters
			else fprintf(fout_initparam, "%35s %12.6f\n", param_name[i], rad2deg(init_set.obj.initparam[i])); //all saving files & visualizing uses degree while intermediate use degree
		}
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++) fprintf(fout_initparam, "%35s %12.6f\n", pers_param_name[i], rad2deg(init_set.obj.init_pers_angle[i]));
		for (int i = 0; i < 2; i++) fprintf(fout_initparam, "%35s %12.6f\n", pers_param_name[i + 3], init_set.obj.init_pers_trans[i]);
		fprintf(fout_initparam, "%35s %12.6f\n", pers_param_name[5], init_set.obj.init_pers_scale);
#endif
		fclose(fout_initparam);

		//saving parameters
		char paramfile[maxlen];
		sprintf(paramfile, "%s%s%d%s%d%s", directory, "\\intermediate\\param_", current_image, "_", save_id, ".txt");
		FILE *fout_param;
		fout_param = fopen(paramfile, "w");
		for (int i = 0; i < ParamNum; i++)
		{
			if (i<3) fprintf(fout_param, "%35s %12.6f\n", param_name[i], opt_param[i]); //translation parameters
			else fprintf(fout_param, "%35s %12.6f\n", param_name[i], rad2deg(opt_param[i])); //all saving files & visualizing uses degree while intermediate use degree
		}
#ifdef use_weak_perspective
		fprintf(fout_param, "%35s %12.6f\n", "angle_x", rad2deg(opt_angle[0]));
		fprintf(fout_param, "%35s %12.6f\n", "angle_y", rad2deg(opt_angle[1]));
		fprintf(fout_param, "%35s %12.6f\n", "angle_z", rad2deg(opt_angle[2]));  //save as degree
		fprintf(fout_param, "%35s %12.6f\n", "trans_x", opt_trans[0]);
		fprintf(fout_param, "%35s %12.6f\n", "trans_y", opt_trans[1]);
		fprintf(fout_param, "%35s %12.6f\n", "scale", opt_scale);
#endif
		fclose(fout_param);

		//saving inobj
		char inobjfile[maxlen];
		sprintf(inobjfile, "%s%s%d%s%d%s", directory, "\\intermediate\\inobj_", current_image, "_", save_id, ".in");
		FILE *fout_inobj;
		fout_inobj = fopen(inobjfile, "w");
		for (int i = 0; i < JointNum; i++) fprintf(fout_inobj, "%35s%s%d\n", joint_name[i], "     ", init_set.obj.inobj[i]);
		fclose(fout_inobj);

		//saving angle constraint
		char angleconstraintfile[maxlen];
		sprintf(angleconstraintfile, "%s%s%d%s%d%s", directory, "\\intermediate\\angleconstraint_", current_image, "_", save_id, ".in");
		FILE *fout_angleconstraint;
		fout_angleconstraint = fopen(angleconstraintfile, "w");
		for (int i = 0; i < ParamNum; i++)
		{
			if (i>=3) fprintf(fout_angleconstraint, "%35s %12.6f %12.6f\n", param_name[i], rad2deg(init_set.obj.local_lb[i]), rad2deg(init_set.obj.local_ub[i]));
			else fprintf(fout_angleconstraint, "%35s %12.6f %12.6f\n", param_name[i], init_set.obj.local_lb[i], init_set.obj.local_ub[i]);
		}
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++) fprintf(fout_angleconstraint, "%35s %12.6f %12.6f\n", pers_param_name[i], rad2deg(init_set.obj.pers_global_rot_lb[i]), rad2deg(init_set.obj.pers_global_rot_ub[i]));
		for (int i = 0; i < 2; i++) fprintf(fout_angleconstraint, "%35s %12.6f %12.6f\n", pers_param_name[i + 3], init_set.obj.pers_global_trans_lb[i], init_set.obj.pers_global_trans_ub[i]);
		fprintf(fout_angleconstraint, "%35s %12.6f %12.6f\n", pers_param_name[5], init_set.obj.pers_scale_lb, init_set.obj.pers_scale_ub);
#endif
		fclose(fout_angleconstraint);

		//saving interpretations
		char interpretationfile[maxlen];
		sprintf(interpretationfile, "%s%s%d%s%d%s", directory, "\\intermediate\\interpretation_", current_image, "_", save_id, ".txt");
		FILE *fout_interpretation;
		fout_interpretation = fopen(interpretationfile, "w");
		fprintf(fout_interpretation, "   \n");
		fclose(fout_interpretation);
		break;
	case 'v':
		cout << "Saving Final Now!\n";
		

		//saving parameters
		//char paramfile[maxlen];
		sprintf(paramfile, "%s%s%d%s", directory, "\\result\\param_", current_image, ".txt");
		//FILE *fout_param;
		fout_param = fopen(paramfile, "w");
		for (int i = 0; i < ParamNum; i++)
		{
			if (i<3) fprintf(fout_param, "%35s %12.6f\n", param_name[i], opt_param[i]); //translation parameters
			else fprintf(fout_param, "%35s %12.6f\n", param_name[i], rad2deg(opt_param[i] + shape_convex_param[i] )); //all saving files & visualizing uses degree while intermediate use degree
		}

		fclose(fout_param);
		//saving projections
		//char projfile[maxlen];
		sprintf(projfile, "%s%s%d%s", directory, "\\result\\pred_2d_", current_image, ".txt");
		//FILE *fout_predproj;
		fout_predproj = fopen(projfile, "w");
		for (int i = 0; i < JointNum; i++)
		{
			fprintf(fout_predproj, "%16s %12.6f %12.6f\n", joint_name[i], opt_projection[i * 2], opt_projection[i * 2 + 1]);
		}
		fclose(fout_predproj);
		//saving 3d locations
		//char locfile[maxlen];
		sprintf(locfile, "%s%s%d%s", directory, "\\result\\pred_3d_", current_image, ".txt");
		//FILE *fout_location;
		fout_location = fopen(locfile, "w");
		for (int i = 0; i < JointNum; i++)
		{
			fprintf(fout_location, "%16s %12.6f %12.6f %12.6f\n", joint_name[i], opt_location[i * 3], opt_location[i * 3 + 1], opt_location[i * 3 + 2]);
		}
		fclose(fout_location);
		break;
	case 's':				
		cout << "Saving Final Now!\n";
		//saving modified 2d ground truth
		if (has_difference(init_set.jointgt, origin_projection, JointNum * 2)) //init_set.jointgt may be modified
		{
			char gt_mod[maxlen];
			sprintf(gt_mod, "%s%s%d%s", directory, "\\gt_2d_modified\\mod_", current_image, ".txt");
			FILE *fout_gt_mod;
			fout_gt_mod = fopen(gt_mod, "w");
			for (int i = 0; i < JointNum; i++)
			{
				fprintf(fout_gt_mod, "%16s %12.6f %12.6f\n", joint_name[i], init_set.jointgt[i * 2], init_set.jointgt[i * 2 + 1]);
			}
			fclose(fout_gt_mod);
		}
		
		//saving parameters
		//char paramfile[maxlen];
		sprintf(paramfile, "%s%s%d%s", directory, "\\result\\param_", current_image, ".txt");
		//FILE *fout_param;
		fout_param = fopen(paramfile, "w");
		for (int i = 0; i < ParamNum; i++)
		{
			if (i<3) fprintf(fout_param, "%35s %12.6f\n", param_name[i], opt_param[i]); //translation parameters
			else fprintf(fout_param, "%35s %12.6f\n", param_name[i], rad2deg(opt_param[i])); //all saving files & visualizing uses degree while intermediate use degree
		}
#ifdef use_weak_perspective
		fprintf(fout_param, "%35s %12.6f\n", "angle_x", rad2deg(opt_angle[0]));
		fprintf(fout_param, "%35s %12.6f\n", "angle_y", rad2deg(opt_angle[1]));
		fprintf(fout_param, "%35s %12.6f\n", "angle_z", rad2deg(opt_angle[2]));  //save as degree
		fprintf(fout_param, "%35s %12.6f\n", "trans_x", opt_trans[0]);
		fprintf(fout_param, "%35s %12.6f\n", "trans_y", opt_trans[1]);
		fprintf(fout_param, "%35s %12.6f\n", "scale", opt_scale);
#endif
		fclose(fout_param);
		//saving projections
		//char projfile[maxlen];
		sprintf(projfile, "%s%s%d%s", directory, "\\result\\pred_2d_", current_image, ".txt");
		//FILE *fout_predproj;
		fout_predproj = fopen(projfile, "w");
		for (int i = 0; i < JointNum; i++)
		{
			fprintf(fout_predproj, "%16s %12.6f %12.6f\n", joint_name[i], opt_projection[i * 2], opt_projection[i * 2 + 1]);
		}
		fclose(fout_predproj);
		//saving 3d locations
		//char locfile[maxlen];
		sprintf(locfile, "%s%s%d%s", directory, "\\result\\pred_3d_", current_image, ".txt");
		//FILE *fout_location;
		fout_location = fopen(locfile, "w");
		for (int i = 0; i < JointNum; i++)
		{
			fprintf(fout_location, "%16s %12.6f %12.6f %12.6f\n", joint_name[i], opt_location[i * 3], opt_location[i * 3 + 1], opt_location[i * 3 + 2]);
		}
		fclose(fout_location);
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
				init_set.jointgt[current_sphere * 2 + 1] -= keyboard_speed_2d / 224;
			}
			change_sphere = true;
			break;
		}
		if (mod == (GLUT_ACTIVE_ALT | GLUT_ACTIVE_SHIFT))   //micro adjust gt
		{
			if (current_sphere != -1)
			{				
				init_set.jointgt[current_sphere * 2 + 1] -= keyboard_speed_2d * shift_ratio(directory) / 224;
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
			spherey[current_sphere] += 0.1 * keyboard_speed_3d * ratio;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_DOWN:		
		ratio = 1.0;
		mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT)
		{
			if (current_sphere != -1)
			{
				init_set.jointgt[current_sphere * 2 + 1] += keyboard_speed_2d / 224;
			}
			change_sphere = true;
			break;
		}
		if (mod == (GLUT_ACTIVE_ALT | GLUT_ACTIVE_SHIFT))   //micro adjust gt
		{
			if (current_sphere != -1)
			{
				init_set.jointgt[current_sphere * 2 + 1] += keyboard_speed_2d * shift_ratio(directory) / 224;
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
			dy += keyboard_speed_3d * ratio;
		}
		else
		{
			change_sphere = true;
			spherey[current_sphere] -= 0.1 * keyboard_speed_3d * ratio;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_LEFT:
		ratio = 1.0;
		mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT)
		{
			if (current_sphere != -1)
			{
				init_set.jointgt[current_sphere * 2] -= keyboard_speed_2d / 224;
			}
			change_sphere = true;
			break;
		}
		if (mod == (GLUT_ACTIVE_ALT | GLUT_ACTIVE_SHIFT))  //micro adjust gt
		{
			if (current_sphere != -1)
			{
				init_set.jointgt[current_sphere * 2] -= keyboard_speed_2d * shift_ratio(directory) / 224;
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
			dx += keyboard_speed_3d * ratio;
		}
		else
		{
			change_sphere = true;
			spherex[current_sphere] -= 0.1 * keyboard_speed_3d * ratio;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_RIGHT:
		ratio = 1.0;
		mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_ALT)
		{
			if (current_sphere != -1)
			{
				init_set.jointgt[current_sphere * 2] += keyboard_speed_2d / 224;
			}
			change_sphere = true;
			break;
		}
		if (mod == (GLUT_ACTIVE_ALT | GLUT_ACTIVE_SHIFT))   //micro adjust gt
		{
			if (current_sphere != -1)
			{
				init_set.jointgt[current_sphere * 2] += keyboard_speed_2d * shift_ratio(directory) / 224;
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
			dx -= keyboard_speed_3d * ratio;
		}
		else
		{
			change_sphere = true;
			spherex[current_sphere] += 0.1 * keyboard_speed_3d * ratio;
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
			if (current_image + 1 < TrainingSize)
			{
				current_image++;
				memory_count = 0; //clear memory
				LoadImageInfo(current_image);				
			} 
			return;
		} //next image
		if (current_sphere == -1)
		{
			dz += keyboard_speed_3d * ratio;
		}
		else
		{
			change_sphere = true;
			spherez[current_sphere] += 0.1 * keyboard_speed_3d * ratio;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_PAGE_UP:
		ratio = 1.0;		
		mod = glutGetModifiers();
		if (mod == GLUT_ACTIVE_SHIFT)
		{
			ratio = shift_ratio(directory);
		}
		if (mod == GLUT_ACTIVE_CTRL)
		{
			if (current_image)
			{
				current_image--;
				memory_count = 0; //clear memory
				LoadImageInfo(current_image);				
			}
			return;
		} //next image
		if (current_sphere == -1)
		{
			dz -= keyboard_speed_3d * ratio;
		}
		else
		{
			change_sphere = true;
			spherez[current_sphere] -= 0.1 * keyboard_speed_3d * ratio;
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
		transform_camera *= glm::translate(keyboard_speed_3d * ratio * glm::vec3(dx, dy, -dz));
		glutPostRedisplay();
	}
}
