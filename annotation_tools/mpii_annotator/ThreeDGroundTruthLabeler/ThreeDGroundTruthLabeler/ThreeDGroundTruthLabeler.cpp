#include "basic.h"
#include "gl_inc.h"
#include "cv_related.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "InitConfiguration.h"
#include "WeakPerspective.h"
#include "PinholeCamera.h"  //use standard pinhole camera
#include "DisplayJointOnImage.h"
#include "PoseConditionedJointAngleLimit_define.h"
using namespace std;
using namespace cv;
using namespace HumanModel;
InitConfiguration init_set;

double shape_convex_param[ParamNum];

double spherex[JointNum], spherey[JointNum], spherez[JointNum];
int current_image = 0;
int current_sphere = -1;
bool current_image_not_null = true;
bool has_been_optimized = false;
char directory[maxlen], imagedirectory[maxlen];

//pose-conditioned joint angle limit constraint
double sepplane[parent_num][angle_row_num][angle_col_num][4];
double basis_second[parent_num][angle_row_num][angle_col_num][3];
double bbx[parent_num][angle_row_num][angle_col_num][4];
double boundary_outline[bound_num_all][bound_dimension];
bool anglespread[parent_num][angle_row_num][angle_col_num];
char spread_name[maxlen], sepplane_name[maxlen], basissecond_name[maxlen], bbx_name[maxlen], boundaries_name[maxlen], testpose_name[maxlen], sign_name[maxlen], res_name[maxlen], closest_name[maxlen];
//End of PJCAL

//shape convex 3D Shape Estimation from 2D Landmarks: A Convex Relaxation Approach
double xy[2][15]; //the first row being u, the second row being v
double B[384][15];
double all_mu[3][15];
double intermediate[384][384];
double BBt[384][384];
double shape_convex_joint[JointNum * 3];
//End of shape convex

double fixedbonelen[BoneNum];
//Fixed Init Param
double fixedinitparam[ParamNum];
double fixed_pers_angle[3];
double fixed_pers_trans[2];
double fixed_pers_scale;

int fixedislimited[ParamNum];
int fixedinobj[ParamNum];
double fixedangle_lowerbound[ParamNum];
double fixedangle_upperbound[ParamNum];
double fixedpers_angle_lowerbound[3];
double fixedpers_trans_lowerbound[2];
double fixedpers_scale_lowerbound;
double fixedpers_angle_upperbound[3];
double fixedpers_trans_upperbound[2];
double fixedpers_scale_upperbound;

//temporary optimization result
double opt_location[JointNum * 3];
double opt_projection[JointNum * 2];
double opt_param[ParamNum];
double opt_angle[3];
double opt_trans[2];
double opt_scale;
double origin_projection[JointNum * 2];
double *closest_valid_joint_all = (double *)malloc(sizeof(double)* 3 * joint_num);


int memory_count=0;
double memory_location[max_memory][JointNum * 3];
double memory_projection[max_memory][JointNum * 2];
double memory_param[max_memory][ParamNum];
double memory_angle[max_memory][3];
double memory_trans[max_memory][2];
double memory_scale[max_memory];
Mat src_img;

int win_w = 1366, win_h = 768;
float win_fovy = 45;
float speed_scale = 0.5f;
glm::mat4 transform_camera(1.0f); 
glm::mat4 transform_model(1.0f);  
glm::vec4 position_light0(0);	  

void reshape_func(int w, int h);
void display_func();
void mouse_click_func(int button, int state, int x, int y);
void mouse_move_func(int x, int y);
void keyboard_func(unsigned char key, int x, int y);
void specialkey_func(int key, int x, int y);

void draw_frame(float len);
void draw_world(float len, float tex_repeat, bool yoz_plane, bool zox_plane, bool xoy_plane);
void ogl_init();
void save_memory();

//extern from ShapeConvex.cpp
extern void loadshapedict(char *directory);
void draw_sphere()
{
	GLfloat color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, color);
	glMaterialf(GL_FRONT, GL_SHININESS, 30.0);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glutSolidSphere(sphere_radius(directory), 30, 30);

}

