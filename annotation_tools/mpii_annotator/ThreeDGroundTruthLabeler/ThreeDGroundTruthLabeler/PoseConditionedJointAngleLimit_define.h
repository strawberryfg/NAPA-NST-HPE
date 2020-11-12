#pragma once
#include <cmath>
#include "basic.h"
enum human_skeleton_pjcal
{
	belly = 0, neck = 1, r_shoulder = 2, r_elbow = 3, r_wrist = 4, l_shoulder = 5, l_elbow = 6, l_wrist = 7,
	face = 8, r_hip = 9, r_knee = 10, r_ankle = 11, r_foot = 12, l_hip = 13, l_knee = 14, l_ankle = 15, l_foot = 16
};
const char joint_name_pjcal[17][111] =
{
	"belly", "neck", "r_shoulder", "r_elbow", "r_wrist", "l_shoulder", "l_elbow", "l_wrist",
	"face", "r_hip", "r_knee", "r_ankle", "r_foot", "l_hip", "l_knee", "l_ankle", "l_foot"
};

const int convert_std2pjcai[16] = { 11, 10, 9, 13, 14, 15, 0, 1, 8, 8, 4, 3, 2, 5, 6, 7 };

enum basic_var_pjcal
{	
	parent_num = 11, bone_num = 16, torso_bone_num = 5, joint_num = 17,
	occupancy_matrix_jmp = 3, angle_row_num = 121, angle_col_num = 61, spread_num = 7, //angle spread matrix
	sep_num = 6, //separate plane 
	bound_num_all = 10,
	bound_num_valid = 7, bound_dimension = 800
};
const int children_bone[parent_num] = { 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15 };
//index of edges array starting from 0  expect torso children_bone bones of torso
const int parent_bone[parent_num] = { 1, 2, 4, 5, 1, 8, 9, 10, 12, 13, 14 };
//index of parent bone starting from 0 (in edges array)
//for each i   edges[parent_bone[i]] ---> edges[children_bone[i]]   describe the bone connection
const int torso_bone[torso_bone_num] = { 2, 5, 7, 9, 13 }; //also the index of edges
const int torso[torso_bone_num] = { 0, 1, 4, 8, 12 };
const int edges_pjcal[bone_num][2] =
{
	{ belly, neck },         //0
	{ neck, r_shoulder },    //1
	{ r_shoulder, r_elbow }, //2
	{ r_elbow, r_wrist },    //3
	{ neck, l_shoulder },    //4
	{ l_shoulder, l_elbow }, //5
	{ l_elbow, l_wrist },    //6
	{ neck, face },          //7
	{ belly, r_hip },        //8
	{ r_hip, r_knee },       //9
	{ r_knee, r_ankle },     //10
	{ r_ankle, r_foot },     //11
	{ belly, l_hip },        //12
	{ l_hip, l_knee },       //13
	{ l_knee, l_ankle },     //14
	{ l_ankle, l_foot } };   //15
const char edge_pjcal_name[bone_num][maxlen] =
{
	"pelvis           --->   thorax          ",
	"thorax           --->   right_shoulder  ",
	"right_shoulder   --->   right_elbow     ",
	"right_elbow      --->   right_wrist     ",
	"thorax           --->   left_shoulder   ",
	"left_shoulder    --->   left_elbow      ",
	"left_elbow       --->   left_wrist      ",
	"thorax           --->   upper_neck      ",
	"pelvis           --->   right_hip       ",
	"right_hip        --->   right_knee      ",
	"right_knee       --->   right_ankle     ",
	"right_ankle      --->   right_foot      ",
	"pelvis           --->   left_hip        ",
	"left_hip         --->   left_knee       ",
	"left_knee        --->   left_ankle      ",
	"left_ankle       --->   left_foot       "
};
const double static_a[3] = { 0.997478132770155, 0.00232349033665470, 0.0709364225065005 };
//it's a arbitary static vector
const double static_Joint[joint_num * 3] =  //static pose 17 * 3
{
	0, 0, 0,
	0.530366620433467, 4.06060893504002, 0.110251419567426,
	0.443987827415959, 4.98949324150556, 3.84369070075560,
	0.456865660019621, 0.0176933542205511, 3.82545708563889,
	-0.761902689130960, -3.11141687307495, 3.83871109044724,
	0.976628538166444, 4.47293208547881, -3.62915918385573,
	1.14450026664547, -0.487004298248760, -3.77537839276139,
	-0.0901861570398417, -3.55716127668003, -4.29251383639000,
	0.571694677535282, 7.50448397979653, 0.586390520222050,
	-1.13193191411242, -1.53044836223887, 1.39514500552242,
	-0.917102464370649, -8.47183228322044, 2.01541653345368,
	1.18295726040334, -15.0137463804434, 2.95206435448198,
	-0.827183348021091, -15.6709089856335, 3.50541441195271,
	-0.950657360198185, -1.71021687514648, -1.31973773181085,
	-0.265754898673697, -8.36540635768323, -1.85790700167853,
	1.26606049564817, -15.2250766005915, -2.35134775676245,
	-0.475217540642532, -15.8007194223158, 0
};
const double d_static_Joint[bone_num * 3] = //bone_num = joint_num - 1 
//delta vector of 16 bones in the static pose difference pose vector
{
	-0.530366620433467, -4.06060893504002, -0.110251419567426,
	0.0863787930175084, -0.928884306465545, -3.73343928118817,
	-0.0128778326036620, 4.97179988728501, 0.0182336151167091,
	1.21876834915058, 3.12911022729550, -0.0132540048083536,
	-0.446261917732977, -0.412323150438791, 3.73941060342315,
	-0.167871728479023, 4.95993638372757, 0.146219208905663,
	1.23468642368531, 3.07015697843127, 0.517135443628612,
	-0.0413280571018149, -3.44387504475651, -0.476139100654624,
	1.13193191411242, 1.53044836223887, -1.39514500552242,
	-0.214829449741771, 6.94138392098157, -0.620271527931256,
	-2.10005972477399, 6.54191409722301, -0.936647821028299,
	2.01014060842444, 0.657162605190054, -0.553350057470735,
	0.950657360198185, 1.71021687514648, 1.31973773181085,
	-0.684902461524488, 6.65518948253675, 0.538169269867683,
	-1.53181539432187, 6.85967024290824, 0.493440755083916,
	1.74127803629071, 0.575642821724355, 0.889887596457444
};
const int spread_id[spread_num] = { 0, 2, 4, 5, 6, 8, 9 };

const int sep_id[sep_num] = { 1, 3, 6, 7, 9, 10 };

const int bound_point_num[bound_num_all] = { 409, 0, 344, 0, 117, 352, 173, 0, 307, 172 };

const int bound_point_id[bound_num_valid] = { 0, 2, 4, 5, 6, 8, 9 };

bool find_in_torso(int id);


double *getNormal(double *x1, double *a, double *x); //calculate Ru¡Áa,u
double *gramschimidt(double *u, double *v, double *w);  //three 3-d vector
double *cart2sph(double *x, double *y, double *z);
double *sph2cart(double azimuth, double elevation, double radius);
double *estimateJoint(double *delta, double *base);