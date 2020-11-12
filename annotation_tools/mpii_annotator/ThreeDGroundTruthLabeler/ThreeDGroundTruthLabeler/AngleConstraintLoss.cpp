#include "AngleConstraintLoss.h"

namespace HumanModel
{
	void AngleConstraintLoss::Forward_cpu(double *bottom_data, double *angle_lowerbound, double *angle_upperbound)
	{		
		top_data = 0.0;				
		for (int i = 0; i < ParamNum; i++)
		{
			if (bottom_data[i] < angle_lowerbound[i]) top_data = top_data + pow(bottom_data[i] - angle_lowerbound[i], 2);
			else if (bottom_data[i]>angle_upperbound[i]) top_data = top_data + pow(bottom_data[i] - angle_upperbound[i], 2);
		}
		top_data = top_data / ParamNum;
	}

	void AngleConstraintLoss::Backward_cpu(double *bottom_data, double *angle_lowerbound, double *angle_upperbound)
	{
		for (int i = 0; i < ParamNum; i++) bottom_diff[i] = 0.0;
		for (int i = 0; i < ParamNum; i++)
		{
			if (bottom_data[i] < angle_lowerbound[i]) bottom_diff[i] = 2 * 1.0 / ParamNum * (bottom_data[i] - angle_lowerbound[i]);			
			else if (bottom_data[i]>angle_upperbound[i]) bottom_diff[i] = 2 * 1.0 / ParamNum * (bottom_data[i] - angle_upperbound[i]);
		}
	};
}
