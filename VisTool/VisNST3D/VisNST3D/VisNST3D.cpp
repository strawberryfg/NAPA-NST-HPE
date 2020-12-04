#include <cstdio >
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <omp.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "basic.h"
#include "gl_inc.h"
#include "glm/glm.hpp"
#include "HumanModel_define.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
int use_comp = 0;
double std_pose[3] = { 0.0, 0.0, 1.0 };
#define maxn 111111
#define ratio 0.1 //gt is 2 times pred
char directory[maxlen] = "E:\\HandHumanVSProjects\\Human\\Vis\\";
#define show_pred
#define read_from_separate

using namespace std;
using namespace cv;
//
int win_w = 1920, win_h = 1080;

glm::mat4 transform_camera(1.0f);
glm::mat4 transform_model(1.0f);
glm::vec4 position_light0(0);
double spherex[2][JointNum], spherey[2][JointNum], spherez[2][JointNum]; //0 stands for prediction 1 stands for ground truth
int current_sphere = -1;
float win_fovy = 45;
float speed_scale = 0.5f;



void reshape_func(int w, int h);
void display_func();
void mouse_click_func(int button, int state, int x, int y);
void mouse_move_func(int x, int y);
void keyboard_func(unsigned char key, int x, int y);
void specialkey_func(int key, int x, int y);

void draw_frame(float len);
void draw_world(float len, float tex_repeat, bool yoz_plane, bool zox_plane, bool xoy_plane);
void ogl_init();





const int color_gt_bone[BoneBasedBoneNum][3] =
{
	{ 255, 0, 0 },       //0  right_knee    ->  right_ankle
	{ 255, 0, 0 },       //1  right_knee    ->  right_hip
	{ 255, 0, 0 },       //2  pelvis        ->  right_hip
	{ 0, 0, 255 }, //3  pelvis        ->  left_hip
	{ 0, 0, 255 }, //4  left_hip      ->  left_knee
	{ 0, 0, 255 }, //5  left_knee     ->  left_ankle
	{ 178, 0, 178 },    //6  thorax        ->  pelvis
	{ 178, 0, 178 },    //7  thorax        ->  upper_neck
	{ 178, 0, 178 },    //8  upper_neck    ->  head_top
	{ 255, 0, 0 },       //9  right_elbow   ->  right_wrist
	{ 255, 0, 0 },       //10 right_shoulder->  right_elbow
	{ 255, 0, 0 },       //11 thorax        ->  right_shoulder
	{ 0, 0, 255 }, //12 thorax        ->  left_shoulder
	{ 0, 0, 255 }, //13 left_shoulder ->  left_elbow
	{ 0, 0, 255 }, //14 left_elbow    ->  left_wrist
};

//End Of OpenGL Part

#ifdef show_pred
double pred_joint_3d[JointNum * 3];
char pred_joint_3d_file[maxlen];
double all_pred_joint_3d[maxn][JointNum * 3];

double rot_pred_joint_3d[maxn][JointNum * 3];    //rotate around an axis (in another viewpoint)


#endif
#ifdef show_gt
char gt_joint_3d_file[maxlen];
double all_gt_joint_3d[maxn][JointNum * 3];
#endif


int number_of_image;
int current_image = 0;
int start_index;
int read_pred_separate, read_gt_separate;
int read_pred_start_index, read_gt_start_index;

double mul_x_factor, mul_y_factor, mul_z_factor;
bool whether_subtract_wrist; //when set to true, subtract all other joints by the root: wrist for better visualization

double global_rot_theta = 30.0; //in degree
double getcolor(int opt, double c) {
	return opt == 0 ? c : 255;
}


double* rot_mat_around_axis(int axis_id, double theta)
{
	double* rot_matr = (double*)malloc(sizeof(double) * 3 * 3);
	for (int t = 0; t < 9; t++) rot_matr[t] = 0.0;

	if (axis_id == 1) //y
	{
		rot_matr[0] = cos(theta);
		rot_matr[2] = -sin(theta);
		rot_matr[4] = 1;
		rot_matr[6] = sin(theta);
		rot_matr[8] = cos(theta);

	}
	return rot_matr;
}

