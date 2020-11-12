#include "PinholeCamera.h"
#define focus 300
namespace HumanModel
{
	void PinholeCamera::Forward_cpu(double *bottom_data)
	{		
		for (int i = 0; i < JointNum; i++)
		{
			int Botid = i * 3, Topid = i * 2;
			double x = bottom_data[Botid], y = bottom_data[Botid + 1], z = bottom_data[Botid + 2];
			top_data[Topid] = focus * x / z / size + 0.5;
			top_data[Topid + 1] = -focus * y / z / size + 0.5;
		}
	}

	void PinholeCamera::Backward_cpu(double *bottom_data, double *top_diff)
	{	
		for (int i = 0; i < JointNum; i++)
		{
			int Botid = i * 3, Topid = i * 2;
			double x = bottom_data[Botid], y = bottom_data[Botid + 1], z = bottom_data[Botid + 2];
			bottom_diff[Botid] = top_diff[Topid] * focus / z / size ;
			bottom_diff[Botid + 1] = -top_diff[Topid + 1] * focus / z / size;
			bottom_diff[Botid + 2] = -1.0 / (z * z) * focus * x / size * top_diff[Topid] +
				                      1.0 / (z * z) * focus * y / size * top_diff[Topid + 1];						
		}
	}
}