void draw_local()
{
	glm::mat4 model_view_matrix = glm::affineInverse(transform_camera);
	model_view_matrix *= glm::translate(glm::vec3(spherex[current_sphere], spherey[current_sphere], spherez[current_sphere]));
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

void display_func()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glm::mat4 model_view_matrix = glm::affineInverse(transform_camera);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&model_view_matrix[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &position_light0[0]); // 位置式光源
	draw_world(30, 3, true, true, true); // 绘制世界

	//draw a sphere
	for (int i = 0; i < JointNum; i++)
	{
		model_view_matrix = glm::affineInverse(transform_camera);
		model_view_matrix *= glm::translate(glm::vec3(spherex[i], spherey[i], spherez[i]));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&model_view_matrix[0][0]);
		draw_sphere();
	}

	//draw local coordinate system
	draw_local();

	//restore matrix
	model_view_matrix = glm::affineInverse(transform_camera);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&model_view_matrix[0][0]);
	//draw edges between spheres

	for (int i = 0; i < EdgeNum; i++)
	{
		GLfloat color[] = { color_bone[i][0] / 255.0, color_bone[i][1] / 255.0, color_bone[i][2] / 255.0, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

		glLineWidth(line_width_3d(directory));
		glBegin(GL_LINES);
		glVertex3f(spherex[edges[i][0]], spherey[edges[i][0]], spherez[edges[i][0]]);
		glVertex3f(spherex[edges[i][1]], spherey[edges[i][1]], spherez[edges[i][1]]);
		glEnd();
	}	
	glutSwapBuffers();

}


