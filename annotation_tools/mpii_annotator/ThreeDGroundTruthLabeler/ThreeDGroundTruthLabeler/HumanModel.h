#pragma once
#include <cmath>
#include <vector>
#include "HumanModel_define.h"
#include "numeric\matrix4.h"
#include "numeric\vector4.h"
#define PI 3.1415926535897932384626433832795
using namespace numeric;
namespace HumanModel
{	    
    #define Bid 0	
	class Model
	{
	public:
		Model()
		{
		}

		//public :
		//1. Related to parameter
		//limitDoF initparam angle_lowerbound angle_upperbound		
		int islimited[ParamNum];        //whether to fix that DoF
		double initparam[ParamNum];     //InitialRotationDegree
		int limit_pers_angle[3]; //perspective angle [3]
		int limit_pers_trans[2];
		int limit_pers_scale;
		double init_pers_angle[3];
		double init_pers_trans[2];
		double init_pers_scale;
		double bottom_diff[ParamNum];   //gradient of parameter
		double local_lb[ParamNum];
		double pers_global_rot_lb[3];
		double pers_global_trans_lb[2];
		double pers_scale_lb;
		double local_ub[ParamNum];
		double pers_global_rot_ub[3];
		double pers_global_trans_ub[2];
		double pers_scale_ub;

		//2. Related to joint
		int inobj[JointNum];            //whether to put the joint into objective function
		double top_data[JointNum * 3];  //blob output : joint vector
		//vec gt[JointNum];               //Ground truth of 2D joint location		
		numeric::Vector4d gt[JointNum];

		//3. Related to shape parameters(bone length) 
		double bonelen[BoneNum];
		
		//4. Configuration		
		int width, height;    //width and height of each image
		int crop_size, bbx_x1, bbx_y1, bbx_x2, bbx_y2;


		//5. Main public function		
		void Setup_ConstantMatrices();
		void Setup_Transformation();

		//Forward i-th joint (the size of i-th joint's father 's homogenous matrices is prevsize ; bottom_data stores the motion parameter vector)
		//bottom_data below all means the motion parameter vector
		void Forward(numeric::Matrix4d mat, int i, int prevsize, double *bottom_data);
		void Forward_cpu(double *bottom_data);
		void Backward(int i, double *bottom_data);
		void Backward_cpu(double *bottom_data, double *top_diff, int iter);
		numeric::Vector4d f[JointNum][ParamNum];
	private:
		//temporary variables & arrays
		//1. Related to transformation
		//Matr const_matr[ConstMatrNum];                        //constant matrices
		numeric::Matrix4d const_matr[ConstMatrNum];
		std::vector<std::pair<matrix_operation, int> > Homo_mat[JointNum]; //homogenous matrices (rotation & translation matrices)
		//Matr prevmat[JointNum];                               //prevmat * resttransformation
		numeric::Matrix4d prevmat[JointNum];
		//vec f[JointNum][ParamNum];                            //the gradient of joint with respect to parameter
		
		//2. Related to joint locations
		numeric::Vector4d joint[JointNum];
		//vec joint[JointNum];
		numeric::Matrix4d Model::GetMatrix(matrix_operation opt, int id, bool is_gradient, double *bottom_data);
	};
}