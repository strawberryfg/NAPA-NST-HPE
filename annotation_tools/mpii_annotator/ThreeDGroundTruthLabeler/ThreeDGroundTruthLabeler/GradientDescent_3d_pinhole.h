#pragma once
#include "HumanModel.h"
#include "JointLocationLoss_3d.h"
#include "AngleConstraintLoss.h"
#include "PinholeCamera.h"
#include "DisplayJointOnImage.h"
namespace HumanModel
{
	class GradientDescent_3d_pinhole
	{
	public:
		//gt here is the 3d prediction
		void Gradscnt(double *param, double *gt_3d, double *gt_2d, int *visible, Model gradmodel, Mat rgb, int MaxIter, char *directory, bool add_angle_constraint, bool show_2d);
		//if use_2d is set to false then it's just 3d optimization
		double bestparam[ParamNum];
		double bestlocation[JointNum * 3];
		double bestprojection[JointNum * 2];
		
	private:
		DisplayJoint disp;
		PinholeCamera pinholerender;
		JointLocationLoss_3d locationloss_3d;
		AngleConstraintLoss angle_constraint_loss;
		//temporary variable of SGD
		int iter;
		double lr, base_lr;
		double initerror;
		double fmax;
		double lastparam[ParamNum];
	};
}