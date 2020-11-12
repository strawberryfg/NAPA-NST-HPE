#pragma once
#include "HumanModel.h"
#include "JointLocationLoss_3d.h"
#include "AngleConstraintLoss.h"
#include "WeakPerspective.h"
#include "DisplayJointOnImage.h"
namespace HumanModel
{
	class GradientDescent_3d
	{
	public:
		//gt here is the 3d prediction
		void Gradscnt(double *param, double *gt_3d, double *gt_2d, int *visible, Model gradmodel, Mat rgb, double *m_rot, double *t_trans, double s, double *angle, int MaxIter, char *directory, bool add_angle_constraint);
		double bestparam[ParamNum];
		double bestlocation[JointNum * 3];
		double bestprojection[JointNum * 2];
		double bestangle[3];
		double besttrans[2];
		double bestscale;
	private:
		DisplayJoint disp;
		WeakPerspective persrender;
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