double* matrxvec(double* matr, double* vec)
{
	double* ret_vec = (double*)malloc(sizeof(double) * 3);
	for (int t = 0; t < 3; t++) ret_vec[t] = 0.0;
	for (int k = 0; k < 3; k++)
	{
		for (int row = 0; row < 3; row++)
		{
			ret_vec[row] += matr[row * 3 + k] * vec[k];
		}
	}
	return ret_vec;
}

void draw_sphere(int opt) {
	GLfloat color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//if (opt == 1) color[1] = color[2] = 0.0;
	glMaterialfv(GL_FRONT, GL_SPECULAR, color);
	glMaterialf(GL_FRONT, GL_SHININESS, 30.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glutSolidSphere(sphere_radius(NULL) * (opt == 0 ? 1.0 : ratio), 30, 30);

}

void draw_local() {
	bool to_show_gt = false;
	bool to_show_pred = false;
#ifdef show_gt
	to_show_gt = true;
#endif
#ifdef show_pred
	to_show_pred = true;
#endif

#ifdef draw_local_axis

	for (int i = 0; i < 2; i++) {
		if (!to_show_gt && i == 1) break;
		if (!to_show_pred && i == 0) continue;
		glm::mat4 model_view_matrix = glm::affineInverse(transform_camera);
		model_view_matrix *= glm::translate(glm::vec3(spherex[i][current_sphere], spherey[i][current_sphere], spherez[i][current_sphere]));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&model_view_matrix[0][0]);
		GLfloat color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		//draw X axis
		glPushMatrix();
		glTranslatef(0.5, 0, 0);
		glScalef(1.0f, 0.1f, 0.1f);
		glutSolidCube(1.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(1.0, 0, 0);
		glRotatef(90, 0, 1, 0);
		glutSolidCone(0.1, 0.2, 50, 50);
		glPopMatrix();
		//draw Y axis
		color[0] = .0f; color[1] = 1.0f; color[2] = .0f;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		glPushMatrix();
		glTranslatef(0, 0.5, 0);
		glScalef(0.1f, 1.0f, 0.1f);
		glutSolidCube(1.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 1.0, 0);
		glRotatef(-90, 1, 0, 0);
		glutSolidCone(0.1, 0.2, 50, 50);
		glPopMatrix();
		//draw Z axis
		color[0] = .0f; color[1] = .0f; color[2] = 1.0f;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		glPushMatrix();
		glTranslatef(0, 0, 0.5);
		glScalef(0.1f, 0.1f, 1.0f);
		glutSolidCube(1.0);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, 1.0);
		glutSolidCone(0.1, 0.2, 50, 50);
		glPopMatrix();
	}
#endif
}


double new_loc_arr[JointNum * 3];

