#pragma once
#include "HumanModel.h"
namespace HumanModel
{
	class PinholeCamera
	{
	public:		
		const double size = 224.0;
		double top_data[JointNum * 2];
		double bottom_diff[JointNum * 3];				
		void Forward_cpu(double *bottom_data);
		void Backward_cpu(double *bottom_data, double *top_diff);
	};
}