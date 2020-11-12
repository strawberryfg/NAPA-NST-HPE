#pragma once
#include "HumanModel.h"
#include "JointLocationLoss_2d.h"
#include "AngleConstraintLoss.h"
#include "ProjectionParamLoss.h"
#include "DisplayJointOnImage.h"
#include "PinholeCamera.h"
namespace HumanModel
{
	class GradientDescent_2d_pinhole
	{
	public:
		//param : motion parameter 
		//gt : joint 2D ground truth 
		//visible : if invisible, set weight to 0 in objective function and don't display that joint
		//id : image id foutparam/fout3D/fout2D/fouterr : file pointer for saving parameters & 3D/2D joint locations & joint error outputprefix : the prefix of output image rgb: display on the Mat structure "rgb"
		void Gradscnt(double *param, double *gt_2d, int *visible, Model gradmodel, Mat rgb,  bool local_param, int MaxIter, char *directory, bool add_angle_constraint);
		//opt_model_param=true means 0..ParamNum-1 can be tuned(fixing it means only optimizing projection parameters)
		double bestparam[ParamNum];
		double bestlocation[JointNum * 3];
		double bestprojection[JointNum * 2];		
	private:
		PinholeCamera pinholerender;
		JointLocationLoss_2d locationloss_2d;
		DisplayJoint disp;
		AngleConstraintLoss angle_constraint_loss;		
		//temporary variable of SGD
		int iter;
		double lr, base_lr;
		double initerror;
	};
}