void RotateAroundY()
{
	//O
	double vec_origin[3];
	vec_origin[0] = -all_pred_joint_3d[0][pelvis * 3];
	vec_origin[1] = -all_pred_joint_3d[0][pelvis * 3 + 1];
	vec_origin[2] = -all_pred_joint_3d[0][pelvis * 3 + 2];
	double* rot_o_around_y = rot_mat_around_axis(1, global_rot_theta / 180.0 * PI); //rot origin O around Y axis
	rot_o_around_y = matrxvec(rot_o_around_y, vec_origin); //plus translation


	double new_vec_origin[3];
	new_vec_origin[0] = rot_o_around_y[0] + all_pred_joint_3d[0][pelvis * 3];
	new_vec_origin[1] = rot_o_around_y[1] + all_pred_joint_3d[0][pelvis * 3 + 1];
	new_vec_origin[2] = rot_o_around_y[2] + all_pred_joint_3d[0][pelvis * 3 + 2];
	printf("%12.6f %12.6f %12.6f new O in original coordinate space\n", new_vec_origin[0], new_vec_origin[1], new_vec_origin[2]);

	//X
	double vec_x[3];
	vec_x[0] = 1 - all_pred_joint_3d[0][pelvis * 3];
	vec_x[1] = -all_pred_joint_3d[0][pelvis * 3 + 1];
	vec_x[2] = -all_pred_joint_3d[0][pelvis * 3 + 2];
	double* rot_x_around_y = rot_mat_around_axis(1, global_rot_theta / 180.0 * PI); //rot origin O around Y axis
	rot_x_around_y = matrxvec(rot_x_around_y, vec_x); //plus translation

	double new_vec_x[3];
	new_vec_x[0] = rot_x_around_y[0] + all_pred_joint_3d[0][pelvis * 3];
	new_vec_x[1] = rot_x_around_y[1] + all_pred_joint_3d[0][pelvis * 3 + 1];
	new_vec_x[2] = rot_x_around_y[2] + all_pred_joint_3d[0][pelvis * 3 + 2];
	printf("%12.6f %12.6f %12.6f new X in original coordinate space\n", new_vec_x[0], new_vec_x[1], new_vec_x[2]);

	//Y
	double vec_y[3];
	vec_y[0] = -all_pred_joint_3d[0][pelvis * 3];
	vec_y[1] = 1 - all_pred_joint_3d[0][pelvis * 3 + 1];
	vec_y[2] = -all_pred_joint_3d[0][pelvis * 3 + 2];

	double* rot_y_around_y = rot_mat_around_axis(1, global_rot_theta / 180.0 * PI); //rot origin O around Y axis
	rot_y_around_y = matrxvec(rot_y_around_y, vec_y); //plus translation

	double new_vec_y[3];
	new_vec_y[0] = rot_y_around_y[0] + all_pred_joint_3d[0][pelvis * 3];
	new_vec_y[1] = rot_y_around_y[1] + all_pred_joint_3d[0][pelvis * 3 + 1];
	new_vec_y[2] = rot_y_around_y[2] + all_pred_joint_3d[0][pelvis * 3 + 2];
	printf("%12.6f %12.6f %12.6f new Y in original coordinate space\n", new_vec_y[0], new_vec_y[1], new_vec_y[2]);

	//Z
	double vec_z[3];
	vec_z[0] = -all_pred_joint_3d[0][pelvis * 3];
	vec_z[1] = -all_pred_joint_3d[0][pelvis * 3 + 1];
	vec_z[2] = -1 - all_pred_joint_3d[0][pelvis * 3 + 2];

	double* rot_z_around_y = rot_mat_around_axis(1, global_rot_theta / 180.0 * PI); //rot origin O around Y axis
	rot_z_around_y = matrxvec(rot_z_around_y, vec_z); //plus translation

	double new_vec_z[3];
	new_vec_z[0] = rot_z_around_y[0] + all_pred_joint_3d[0][pelvis * 3];
	new_vec_z[1] = rot_z_around_y[1] + all_pred_joint_3d[0][pelvis * 3 + 1];
	new_vec_z[2] = rot_z_around_y[2] + all_pred_joint_3d[0][pelvis * 3 + 2];
	printf("%12.6f %12.6f %12.6f new Z in original coordinate space\n", new_vec_z[0], new_vec_z[1], new_vec_z[2]);

	//=====min max of x y z
	double min_x = 11111.0, max_x = -11111.0, min_y = 11111.0, max_y = -11111.0, min_z = 11111.0, max_z = -11111.0;

	//rotate all joints
	for (int i = 0; i < JointNum; i++)
	{
		//x, y, z of this joint (on original camera frame)
		double xa = all_pred_joint_3d[0][i * 3];
		double ya = all_pred_joint_3d[0][i * 3 + 1];
		double za = all_pred_joint_3d[0][i * 3 + 2];

		//==============intercept of X
		//xx'-xo'
		double ux = new_vec_x[0] - new_vec_origin[0];
		//yx'-yo'
		double uy = new_vec_x[1] - new_vec_origin[1];
		//zx'-zo'
		double uz = new_vec_x[2] - new_vec_origin[2];

		double fz = (xa * ux + ya * uy + za * uz);
		double fm = (ux * ux + uy * uy + uz * uz);

		//------ intercept on X axis
		double new_x = fz / fm;
		//==============intercept of X




		//==============intercept of Y
		//xy'-xo'
		ux = new_vec_y[0] - new_vec_origin[0];
		//yy'-yo'
		uy = new_vec_y[1] - new_vec_origin[1];
		//zy'-zo'
		uz = new_vec_y[2] - new_vec_origin[2];

		fz = (xa * ux + ya * uy + za * uz);
		fm = (ux * ux + uy * uy + uz * uz);

		//------ intercept on Y axis
		double new_y = fz / fm;
		//==============intercept of Y






		//==============intercept of Z
		//xz'-xo'
		ux = new_vec_z[0] - new_vec_origin[0];
		//yz'-yo'
		uy = new_vec_z[1] - new_vec_origin[1];
		//zz'-zo'
		uz = new_vec_z[2] - new_vec_origin[2];

		fz = (xa * ux + ya * uy + za * uz);
		fm = (ux * ux + uy * uy + uz * uz);

		//------ intercept on Y axis
		double new_z = fz / fm;
		//==============intercept of Z

		rot_pred_joint_3d[0][i * 3] = new_x * (new_vec_x[0] - new_vec_origin[0]) + new_vec_origin[0]; //s * delta_x + o'
		rot_pred_joint_3d[0][i * 3 + 1] = new_y * (new_vec_y[1] - new_vec_origin[1]) + new_vec_origin[1];
		rot_pred_joint_3d[0][i * 3 + 2] = new_z * (new_vec_z[2] - new_vec_origin[2]) + new_vec_origin[2];

		/*rot_pred_joint_3d[0][i * 3] = new_x + all_pred_joint_3d[0][pelvis * 3]; //s * delta_x + o'
		rot_pred_joint_3d[0][i * 3 + 1] = new_y + all_pred_joint_3d[0][pelvis * 3 + 1];
		rot_pred_joint_3d[0][i * 3 + 2] = new_z + all_pred_joint_3d[0][pelvis * 3 + 2];*/


		printf("%4d %12.6f %12.6f %12.6f\n", i, new_x, new_y, new_z);

		new_loc_arr[i * 3] = new_x;
		new_loc_arr[i * 3 + 1] = new_y;
		new_loc_arr[i * 3 + 2] = new_z;

		min_x = min(min_x, new_x);
		max_x = max(max_x, new_x);

		min_y = min(min_y, new_y);
		max_y = max(max_y, new_y);

		min_z = min(min_z, new_z);
		max_z = max(max_z, new_z);
	}

	//Show projection on xy space
	double scale_x = 1.25;
	double scale_y = 1.25;
	double scale_z = 1.25;
	double avg_x = (min_x + max_x) / 2.0;
	double avg_y = (min_y + max_y) / 2.0;
	double avg_z = (min_z + max_z) / 2.0;

	double delta_x = max(avg_x - min_x, max_x - avg_x) * scale_x;
	double delta_y = max(avg_y - min_y, max_y - avg_y) * scale_y;
	double delta_z = max(avg_z - min_z, max_z - avg_z) * scale_z;


	delta_x = 0.6;
	delta_y = 0.6;
	delta_z = 0.25;

	min_x = avg_x - delta_x;
	max_x = avg_x + delta_x;

	min_y = avg_y - delta_y;
	max_y = avg_y + delta_y;

	min_z = avg_z - delta_z;
	max_z = avg_z + delta_z;


	double t_proj[JointNum * 2];
	double root_joint[3];
	root_joint[0] = new_loc_arr[pelvis * 3];
	root_joint[1] = new_loc_arr[pelvis * 3 + 1];
	root_joint[2] = new_loc_arr[pelvis * 3 + 2];

	//projection on XY space
	for (int i = 0; i < JointNum; i++)
	{
		t_proj[i * 2] = (new_loc_arr[i * 3] - root_joint[0]) / (max_x - min_x) + 0.5;
		t_proj[i * 2 + 1] = (new_loc_arr[i * 3 + 1] - root_joint[1]) / (max_y - min_y) + 0.5;
	}

	int S = 224;
	Mat img = Mat::zeros(Size(S, S), CV_8UC3);
	for (int i = 0; i < BoneBasedBoneNum; i++)
	{
		double x1 = t_proj[bones[i][0] * 2] * S;
		double x2 = t_proj[bones[i][1] * 2] * S;
		double y1 = S - t_proj[bones[i][0] * 2 + 1] * S;
		double y2 = S - t_proj[bones[i][1] * 2 + 1] * S;
		line(img, Point(x1, y1), Point(x2, y2), Scalar(color_gt_bone[i][0], color_gt_bone[i][1], color_gt_bone[i][2]), 10);
	}
	imshow("XY", img);
	waitKey(0);



	//projection on XZ space
	for (int i = 0; i < JointNum; i++)
	{
		t_proj[i * 2] = (new_loc_arr[i * 3] - root_joint[0]) / (max_x - min_x) + 0.5;
		t_proj[i * 2 + 1] = (new_loc_arr[i * 3 + 2] - root_joint[2]) / (max_z - min_z) + 0.5;
	}

	S = 224;
	img = Mat::zeros(Size(S, S), CV_8UC3);
	for (int i = 0; i < BoneBasedBoneNum; i++)
	{
		double x1 = t_proj[bones[i][0] * 2] * S;
		double x2 = t_proj[bones[i][1] * 2] * S;
		double y1 = S - t_proj[bones[i][0] * 2 + 1] * S;
		double y2 = S - t_proj[bones[i][1] * 2 + 1] * S;
		line(img, Point(x1, y1), Point(x2, y2), Scalar(color_gt_bone[i][0], color_gt_bone[i][1], color_gt_bone[i][2]), 10);
	}
	imshow("XZ", img);
	waitKey(0);


	//projection on YZ space
	for (int i = 0; i < JointNum; i++)
	{
		t_proj[i * 2] = (new_loc_arr[i * 3 + 1] - root_joint[1]) / (max_y - min_y) + 0.5;
		t_proj[i * 2 + 1] = (new_loc_arr[i * 3 + 2] - root_joint[2]) / (max_z - min_z) + 0.5;
	}

	S = 224;
	img = Mat::zeros(Size(S, S), CV_8UC3);
	for (int i = 0; i < BoneBasedBoneNum; i++)
	{
		double x1 = t_proj[bones[i][0] * 2] * S;
		double x2 = t_proj[bones[i][1] * 2] * S;
		double y1 = t_proj[bones[i][0] * 2 + 1] * S;
		double y2 = t_proj[bones[i][1] * 2 + 1] * S;
		line(img, Point(x1, y1), Point(x2, y2), Scalar(color_gt_bone[i][0], color_gt_bone[i][1], color_gt_bone[i][2]), 10);
	}
	imshow("YZ", img);
	waitKey(0);
}


