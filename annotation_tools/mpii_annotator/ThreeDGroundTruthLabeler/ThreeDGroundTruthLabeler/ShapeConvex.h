#pragma once
#include <iostream>
#include "basic.h"
#include "GradientDescent_3d_pinhole.h"
#define dividestd
#define convex 0
#define altern 1
#define inf 1e30
#define need_normalize
using namespace HumanModel;
using namespace Eigen;


//extern from main.cpp
extern double xy[2][15]; //the first row being u, the second row being v
extern double B[384][15];
extern double all_mu[3][15];
extern double intermediate[384][384];
extern double BBt[384][384];
extern double shape_convex_joint[JointNum * 3];
extern double shape_convex_param[ParamNum];

void loadshapedict(char *directory);
void normalize_xy();
void centralize_basis();
MatrixXd prox_2norm(MatrixXd Z, double lam, double *normX);
void cross_product(double *a, double *b, double *c);
void ssr2D3D_alm(double lam, double beta, int id);
void ssr2D3D_wrapper(int method, int id);
double calc_bone_len(double *endpoint_a, double *endpoint_b);
void prepare_2d_landmark(InitConfiguration t_init, int id); //prepare 2d gt landmark for shape convex code
void FitModelToShapeConvex();