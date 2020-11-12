#include "ProjectionParamLoss.h"

namespace HumanModel
{
	void ProjectionParamLoss::Forward_cpu(double *pers_angle, double *pers_trans, double pers_scale, double *pers_angle_lowerbound, double *pers_angle_upperbound, double *pers_trans_lowerbound, double *pers_trans_upperbound, double pers_scale_lowerbound, double pers_scale_upperbound)
	{
		top_data = 0.0;
		for (int i = 0; i < 3; i++)
		{
			if (pers_angle[i] < pers_angle_lowerbound[i]) top_data = top_data + pow(pers_angle[i] - pers_angle_lowerbound[i], 2);
			else if (pers_angle[i] > pers_angle_upperbound[i]) top_data = top_data + pow(pers_angle[i] - pers_angle_upperbound[i], 2);
		}
		for (int i = 0; i < 2; i++)
		{
			if (pers_trans[i] < pers_trans_lowerbound[i]) top_data = top_data + pow(pers_trans[i] - pers_trans_lowerbound[i], 2);
			else if (pers_trans[i] > pers_trans_upperbound[i]) top_data = top_data + pow(pers_trans[i] - pers_trans_upperbound[i], 2);
		}
		if (pers_scale < pers_scale_lowerbound) top_data = top_data + pow(pers_scale - pers_scale_lowerbound, 2);
		else if (pers_scale > pers_scale_upperbound) top_data = top_data + pow(pers_scale - pers_scale_upperbound, 2);
		top_data = top_data / 6;
	}

	void ProjectionParamLoss::Backward_cpu(double *pers_angle, double *pers_trans, double pers_scale, double *pers_angle_lowerbound, double *pers_angle_upperbound, double *pers_trans_lowerbound, double *pers_trans_upperbound, double pers_scale_lowerbound, double pers_scale_upperbound)
	{
		for (int i = 0; i < 3; i++) pers_angle_diff[i] = 0.0;
		for (int i = 0; i < 2; i++) pers_trans_diff[i] = 0.0;
		pers_scale_diff = 0.0;
		for (int i = 0; i < 3; i++)
		{
			if (pers_angle[i] < pers_angle_lowerbound[i]) pers_angle_diff[i] = 2 * 1.0 / 6.0 * (pers_angle[i] - pers_angle_lowerbound[i]);
			else if (pers_angle[i] > pers_angle_upperbound[i]) pers_angle_diff[i] = 2 * 1.0 / 6.0 * (pers_angle[i] - pers_angle_upperbound[i]);
		}
		for (int i = 0; i < 2; i++)
		{
			if (pers_trans[i] < pers_trans_lowerbound[i]) pers_trans_diff[i] = 2 * 1.0 / 6.0 * (pers_trans[i] - pers_trans_lowerbound[i]);
			else if (pers_trans[i] > pers_trans_upperbound[i]) pers_trans_diff[i] = 2 * 1.0 / 6.0 * (pers_trans[i] - pers_trans_upperbound[i]);
		}
		if (pers_scale < pers_scale_lowerbound) pers_scale_diff = 2 * 1.0 / 6.0 * (pers_scale - pers_scale_lowerbound);
		else if (pers_scale >pers_scale_upperbound) pers_scale_diff = 2 * 1.0 / 6.0 * (pers_scale - pers_scale_upperbound);
	};
}