void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::mat4 model_view_matrix = glm::affineInverse(transform_camera);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&model_view_matrix[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &position_light0[0]); // 位置式光源
	draw_world(30, 3, true, true, true); // 绘制世界

	bool to_show_gt = false;
	bool to_show_pred = false;
#ifdef show_gt
	to_show_gt = true;
#endif
#ifdef show_pred
	to_show_pred = true;
#endif
	for (int j = 0; j < 2; j++) {
		if (!to_show_gt && j == 1) break;
		if (!to_show_pred && j == 0) continue;
		//draw a sphere
		for (int i = 0; i < JointNum; i++) {
			model_view_matrix = glm::affineInverse(transform_camera);
			model_view_matrix *= glm::translate(glm::mat4(1.0f), glm::vec3(spherex[j][i], spherey[j][i], spherez[j][i]));
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(&model_view_matrix[0][0]);
			draw_sphere(j);
		}
	}


	//draw local coordinate system
	draw_local();

	//restore matrix
	model_view_matrix = glm::affineInverse(transform_camera);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&model_view_matrix[0][0]);
	//draw bones between spheres

	for (int j = 0; j < 2; j++) {
		if (!to_show_gt && j == 1) break;
		if (!to_show_pred && j == 0) continue;
		for (int i = 0; i < BoneBasedBoneNum; i++) {
			GLfloat color[] = { getcolor(j, color_gt_bone[i][0] / 255.0), getcolor(j, color_gt_bone[i][1] / 255.0), getcolor(j, color_gt_bone[i][2] / 255.0), 1.0f };
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

			glLineWidth(line_width_3d(directory) * (j == 1 ? 1.0 * ratio : 1.0));
			//glLineWidth(30);
			glBegin(GL_LINES);
			glVertex3f(spherex[j][bones[i][0]], spherey[j][bones[i][0]], spherez[j][bones[i][0]]);
			glVertex3f(spherex[j][bones[i][1]], spherey[j][bones[i][1]], spherez[j][bones[i][1]]);
			glEnd();
		}
	}

	glutSwapBuffers();

}