void LoadImageInfo(int id) //be executed every time the current image is changed
{
	has_been_optimized = false;
	//write current image id to file
	char id_file[maxlen];
	sprintf(id_file, "%s%s", directory, "\\configuration\\current_imageid.in");
	FILE *fout_id;
	fout_id = fopen(id_file, "w");
	fprintf(fout_id, "%d\n", current_image);
	fclose(fout_id);
	printf("Current Image : %7d\n", current_image);		
	for (int i = 0; i < JointNum; i++)
	{
		spherex[i] = spherey[i] = spherez[i] = 0.0;
	}//clear the screen
	glutPostRedisplay();

	//------------------------------------------------------------------------------

	//Load 3d Joint Prediction
	char locationfile[maxlen];
	sprintf(locationfile, "%s%s%d%s", directory, "\\result\\pred_3d_", id, ".txt");
	FILE *fin_position = fopen(locationfile, "r");	
	if (fin_position != NULL) //has been optimized before now loading 
	{	
		has_been_optimized = true;
		for (int i = 0; i < JointNum; i++)
		{						
			fscanf(fin_position, "%*s %lf %lf %lf", &spherex[i], &spherey[i], &spherez[i]);
			opt_location[i * 3] = spherex[i];
			opt_location[i * 3 + 1] = spherey[i];
			opt_location[i * 3 + 2] = spherez[i];
			spherex[i] = spherex[i] * mul_factor(directory); spherey[i] = spherey[i] * mul_factor(directory); spherez[i] = spherez[i] * mul_factor(directory);			
		}
		glutPostRedisplay();		
		fclose(fin_position);
	}		

	//Load 2d joint Prediction
	char projectionfile[maxlen];
	sprintf(projectionfile, "%s%s%d%s", directory, "\\result\\pred_2d_", id, ".txt");
	FILE *fin_projection = fopen(projectionfile, "r");
	if (fin_projection != NULL) //has been optimized before now loading 
	{
		has_been_optimized = true;
		for (int i = 0; i < JointNum; i++)
		{
			fscanf(fin_projection, "%*s %lf %lf", &opt_projection[i],&opt_projection[i]);			
		}		
		fclose(fin_projection);
	}


	//Load Parameters
	char paramfile[maxlen];
	bool hasparam = false;
	sprintf(paramfile, "%s%s%d%s", directory, "\\result\\param_", id, ".txt");
	FILE *fin_param = fopen(paramfile, "r");
	if (fin_param != NULL)//has been optimized before
	{
		has_been_optimized = true;
		hasparam = true;
		for (int i = 0; i < ParamNum; i++) fscanf(fin_param, "%*s %lf", &opt_param[i]);
		for (int i = 3; i < ParamNum; i++) opt_param[i] = deg2rad(opt_param[i]);
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++)
		{
			fscanf(fin_param, "%*s %lf", &opt_angle[i]);
			opt_angle[i] = deg2rad(opt_angle[i]);
		}		
		fscanf(fin_param, "%*s %lf", &opt_trans[0]);
		fscanf(fin_param, "%*s %lf", &opt_trans[1]);
		fscanf(fin_param, "%*s %lf", &opt_scale);
#endif
		fclose(fin_param);
	}
	
	//Load configuration file of the current image
	char configfile[maxlen];
	sprintf(configfile, "%s%s%d%s", directory, "\\origin_data\\config_", id, ".txt");
	FILE *fin_config = fopen(configfile, "r");
	if (fin_config != NULL)
	{		
		init_set.ReadImageInfo(configfile);
		fclose(fin_config);
	}
	else
	{
		current_image_not_null = false; //no config file
	}
	//Load specific bone length setting for the current image
	char bonelenfile[maxlen];
	sprintf(bonelenfile, "%s%s%d%s", directory, "\\configuration\\bonelen_", id, "in"); //in case one image has specific bone length setting
	FILE *fin_bonelen = fopen(bonelenfile, "r");
	if (fin_bonelen != NULL)
	{
		double bonelen[BoneNum];
		for (int i = 0; i < BoneNum; i++)  fscanf(fin_bonelen, "%*s %lf", &bonelen[i]);
		init_set.SetBoneLen(bonelen);
		fclose(fin_bonelen);
	}
	else
	{
		init_set.SetBoneLen(fixedbonelen); //constant 
	}
	//Load 2d projection annotation
	char projgtfile[maxlen];
	sprintf(projgtfile, "%s%s%d%s", directory, "\\gt_2d_modified\\mod_", id, ".txt");
	FILE *fin_proj = fopen(projgtfile, "r");
	if (fin_proj != NULL) //if the ground truth 2d annotation has been modified
	{
		init_set.ReadJointGT(projgtfile);
		for (int i = 0; i < JointNum * 2; i++) origin_projection[i] = init_set.jointgt[i]; //save joint gt from reading file
		fclose(fin_proj);
	}
	else //has never been modified
	{
		sprintf(projgtfile, "%s%s%d%s", directory, "\\origin_data\\gt_2d_", id, ".txt");
		FILE *fin_proj = fopen(projgtfile, "r");
		if (fin_proj != NULL)
		{
			fclose(fin_proj);
			init_set.ReadJointGT(projgtfile);
			for (int i = 0; i < JointNum * 2; i++) origin_projection[i] = init_set.jointgt[i]; //save joint gt from reading file }
		}
		else current_image_not_null = false; //missing gt		
	}
	//Load mark for visibility
	char visfile[maxlen];
	sprintf(visfile, "%s%s%d%s", directory, "\\origin_data\\vis_", id, ".txt");
	FILE *fin_vis = fopen(visfile,"r");
	if (fin_vis != NULL) { fclose(fin_vis); init_set.ReadVisibility(visfile); }
	else current_image_not_null = false; //no visibility file
	//Load Initparam
	char initparamfile[maxlen];
	sprintf(initparamfile, "%s%s%d%s", directory, "\\configuration\\initparam_", id, ".in");
	FILE *fin_initparam = fopen(initparamfile, "r");
	bool has_init_param = true;
	if (fin_initparam != NULL)   //has initial param (not the standard fixed initparam)
	{
		has_init_param = true;
		double initparam[ParamNum];
		for (int i = 0; i < ParamNum; i++) fscanf(fin_initparam, "%*s %lf", &initparam[i]);
		for (int i = 3; i < ParamNum; i++) initparam[i] = deg2rad(initparam[i]);
		init_set.SetInitParam(initparam);
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++)
		{
			fscanf(fin_initparam, "%*s %lf", &init_set.obj.init_pers_angle[i]);
			init_set.obj.init_pers_angle[i] = deg2rad(init_set.obj.init_pers_angle[i]);
		}
		for (int i = 0; i < 2; i++) fscanf(fin_initparam, "%*s %lf", &init_set.obj.init_pers_trans[i]);
		fscanf(fin_initparam, "%*s %lf", &init_set.obj.init_pers_scale);				
