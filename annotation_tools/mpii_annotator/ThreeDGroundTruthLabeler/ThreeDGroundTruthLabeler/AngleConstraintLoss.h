#pragma once
#include "HumanModel.h"
#include "basic.h"
namespace HumanModel
{
	class AngleConstraintLoss
	{
	public:
		double top_data = 0.0;
		double bottom_diff[ParamNum];		
		double angle_lowerbound[ParamNum];
		double angle_upperbound[ParamNum];
		void Forward_cpu(double *bottom_data, double *angle_lowerbound, double *angle_upperbound);
		void Backward_cpu(double *bottom_data, double *angle_lowerbound, double *angle_upperbound);
	};
}
