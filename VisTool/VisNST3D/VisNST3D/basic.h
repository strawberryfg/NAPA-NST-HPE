#pragma once
#include "HumanModel_define.h"
//#include "eigen/dense"
//#include "InitConfiguration.h"
#define Global
#define PI 3.1415926
#define epss 1e-6 // c++ precision is that accurate enough (unlike PASCAL LOL)
#define Display_Edge_In_Image
#define use_pinhole_camera
//#define use_weak_perspective
#define incorporate_shape_convex //whether to directly incorporate the shape convex code in this project

enum basic
{
	EdgeNum = 15, TrainingSize = 28882, max_memory = 10000
	//25924 + 2958
};

//display settings
// + b1) * mul1 + b2) * mul2 + b3
const double b1[2] = { 0, 0 };
const double mul1[2] = { 224, 224 };
const double b2[2] = { 0.0, 0.0 };
const double mul2[2] = { 1.0, 1.0 };
const double b3[2] = { 0.0, 0.0 };

const int edges[EdgeNum][2] = { { thorax, right_shoulder }, { right_shoulder, right_elbow }, { right_elbow, right_wrist }, { thorax, pelvis },
{ pelvis, right_hip }, { right_hip, right_knee }, { right_knee, right_ankle }, { thorax, left_shoulder }, { left_shoulder, left_elbow }, { left_elbow, left_wrist },
{ pelvis, left_hip }, { left_hip, left_knee }, { left_knee, left_ankle }, { thorax, upper_neck }, { upper_neck, head_top } };

const int color_bone[EdgeNum][3] = {
	{ 255, 255, 0 },   //thorax         -> right_shoulder
	{ 0, 255, 0 },     //right_shoulder -> right_elbow
	{ 0, 255, 0 },     //right_elbow    -> right_wrist
	{ 255, 255, 0 },   //thorax         -> pelvis
	{ 255, 255, 0 },   //pelvis         -> right_hip
	{ 0, 0, 255 },     //right_hip      -> right_knee
	{ 0, 0, 255 },     //right_knee     -> right_ankle
	{ 255, 255, 0 },   //thorax         -> left_shoulder
	{ 255, 0, 0 },     //left_shoulder  -> left_elbow
	{ 255, 0, 0 },     //left_elbow     -> left_wrist
	{ 255, 255, 0 },   //pelvis         -> left_hip
	{ 255, 127, 39 },  //left_hip       -> left_knee
	{ 255, 127, 39 },  //left_knee      -> left_ankle
	{ 63, 72, 204 },   //thorax         -> upper_neck
	{ 63, 72, 204 } }  //upper_neck     -> head_top
;


//extern from main function
extern bool current_image_not_null; //if null then optimization will not proceed
extern double opt_location[JointNum * 3];
extern double opt_projection[JointNum * 2];
extern double opt_param[ParamNum];
extern double opt_angle[3];
extern double opt_trans[2];
extern char directory[maxlen];

const char joint_name[JointNum][maxlen] = { "right_ankle", " right_knee", " right_hip", " left_hip", " left_knee", " left_ankle", " pelvis", " thorax", "upper_neck", " head_top", "right_wrist", " right_elbow", " right_shoulder", " left_shoulder", "left_elbow", " left_wrist" };
const char param_name[ParamNum][maxlen] =
{ "global_trans_x", "global_trans_y", "global_trans_z", "global_rot_x", "global_rot_y", "global_rot_z",
"right_knee_rot_x", "right_hip_rot_x", "right_hip_rot_y", "right_hip_rot_z", "left_hip_rot_x", "left_hip_rot_y", "left_hip_rot_z", "left_knee_rot_x",
"upper_neck_rot_x", "upper_neck_rot_y", "upper_neck_rot_z", "head_top_rot_x",
"right_elbow_rot_x", "right_shoulder_rot_x", "right_shoulder_rot_y", "right_shoulder_rot_z", "left_shoulder_rot_x", "left_shoulder_rot_y", "left_shoulder_rot_z", "left_elbow_rot_x", "hip_rot_y", "hip_rot_z" };
const char pers_param_name[6][maxlen] =
{
	"perspective_rot_x", "perspective_rot_y", "perspective_rot_z", "perspective_trans_x", "perspective_trans_y", "perspective_scale"
};
const char part_name[PartNum][maxlen] = { "hip", "shoulder", "elbow", "wrist", "knee", "ankle", "head" };
const char bone_name[BoneNum][maxlen] =
{
	"bone_knee_connect_ankle",
	"bone_hip_connect_knee",
	"bone_pelvis_connect_hip",
	"bone_thorax_connect_pelvis",
	"bone_thorax_connect_upper_neck",
	"bone_upper_neck_connect_head_top",
	"bone_elbow_connect_wrist",
	"bone_shoulder_connect_elbow",
	"bone_thorax_connect_shoulder"
};

double deg2rad(double deg);
double rad2deg(double rad);
double *concat_spherelocation(double *spherex, double *spherey, double *spherez);
bool has_difference(double *comp_0, double *comp_1, int arr_len);
int sgn(double x);
bool isnan(double x);
void copy(double *a, double *b);
void addition(double *a, double *b);
void subtract(double *a, double *b);
void divide_single_num(double *a, double b);
double *mul_single_num(double *a, double b);
double *calc_add(double *a, double *b);
double *calc_sub(double *a, double *b);
double *calc_mat3x3Xvec3(double *a, double *b); //3*3 matrix' multiply 3-d vec
double *calc_mat3x3transposeXvec3(double *a, double *b); //3*3 matrix's transpose multiply 3-d vec
double calc_vec4Xvec4(double *a, double *b);
double *calc_mat3x3last2coltransposeXvec3(double *a, double *b); //a is 3*3 but we calculate a(:,2:3)' * b
double *calc_mat3x3last2colXvec2(double *a, double *b); //a(:,2:3) * b
double norm(double *a);
double *cross(double *a, double *b);
void normalize(double *a);
int index(int x);

//hyper parameters
double shift_ratio(char *directory);
double show_image_size(char *directory);
double sphere_radius(char *directory);
double circle_radius(char *directory);
double line_width_2d(char *directory);
double line_width_3d(char *directory);
double mul_factor(char *directory);
bool fig_edges(char *directory);
int GetBackupId(char *directory);