#endif
		fclose(fin_initparam);		
	}
	else
	{
		has_init_param = false;
		init_set.SetInitParam(fixedinitparam);
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++) init_set.obj.init_pers_angle[i] = fixed_pers_angle[i];
		for (int i = 0; i < 2; i++) init_set.obj.init_pers_trans[i] = fixed_pers_trans[i];
		init_set.obj.init_pers_scale = fixed_pers_scale;
#endif
	}
	//Load islimited
	char islimitedfile[maxlen];
	sprintf(islimitedfile, "%s%s%d%s", directory, "\\configuration\\islimited_", id, ".in");
	FILE *fin_islimited = fopen(islimitedfile, "r");
	if (fin_islimited != NULL)
	{
		int islimited[ParamNum];
		for (int i = 0; i < ParamNum; i++) fscanf(fin_islimited, "%*s %d", &islimited[i]);
		init_set.SetLimitDoF(islimited);		
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++) fscanf(fin_islimited, "%*s %d", &init_set.obj.limit_pers_angle[i]);
		for (int i = 0; i < 2; i++) fscanf(fin_islimited, "%*s %d", &init_set.obj.limit_pers_trans[i]);
		fscanf(fin_islimited, "%*s %d", &init_set.obj.limit_pers_scale);
#endif
		fclose(fin_islimited);
	}
	else
	{
		init_set.SetLimitDoF(fixedislimited);
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++) init_set.obj.limit_pers_angle[i] = 0;
		for (int i = 0; i < 2; i++) init_set.obj.limit_pers_trans[i] = 0;
		init_set.obj.limit_pers_scale = 0;
#endif
	}
	//Load inobj
	char inobjfile[maxlen];
	sprintf(inobjfile, "%s%s%d%s", directory, "\\configuration\\inobj_", id, ".in");
	FILE *fin_inobj = fopen(inobjfile, "r");
	if (fin_inobj != NULL)
	{
		int inobj[JointNum];
		for (int i = 0; i < JointNum; i++) fscanf(fin_inobj, "%*s %d", &inobj[i]);
		init_set.SetInObj(inobj);
		fclose(fin_inobj);
	}
	else
	{
		init_set.SetInObj(fixedinobj);
	}
	//Load Angle Constraint
	char angleconstraintfile[maxlen];
	sprintf(angleconstraintfile, "%s%s%d%s", directory, "\\configuration\\angleconstraint_", id, ".in");
	FILE *fin_angleconstraint = fopen(angleconstraintfile, "r");
	if (fin_angleconstraint != NULL)
	{
		for (int i = 0; i < ParamNum; i++)
		{
			fscanf(fin_angleconstraint, "%*s %lf %lf", &init_set.obj.local_lb[i], &init_set.obj.local_ub[i]);
			if (i>=3)
			{
				init_set.obj.local_lb[i] = deg2rad(init_set.obj.local_lb[i]);
				init_set.obj.local_ub[i] = deg2rad(init_set.obj.local_ub[i]);
			}
		}
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++)
		{
			fscanf(fin_angleconstraint, "%*s %lf %lf", &init_set.obj.pers_global_rot_lb[i], &init_set.obj.pers_global_rot_ub[i]);
			init_set.obj.pers_global_rot_lb[i] = deg2rad(init_set.obj.pers_global_rot_lb[i]);
			init_set.obj.pers_global_rot_ub[i] = deg2rad(init_set.obj.pers_global_rot_ub[i]);
		}
		for (int i = 0; i < 2; i++) fscanf(fin_angleconstraint, "%*s %lf %lf", &init_set.obj.pers_global_trans_lb[i], &init_set.obj.pers_global_trans_ub[i]);
		fscanf(fin_angleconstraint, "%*s %lf %lf", &init_set.obj.pers_scale_lb,&init_set.obj.pers_scale_ub);
