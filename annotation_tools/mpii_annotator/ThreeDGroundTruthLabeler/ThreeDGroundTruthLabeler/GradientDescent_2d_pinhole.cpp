#include <cmath>
#include <algorithm>
#include <iostream>
#include "GradientDescent_2d_pinhole.h"
using namespace std;
using namespace HumanModel;

namespace HumanModel
{
	void GradientDescent_2d_pinhole::Gradscnt(double *param, double *gt_2d, int *visible, Model gradmodel, Mat rgb, bool opt_model_param, int MaxIter, char *directory, bool add_angle_constraint)
	{
		double *t_gt = normalize_gt_2d(gt_2d, gradmodel);
		gradmodel.Forward_cpu(param);
		pinholerender.Forward_cpu(gradmodel.top_data);		
		locationloss_2d.Forward_cpu(pinholerender.top_data, t_gt, visible, gradmodel.inobj);
		if (add_angle_constraint)
		{
			angle_constraint_loss.Forward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);			
		}
		locationloss_2d.Backward_cpu(pinholerender.top_data, t_gt, visible, gradmodel.inobj);
		pinholerender.Backward_cpu(gradmodel.top_data, locationloss_2d.bottom_diff);
		if (opt_model_param) gradmodel.Backward_cpu(param, pinholerender.bottom_diff, 0);
		if (add_angle_constraint)
		{
			angle_constraint_loss.Backward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);			
		}
		initerror = locationloss_2d.top_data;
		if (add_angle_constraint) initerror += angle_constraint_loss.top_data;
		iter = 0;
		lr = base_lr = 200;
		double ratio_globaloffset = 0.1;
		double ratio_globalz = 1;
		double ratio_globalrotation = 1;
		double ratio_others = 1;
		double ratio_angle = 1.0;
		double lastloss = initerror;
		int nochange = 0;
		Mat rgb_ = Mat::zeros(Size(224, 224), CV_8UC3);
		while (iter <= MaxIter)
		{
			iter++;
			lr = base_lr;
			while (1)
			{
				if (opt_model_param)
				{
					for (int i = 0; i < ParamNum; i++)
					{
						if (!gradmodel.islimited[i])    //not fixed
						{
							if (i < 2) param[i] -= lr * ratio_globaloffset * gradmodel.bottom_diff[i];
							else if (i < 3) param[i] -= lr * ratio_globalz * gradmodel.bottom_diff[i];
							else if (i < 6) param[i] -= lr * ratio_globalrotation * gradmodel.bottom_diff[i];
							else param[i] -= lr * ratio_others * gradmodel.bottom_diff[i];

							if (add_angle_constraint) param[i] -= lr * ratio_angle * angle_constraint_loss.bottom_diff[i];
						}
					}
				}
				
				
				//End of Backward
				//Forward the network
				gradmodel.Forward_cpu(param);
				pinholerender.Forward_cpu(gradmodel.top_data);
				locationloss_2d.Forward_cpu(pinholerender.top_data, t_gt, visible, gradmodel.inobj);
				if (add_angle_constraint)
				{
					angle_constraint_loss.Forward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);					
				}
				double now_loss = locationloss_2d.top_data + (add_angle_constraint ? angle_constraint_loss.top_data  : 0.0);
				if (now_loss - lastloss > 1e-16)
				{
					if (opt_model_param)
					{
						for (int i = 0; i < ParamNum; i++)
						{
							if (!gradmodel.islimited[i]) //not fix tunable
							{
								if (i < 2) param[i] += lr * ratio_globaloffset * gradmodel.bottom_diff[i];
								else if (i < 3) param[i] += lr * ratio_globalz * gradmodel.bottom_diff[i];
								else if (i < 6) param[i] += lr * ratio_globalrotation * gradmodel.bottom_diff[i];
								else param[i] += lr * ratio_others * gradmodel.bottom_diff[i];
								if (add_angle_constraint) param[i] += lr * ratio_angle * angle_constraint_loss.bottom_diff[i];
							}
						}
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
				double now_loss = locationloss_2d.top_data + (add_angle_constraint ? angle_constraint_loss.top_data : 0.0);
				printf("Current Iteration %7d last error: %12.8f 2D error to GT %12.8f current stepsize: %10.6f\n", iter, lastloss, now_loss, lr);
				printf("--------------------------------------\n");
				disp.Display(rgb_, pinholerender.top_data, t_gt, 1, 1, 1, 1, visible, gradmodel.inobj, NULL, mul1, mul2, b1, b2, b3, directory);				
			}
			double now_loss = locationloss_2d.top_data + (add_angle_constraint ? angle_constraint_loss.top_data  : 0.0);
			if (fabs(now_loss - lastloss) < 1e-10)
			{
				nochange++;
				if (nochange>500) break;
			}
			else nochange = 0;
			lastloss = now_loss;

			//Backpropogate the gradient			
			locationloss_2d.Backward_cpu(pinholerender.top_data, t_gt, visible, gradmodel.inobj);
			pinholerender.Backward_cpu(gradmodel.top_data, locationloss_2d.bottom_diff);
			if (opt_model_param) gradmodel.Backward_cpu(param, pinholerender.bottom_diff, 0);
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
		//for (int i = 0; i < ParamNum; i++) bestparam[i] = 0;
		//bestparam[global_rot_x] = deg2rad(50.0);
		//angle[0] = deg2rad(40.0);
		//angle[1] = deg2rad(20.0);
		//angle[2] = 0.0;
		gradmodel.Forward_cpu(bestparam);
		pinholerender.Forward_cpu(gradmodel.top_data);
		for (int i = 0; i < JointNum * 3; i++) bestlocation[i] = gradmodel.top_data[i];
		for (int i = 0; i < JointNum * 2; i++) bestprojection[i] = pinholerender.top_data[i];
		
		for (int h = 0; h < 224; h++)
		{
			for (int w = 0; w < 224; w++)
			{
				rgb_.at<Vec3b>(h, w)[0] = rgb.at<Vec3b>(h, w)[0];
				rgb_.at<Vec3b>(h, w)[1] = rgb.at<Vec3b>(h, w)[1];
				rgb_.at<Vec3b>(h, w)[2] = rgb.at<Vec3b>(h, w)[2];
			}
		}
		disp.Display(rgb_, pinholerender.top_data, t_gt, 1, 1, 1, 1, visible, gradmodel.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
	}
}
