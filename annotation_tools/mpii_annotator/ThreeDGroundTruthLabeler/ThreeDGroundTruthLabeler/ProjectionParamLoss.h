#pragma once
#include "HumanModel.h"
#include "basic.h"
namespace HumanModel
{
	class ProjectionParamLoss
	{
	public:
		double top_data = 0.0;
		double pers_angle_diff[3];
		double pers_trans_diff[2];
		double pers_scale_diff;
		void Forward_cpu(double *pers_angle, double *pers_trans, double pers_scale, double *pers_angle_lowerbound, double *pers_angle_upperbound, double *pers_trans_lowerbound, double *pers_trans_upperbound, double pers_scale_lowerbound, double pers_scale_upperbound);
		void Backward_cpu(double *pers_angle, double *pers_trans, double pers_scale, double *pers_angle_lowerbound, double *pers_angle_upperbound, double *pers_trans_lowerbound, double *pers_trans_upperbound, double pers_scale_lowerbound, double pers_scale_upperbound);
	};
}