#endif
		fclose(fin_angleconstraint);
	}
	else
	{
		for (int i = 0; i < ParamNum; i++) { init_set.obj.local_lb[i] = fixedangle_lowerbound[i]; init_set.obj.local_ub[i] = fixedangle_upperbound[i]; }
#ifdef use_weak_perspective
		for (int i = 0; i < 3; i++) { init_set.obj.pers_global_rot_lb[i] = fixedpers_angle_lowerbound[i]; init_set.obj.pers_global_rot_ub[i] = fixedpers_angle_upperbound[i]; }
		for (int i = 0; i < 2; i++) { init_set.obj.pers_global_trans_lb[i] = fixedpers_trans_lowerbound[i]; init_set.obj.pers_global_trans_ub[i] = fixedpers_trans_upperbound[i]; }
		init_set.obj.pers_scale_lb = fixedpers_scale_lowerbound;
		init_set.obj.pers_scale_ub = fixedpers_scale_upperbound;
#endif
	}
	//Load Image
	char imagename[maxlen];
	sprintf(imagename, "%s%s%d%s", imagedirectory, "\\",id, ".png");
	init_set.ReadImageContent(imagename);
	src_img = imread(imagename);
#ifdef use_weak_perspective
	WeakPerspective load_projector;
#else
	PinholeCamera load_projector;
#endif
	DisplayJoint load_disp;
	Mat mid_img = Mat::zeros(Size(224, 224), CV_8UC3);
	src_img.copyTo(mid_img);
	if (has_init_param)
	{
		init_set.obj.Setup_ConstantMatrices();
		init_set.obj.Setup_Transformation();
		double t_param[ParamNum];
		for (int i = 0; i < ParamNum; i++) t_param[i] = 0.0;
		init_set.obj.Forward_cpu(t_param);		
#ifdef use_weak_perspective
		double load_m_rot[6];
		for (int i = 0; i < 6; i++) load_m_rot[i] = 0.0;
		load_projector.Forward_cpu(init_set.obj.top_data, load_m_rot, init_set.obj.init_pers_trans, init_set.obj.init_pers_scale, init_set.obj.init_pers_angle);
		load_disp.Display(mid_img, load_projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj), 1, 1, 1, 1, init_set.visible, init_set.obj.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
#else
		load_projector.Forward_cpu(init_set.obj.top_data);
		load_disp.Display(mid_img, load_projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj), 1, 1, 1, 1, init_set.visible, init_set.obj.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
#endif
		for (int i = 0; i < ParamNum; i++) opt_param[i] = 0.0; 
	}
	else //show origin image
	{
		if (hasparam)    //has init has param (just not set param as new initparam)
		{
			init_set.obj.Setup_ConstantMatrices();
			init_set.obj.Setup_Transformation();
			init_set.obj.Forward_cpu(opt_param);
#ifdef use_weak_perspective
			double load_m_rot[6];
			for (int i = 0; i < 6; i++) load_m_rot[i] = 0.0;
			load_projector.Forward_cpu(init_set.obj.top_data, load_m_rot, opt_trans, opt_scale, opt_angle);
			load_disp.Display(mid_img, load_projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj), 1, 1, 1, 1, init_set.visible, init_set.obj.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
#else
			load_projector.Forward_cpu(init_set.obj.top_data);
			load_disp.Display(mid_img, load_projector.top_data, normalize_gt_2d(init_set.jointgt, init_set.obj), 1, 1, 1, 1, init_set.visible, init_set.obj.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
#endif
		}
		else
		{
			Mat dst_img;
			resize(src_img, dst_img, Size(show_image_size(directory), show_image_size(directory)));
			imshow("show_image", dst_img);
			waitKey(1);
		}
	}
	if (has_been_optimized) save_memory();
}

void InitFile()
{
	FILE *fin_path = fopen("FilePath.in", "r");
	fscanf(fin_path, "%s", directory);
	fscanf(fin_path, "%s", imagedirectory);
	fscanf(fin_path, "%s", spread_name);
	fscanf(fin_path, "%s", sepplane_name);
	fscanf(fin_path, "%s", basissecond_name);
	fscanf(fin_path, "%s", bbx_name);
	fscanf(fin_path, "%s", boundaries_name);	
	fclose(fin_path);
	loadshapedict(directory);
}

