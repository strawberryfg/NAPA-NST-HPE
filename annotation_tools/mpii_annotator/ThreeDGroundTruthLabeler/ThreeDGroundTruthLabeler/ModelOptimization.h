#pragma once
#include "HumanModel.h"
#include "InitConfiguration.h"
#include "GradientDescent_2d.h"
#include "GradientDescent_2d_pinhole.h"
#include "GradientDescent_3d.h"
#include "GradientDescent_3d_pinhole.h"
using namespace HumanModel;
void Optimize_2d(InitConfiguration work, bool whether_to_add_angle_constraint);
void Optimize_2d_pinhole(InitConfiguration work, bool whether_to_add_angle_constraint, bool is_shapeconvex);
void Optimize_3d(InitConfiguration work, double *joint_3d, bool whether_to_add_angle_constraint);
void Optimize_3d_pinhole(InitConfiguration work, double *joint_3d, bool whether_to_add_angle_constraint);
bool opt_local_model_param = true;

//from main.cpp (working directory)
extern char directory[maxlen];
extern double opt_location[JointNum * 3];
extern double opt_projection[JointNum * 2];
extern double opt_param[ParamNum];
extern double opt_angle[3];
extern double opt_trans[2];
extern double opt_scale;
extern double shape_convex_param[ParamNum];

