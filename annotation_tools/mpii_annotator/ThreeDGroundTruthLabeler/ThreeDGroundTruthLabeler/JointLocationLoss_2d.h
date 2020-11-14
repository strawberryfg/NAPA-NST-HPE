#pragma once
#include "HumanModel.h"
namespace HumanModel
{
	class JointLocationLoss_2d
	{
	public:
		int valid_jointnum = 0;
		double top_data = 0.0;
		double bottom_diff[JointNum * 2];		
		void Forward_cpu(double *bottom_data, double *label_data, int *vis, int *inobj); 
		void Backward_cpu(double *bottom_data, double *label_data, int *vis, int *inobj);		
	};	
}