void LoadImageInfo(int id, int opt) {
	printf("Current Image Id %d\n", id);
#ifdef show_pred
	if (read_pred_separate) {
		char pred_file[maxlen];
		if (use_comp) sprintf(pred_file, "%s%d%s", pred_joint_3d_file, id + read_pred_start_index, "_comp.txt");
		else sprintf(pred_file, "%s%d%s", pred_joint_3d_file, id + read_pred_start_index, ".txt");
		FILE* fin_pred = fopen(pred_file, "r");
		cout << pred_file << "\n";
		for (int j = 0; j < JointNum; j++) {
			//fscanf(fin_pred, "%*s");
			for (int k = 0; k < 3; k++) {
				fscanf(fin_pred, "%lf", &all_pred_joint_3d[id][j * 3 + k]);
				cout << all_pred_joint_3d[id][j * 3 + k] << "    \n";

			}
		}


		double root_joint[3];
		for (int j = 0; j < 3; j++) root_joint[j] = all_pred_joint_3d[id][thorax * 3 + j];

		//take the negative value
		for (int j = 0; j < JointNum; j++) {
			all_pred_joint_3d[id][j * 3 + 0] = (all_pred_joint_3d[id][j * 3 + 0] - root_joint[0]) * mul_x_factor;
			all_pred_joint_3d[id][j * 3 + 1] = (all_pred_joint_3d[id][j * 3 + 1] - root_joint[1]) * mul_y_factor;
			all_pred_joint_3d[id][j * 3 + 2] = (all_pred_joint_3d[id][j * 3 + 2] - root_joint[2]) * mul_z_factor;
		}
		fclose(fin_pred);
	}
#endif
#ifdef show_gt
	if (read_gt_separate) {
		char gt_file[maxlen];
		sprintf(gt_file, "%s%d%s", gt_joint_3d_file, id + read_gt_start_index, ".txt");
		FILE* fin_gt = fopen(gt_file, "r");
		for (int j = 0; j < JointNum; j++) {
			//fscanf(fin_gt, "%*s");
			for (int k = 0; k < 3; k++) {
				fscanf(fin_gt, "%lf", &all_gt_joint_3d[id][j * 3 + k]);
			}
		}

		//take the negative value
		for (int j = 0; j < JointNum; j++) {
			all_gt_joint_3d[id][j * 3 + 0] = all_gt_joint_3d[id][j * 3 + 0] * mul_x_factor;
			all_gt_joint_3d[id][j * 3 + 1] = all_gt_joint_3d[id][j * 3 + 1] * mul_y_factor;
			all_gt_joint_3d[id][j * 3 + 2] = all_gt_joint_3d[id][j * 3 + 2] * mul_z_factor;
		}
		fclose(fin_gt);
	}
#endif
	for (int i = 0; i < JointNum; i++) {
#ifdef show_pred
		if (opt == 0)
		{
			spherex[0][i] = mul_factor(directory) * all_pred_joint_3d[id][i * 3]; spherey[0][i] = mul_factor(directory) * all_pred_joint_3d[id][i * 3 + 1]; spherez[0][i] = mul_factor(directory) * all_pred_joint_3d[id][i * 3 + 2];
		}
		else
			// in another viewpoint
		{
			spherex[0][i] = mul_factor(directory) * rot_pred_joint_3d[id][i * 3]; spherey[0][i] = mul_factor(directory) * rot_pred_joint_3d[id][i * 3 + 1]; spherez[0][i] = mul_factor(directory) * rot_pred_joint_3d[id][i * 3 + 2];
		}


		printf("Pred         sphere %4d %8.2f %8.2f %8.2f\n", i, spherex[0][i], spherey[0][i], spherez[0][i]);
		printf("\n");
#endif
#ifdef show_gt
		spherex[1][i] = mul_factor(directory) * all_gt_joint_3d[id][i * 3]; spherey[1][i] = mul_factor(directory) * all_gt_joint_3d[id][i * 3 + 1]; spherez[1][i] = mul_factor(directory) * all_gt_joint_3d[id][i * 3 + 2];
		printf("Ground Truth sphere %4d %8.2f %8.2f %8.2f\n", i, spherex[1][i], spherey[1][i], spherez[1][i]);
		printf("\n");
#endif

	}
	glutPostRedisplay();
}


