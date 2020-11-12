#pragma once
#include "HumanModel.h"
namespace HumanModel
{	
	class WeakPerspective
	{
	public:
		double top_data[JointNum * 2];
		double bottom_diff[JointNum * 3];
		double m_rot_diff[6];
		double t_trans_diff[2];
		double s_diff;
		double angle_diff[3]; //alpha beta gamma
		void Forward_cpu(double *bottom_data, double *m_rot, double *t_trans, double s, double *angle);
		void Backward_cpu(double *bottom_data, double *top_diff, double *m_rot, double *t_trans, double s, double *angle);
	};
}