//pose conditioned joint angle limit start

void ReadAngleSpreadMat()
{
	FILE *fin = fopen(spread_name, "r");
	for (int i = 0; i < parent_num; i++)
	{
		for (int j = 0; j < angle_row_num; j++)
		{
			for (int k = 0; k < angle_col_num; k++)
			{
				anglespread[i][j][k] = false;
			}
		}
	}
	for (int i = 0; i < spread_num; i++)
	{
		for (int j = 0; j < angle_row_num; j++)
		{
			for (int k = 0; k < angle_col_num; k++)
			{
				int x;
				fscanf(fin, "%d", &x);
				anglespread[spread_id[i]][j][k] = (bool)x;
			}
		}
	}
	fclose(fin);
}

void ReadSepPlane()
{
	FILE *fin = fopen(sepplane_name, "r");
	for (int i = 0; i < parent_num; i++)
	{
		for (int j = 0; j < angle_row_num; j++)
		{
			for (int k = 0; k < angle_col_num; k++)
			{
				for (int l = 0; l < 4; l++)
					sepplane[i][j][k][l] = 0.0;
			}
		}
	}
	for (int i = 0; i < sep_num; i++)
	{
		for (int j = 0; j < angle_row_num; j++)
		{
			for (int k = 0; k < angle_col_num; k++)
			{
				for (int l = 0; l < 4; l++)
					fscanf(fin, "%lf", &sepplane[sep_id[i]][j][k][l]);
			}
		}
	}
	fclose(fin);
}

void ReadSecondBasis()
{
	FILE *fin = fopen(basissecond_name, "r");
	for (int i = 0; i < parent_num; i++)
	{
		for (int j = 0; j < angle_row_num; j++)
		{
			for (int k = 0; k < angle_col_num; k++)
			{
				for (int l = 0; l < 3; l++)
					basis_second[i][j][k][l] = 0.0;
			}
		}
	}
	for (int i = 0; i < sep_num; i++)
	{
		for (int j = 0; j < angle_row_num; j++)
		{
			for (int k = 0; k < angle_col_num; k++)
			{
				for (int l = 0; l < 3; l++)
					fscanf(fin, "%lf", &basis_second[sep_id[i]][j][k][l]);
			}
		}
	}
	fclose(fin);
}

void ReadDatasetProjectionBoundingbox()
{
	FILE *fin = fopen(bbx_name, "r");
	for (int i = 0; i < parent_num; i++)
	{
		for (int j = 0; j < angle_row_num; j++)
		{
			for (int k = 0; k < angle_col_num; k++)
			{
				for (int l = 0; l < 4; l++)
					bbx[i][j][k][l] = 0.0;
			}
		}
	}
	for (int i = 0; i < sep_num; i++)
	{
		for (int j = 0; j < angle_row_num; j++)
		{
			for (int k = 0; k < angle_col_num; k++)
			{
				for (int l = 0; l < 4; l++)
					fscanf(fin, "%lf", &bbx[sep_id[i]][j][k][l]);
			}
		}
	}
	fclose(fin);
}

void ReadDatasetProjectionBoundaryPoints()
{
	FILE *fin = fopen(boundaries_name, "r");
	for (int i = 0; i < bound_num_all; i++)
	{
		for (int j = 0; j < bound_dimension; j++)
			boundary_outline[i][j] = 0.0;
	}
	for (int i = 0; i < bound_num_valid; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < bound_point_num[bound_point_id[i]]; k++)
			{
				fscanf(fin, "%lf", &boundary_outline[bound_point_id[i]][k * 2 + j]);
				boundary_outline[bound_point_id[i]][k * 2 + j]--; //because matlab index starts from 1 while c++ starts from 0
			}
		}
	}
	fclose(fin);
}
//pose conditioned joint angle limit end

