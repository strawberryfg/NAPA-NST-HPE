#include "JointLocationLoss_3d.h"
namespace HumanModel
{
	void JointLocationLoss_3d::Forward_cpu(double *bottom_data, double *label_data, int *vis, int *inobj)
	{
		valid_jointnum = 0;
		top_data = 0.0;
		for (int i = 0; i < JointNum; i++)
		{
			if (vis[i] && inobj[i])
			{
				valid_jointnum++;
				for (int j = 0; j < 3; j++)
				{
					top_data += ((bottom_data[i * 3 + j] - label_data[i * 3 + j]) * (bottom_data[i * 3 + j] - label_data[i * 3 + j]));
				}
			}
		}
		top_data = top_data / double(valid_jointnum);
	}

	void JointLocationLoss_3d::Backward_cpu(double *bottom_data, double *label_data, int *vis, int *inobj)
	{
		double top_diff = 1.0 / double(valid_jointnum);
		for (int i = 0; i < JointNum; i++)
		{
			for (int j = 0; j < 3; j++) bottom_diff[i * 3 + j] = 0.0;
			if (vis[i] && inobj[i])
			{
				for (int j = 0; j < 3; j++)
				{
					bottom_diff[i * 3 + j] = top_diff * 2 * (bottom_data[i * 3 + j] - label_data[i * 3 + j]);
				}
			}
		}
	};
}
