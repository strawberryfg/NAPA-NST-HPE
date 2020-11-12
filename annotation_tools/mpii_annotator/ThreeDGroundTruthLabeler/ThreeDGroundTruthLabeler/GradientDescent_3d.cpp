
#include <cmath>
#include <algorithm>
#include <iostream>
#include "GradientDescent_3d.h"
using namespace std;
using namespace HumanModel;

namespace HumanModel
{
	//gt 3d location
	void GradientDescent_3d::Gradscnt(double *param, double *gt_3d, double *gt_2d, int *visible, Model gradmodel, Mat rgb, double *m_rot, double *t_trans, double s, double *angle, int MaxIter, char *directory, bool add_angle_constraint)
	{		
		double *t_gt = normalize_gt_2d(gt_2d, gradmodel);
		gradmodel.Forward_cpu(param);
		persrender.Forward_cpu(gradmodel.top_data, m_rot, t_trans, s, angle);
		locationloss_3d.Forward_cpu(gradmodel.top_data, gt_3d, visible, gradmodel.inobj);
		if (add_angle_constraint)
		{
			angle_constraint_loss.Forward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);
		}

		locationloss_3d.Backward_cpu(gradmodel.top_data, gt_3d, visible, gradmodel.inobj);		
		gradmodel.Backward_cpu(param, locationloss_3d.bottom_diff, 0);
		if (add_angle_constraint)
		{
			angle_constraint_loss.Backward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);
		}
		initerror = locationloss_3d.top_data;
		if (add_angle_constraint) initerror += angle_constraint_loss.top_data;
		iter = 0;
		lr = base_lr = 200;
		double ratio_globaloffset = 0.1;
		double ratio_globalz = 0.1;
		double ratio_globalrotation = 0.1;
		double ratio_others = 1;
		double ratio_angle = 1.0;
		fmax = 0.0;		
		double lastloss = initerror;
		int nochange = 0;
		Mat rgb_ = Mat::zeros(Size(224, 224), CV_8UC3);
		while (iter <= MaxIter)
		{
			iter++;
			lr = base_lr;
			while (1)
			{
				for (int i = 0; i < ParamNum; i++)
				{				
					if (!gradmodel.islimited[i])
					{
						if (i < 2) param[i] -= lr * ratio_globaloffset * gradmodel.bottom_diff[i];
						else if (i < 3) param[i] -= lr * ratio_globalz * gradmodel.bottom_diff[i];
						else if (i < 6) param[i] -= lr * ratio_globalrotation * gradmodel.bottom_diff[i];
						else param[i] -= lr * ratio_others * gradmodel.bottom_diff[i];

						if (add_angle_constraint) param[i] -= lr * ratio_angle * angle_constraint_loss.bottom_diff[i];
					}
				}
				//Backward Variable				
				for (int i = 0; i < 3; i++)
				{
					if (!gradmodel.limit_pers_angle[i]) angle[i] -= lr * persrender.angle_diff[i];
				}
				if (!gradmodel.limit_pers_scale) s -= lr * 1 * persrender.s_diff;
				for (int i = 0; i < 2; i++)
				{
					if (!gradmodel.limit_pers_trans[i]) t_trans[i] -= lr * persrender.t_trans_diff[i];
				}
				//End of Backward
				//Forward the network
				gradmodel.Forward_cpu(param);
				persrender.Forward_cpu(gradmodel.top_data, m_rot, t_trans, s, angle);
				locationloss_3d.Forward_cpu(gradmodel.top_data, gt_3d, visible, gradmodel.inobj);
				if (add_angle_constraint)
				{
					angle_constraint_loss.Forward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);
				}
				double now_loss = locationloss_3d.top_data + (add_angle_constraint ? angle_constraint_loss.top_data : 0.0);
				if (now_loss - lastloss > 1e-16)
				{
					for (int i = 0; i < ParamNum; i++)
					{					
						if (!gradmodel.islimited[i])
						{
							if (i < 2) param[i] += lr * ratio_globaloffset * gradmodel.bottom_diff[i];
							else if (i < 3) param[i] += lr * ratio_globalz * gradmodel.bottom_diff[i];
							else if (i < 6) param[i] += lr * ratio_globalrotation * gradmodel.bottom_diff[i];
							else param[i] += lr * ratio_others * gradmodel.bottom_diff[i];

							if (add_angle_constraint) param[i] += lr * ratio_angle * angle_constraint_loss.bottom_diff[i];
						}						
					}
					for (int i = 0; i < 3; i++)
					{
					 	if (!gradmodel.limit_pers_angle[i]) angle[i] += lr * persrender.angle_diff[i];
					}
					if (!gradmodel.limit_pers_scale) s += lr * 1 * persrender.s_diff;
					for (int i = 0; i < 2; i++)
					{
						if (!gradmodel.limit_pers_trans[i]) t_trans[i] += lr * persrender.t_trans_diff[i];
					}
					lr = lr * 0.5;
				}
				else break;
				if (lr < 0.0001) break;
			}

			if ((iter - 1) % 10000 == 0)
			{
				for (int h = 0; h < 224; h++)
				{
					for (int w = 0; w < 224; w++)
					{
						rgb_.at<Vec3b>(h, w)[0] = rgb.at<Vec3b>(h, w)[0];
						rgb_.at<Vec3b>(h, w)[1] = rgb.at<Vec3b>(h, w)[1];
						rgb_.at<Vec3b>(h, w)[2] = rgb.at<Vec3b>(h, w)[2];
					}
				}
				disp.Display(rgb_, persrender.top_data, gt_2d, 1, 1, 1, 1, visible, gradmodel.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
				printf("Current Iteration %7d last error: %12.8f error to GT %12.8f current stepsize: %10.6f\n", iter, lastloss, locationloss_3d.top_data, lr);
				printf("--------------------------------------\n");
				printf("camera rotation matrix: \n");
				for (int j = 0; j < 3; j++) printf("%12.6f ", m_rot[j]);
				printf("\n");
				for (int j = 0; j < 3; j++) printf("%12.6f ", m_rot[j + 3]);
				printf("\n");
				printf("scale: %12.6f \n", s);
				printf("t_u: %12.6f t_v: %12.6f\n", t_trans[0], t_trans[1]);
			}

			fmax = 0.0;
			for (int i = 0; i < ParamNum; i++) fmax = max(fmax, fabs(lastparam[i] - param[i]));
			//if (fmax < 0.0001) break;
			double now_loss = locationloss_3d.top_data + (add_angle_constraint ? angle_constraint_loss.top_data : 0.0);
			if (fabs(now_loss - lastloss) < 1e-10)
			{
				nochange++;
				if (nochange>500) break;
			}
			else nochange = 0;			
			for (int i = 0; i < ParamNum; i++) lastparam[i] = param[i];
			lastloss = now_loss;

			//Backpropogate the gradient			
			locationloss_3d.Backward_cpu(gradmodel.top_data, gt_3d, visible, gradmodel.inobj);			
			gradmodel.Backward_cpu(param, locationloss_3d.bottom_diff, 0);			
			if (add_angle_constraint)
			{
				angle_constraint_loss.Backward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);
			}
		}
		for (int i = 3; i < ParamNum; i++)
		{
			while (param[i] > PI) param[i] -= 2 * PI;
			while (param[i] < -PI) param[i] += 2 * PI;
		}
		for (int i = 0; i < ParamNum; i++) bestparam[i] = param[i];
		gradmodel.Forward_cpu(bestparam);
		persrender.Forward_cpu(gradmodel.top_data, m_rot, t_trans, s, angle);
		for (int i = 0; i < JointNum * 3; i++) bestlocation[i] = gradmodel.top_data[i];		
		for (int i = 0; i < JointNum * 2; i++)	bestprojection[i] = persrender.top_data[i];
		for (int i = 0; i < 3; i++) bestangle[i] = angle[i];
		for (int i = 0; i < 2; i++) besttrans[i] = t_trans[i];
		bestscale = s;
		for (int h = 0; h < 224; h++)
		{
			for (int w = 0; w < 224; w++)
			{
				rgb_.at<Vec3b>(h, w)[0] = rgb.at<Vec3b>(h, w)[0];
				rgb_.at<Vec3b>(h, w)[1] = rgb.at<Vec3b>(h, w)[1];
				rgb_.at<Vec3b>(h, w)[2] = rgb.at<Vec3b>(h, w)[2];
			}
		}
		disp.Display(rgb_, persrender.top_data, t_gt, 1, 1, 1, 1, visible, gradmodel.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
	}
}