int LoadOpenGLEngine(int argc, char** argv) {
	//OpenGL part
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(win_w, win_h);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("3D Human Model Visualization");
	// The callbacks.
	glutReshapeFunc(reshape_func);
	glutDisplayFunc(display_func);
	glutMouseFunc(mouse_click_func);
	glutMotionFunc(mouse_move_func);
	glutKeyboardFunc(keyboard_func);
	glutSpecialFunc(specialkey_func);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cout << "Error:" << glewGetErrorString(err);
		cin.get(); // hold the screen
		return -1;
	}

	transform_camera = glm::affineInverse(glm::lookAt(glm::vec3(10, 2, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
	transform_model = glm::translate(glm::mat4(1.0f), glm::vec3(5, 2, 5));
	position_light0 = glm::vec4(20, 20, 20, 1);

	ogl_init();


	int option;
	cin >> option;
	if (option == 0)
	{
		LoadImageInfo(current_image, 0);
	}
	else
	{
		RotateAroundY();
		LoadImageInfo(current_image, 1);
	}

	glutMainLoop();


	return 0;
}

void ReadConfiguration() {
	FILE* fin = fopen("conf.in", "r");
#ifdef show_pred
	fscanf(fin, "%s %d", pred_joint_3d_file, &read_pred_separate);
	if (read_pred_separate)
	{
		fscanf(fin, "%d", &read_pred_start_index);
		fscanf(fin, "%d", &use_comp);
	}
#endif
#ifdef show_gt
	fscanf(fin, "%s %d", gt_joint_3d_file, &read_gt_separate);
	if (read_gt_separate) fscanf(fin, "%d", &read_gt_start_index);
#endif
	fscanf(fin, "%d", &number_of_image);
	fscanf(fin, "%d", &start_index);
	fscanf(fin, "%lf", &mul_x_factor);
	fscanf(fin, "%lf", &mul_y_factor);
	fscanf(fin, "%lf", &mul_z_factor);
	int whe;
	fscanf(fin, "%d", &whe);
	whether_subtract_wrist = (bool)whe;
	fscanf(fin, "%lf", &global_rot_theta);
	fclose(fin);
	current_image = start_index;
}
#ifdef show_pred
void ReadPredJoint3dFile() {
	if (!read_pred_separate) {

		FILE* fin_pred = fopen(pred_joint_3d_file, "r");
		for (int i = 0; i < number_of_image; i++) {
			for (int j = 0; j < JointNum; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					fscanf(fin_pred, "%lf", &all_pred_joint_3d[i][j * 3 + k]);
				}
			}

			for (int j = 0; j < JointNum; j++)
			{
				//all_pred_joint_3d[i][j * 3 + 2] -= 3.0;
			}
		}
		fclose(fin_pred);
	}
}
#endif

#ifdef show_gt
void ReadGTJoint3dFile() {
	if (!read_gt_separate) {
		FILE* fin_gt = fopen(gt_joint_3d_file, "r");
		for (int i = 0; i < number_of_image; i++) {
			for (int j = 0; j < JointNum; j++) {
				for (int k = 0; k < 3; k++) {
					fscanf(fin_gt, "%lf", &all_gt_joint_3d[i][j * 3 + k]);
				}
			}
		}
		fclose(fin_gt);
	}
}
#endif


int main(int argc, char** argv)
{

	ReadConfiguration();
#ifdef show_pred
	ReadPredJoint3dFile();
#endif
#ifdef show_gt
	ReadGTJoint3dFile();
#endif

	//RotateAroundY();

	return LoadOpenGLEngine(argc, argv);
}