void LoadFixedSetting()
{
	//bone length
	char bonefile[maxlen];
	sprintf(bonefile, "%s%s", directory, "\\BoneLength.in");
	FILE *fin_bone = fopen(bonefile, "r");
	for (int i = 0; i < BoneNum; i++) fscanf(fin_bone, "%*s %lf", &fixedbonelen[i]);
	fclose(fin_bone);
	//init param
	char initparamfile[maxlen];
	sprintf(initparamfile, "%s%s", directory, "\\InitParam.in");
	FILE *fin_initparam = fopen(initparamfile, "r");
	for (int i = 0; i < ParamNum; i++) fscanf(fin_initparam, "%*s %lf", &fixedinitparam[i]);
	for (int i = 3; i < ParamNum; i++) fixedinitparam[i] = deg2rad(fixedinitparam[i]);
	for (int i = 0; i < 3; i++)
	{
		fscanf(fin_initparam, "%*s %lf", &fixed_pers_angle[i]);
		fixed_pers_angle[i] = deg2rad(fixed_pers_angle[i]); 
	}
	for (int i = 0; i < 2; i++) fscanf(fin_initparam, "%*s %lf", &fixed_pers_trans[i]);
	fscanf(fin_initparam, "%*s %lf", &fixed_pers_scale);	
	fclose(fin_initparam);
	//limited
	for (int i = 0; i < ParamNum; i++) fixedislimited[i] = 0; //all are not limited
	//inobj
	for (int i = 0; i < JointNum; i++) fixedinobj[i] = 1; //all are in objective function
	//angle constraint
	char angle_constraintfile[maxlen];
	sprintf(angle_constraintfile, "%s%s", directory, "\\angle_constraint.in");
	FILE *fin_angleconstraint = fopen(angle_constraintfile, "r");
	for (int i = 0; i < ParamNum; i++)
	{
		fscanf(fin_angleconstraint, "%*s %lf %lf", &fixedangle_lowerbound[i], &fixedangle_upperbound[i]);
		if (i >= 3)
		{
			fixedangle_lowerbound[i] = deg2rad(fixedangle_lowerbound[i]);
			fixedangle_upperbound[i] = deg2rad(fixedangle_upperbound[i]);
		}		
	}
	for (int i = 0; i < 3; i++)
	{
		fscanf(fin_angleconstraint, "%*s %lf %lf", &fixedpers_angle_lowerbound[i], &fixedpers_angle_upperbound[i]);
		fixedpers_angle_lowerbound[i] = deg2rad(fixedpers_angle_lowerbound[i]);
		fixedpers_angle_upperbound[i] = deg2rad(fixedpers_angle_upperbound[i]);
	}
	for (int i = 0; i < 2; i++) fscanf(fin_angleconstraint, "%*s %lf %lf", &fixedpers_trans_lowerbound[i], &fixedpers_trans_upperbound[i]);
	fscanf(fin_angleconstraint, "%*s %lf %lf", &fixedpers_scale_lowerbound, &fixedpers_scale_upperbound);
	fclose(fin_angleconstraint);
}

void LoadCurrentImageId()
{
	char id_file[maxlen];
	sprintf(id_file, "%s%s", directory, "\\configuration\\current_imageid.in");
	FILE *fin_id;
	fin_id = fopen(id_file, "r");
	if (fin_id != NULL)
	{
		int id;
		fscanf(fin_id, "%d", &id);
		current_image = id;
		fclose(fin_id);
	}
	else current_image = 0;
}

void LoadPoseCondtionedJointAngleLimit()
{
	ReadAngleSpreadMat();
	ReadSepPlane();
	ReadSecondBasis();
	ReadDatasetProjectionBoundingbox();
	ReadDatasetProjectionBoundaryPoints();
}


int LoadOpenGLEngine(int argc, char** argv)
{
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
	transform_model = glm::translate(glm::vec3(5, 2, 5));
	position_light0 = glm::vec4(20, 20, 20, 1);

	ogl_init();
	LoadImageInfo(current_image);
	glutMainLoop();
	return 0;
}

int main(int argc, char** argv)
{
	InitFile();
	LoadPoseCondtionedJointAngleLimit();
	LoadFixedSetting();	
	LoadCurrentImageId();	
	return LoadOpenGLEngine(argc,argv);
}
