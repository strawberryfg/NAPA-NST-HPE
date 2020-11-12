#include <cmath>
#include <algorithm>
#include <iostream>
#include "GradientDescent_2d.h"
using namespace std;
using namespace HumanModel;

namespace HumanModel
{
	void GradientDescent_2d::Gradscnt(double *param, double *gt_2d, int *visible, Model gradmodel, Mat rgb, double *m_rot, double *t_trans, double s, double *angle, bool opt_model_param, int MaxIter, char *directory, bool add_angle_constraint)
	{
		double *t_gt = normalize_gt_2d(gt_2d, gradmodel);
		gradmodel.Forward_cpu(param);
		persrender.Forward_cpu(gradmodel.top_data, m_rot, t_trans, s, angle);
		locationloss_2d.Forward_cpu(persrender.top_data, t_gt, visible, gradmodel.inobj);		
		if (add_angle_constraint)
		{
			angle_constraint_loss.Forward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);
			projection_param_loss.Forward_cpu(angle, t_trans, s, gradmodel.pers_global_rot_lb, gradmodel.pers_global_rot_ub, gradmodel.pers_global_trans_lb, gradmodel.pers_global_trans_ub, gradmodel.pers_scale_lb, gradmodel.pers_scale_ub);
		}
		locationloss_2d.Backward_cpu(persrender.top_data, t_gt, visible, gradmodel.inobj);
		persrender.Backward_cpu(gradmodel.top_data, locationloss_2d.bottom_diff, m_rot, t_trans, s, angle);
		if (opt_model_param) gradmodel.Backward_cpu(param, persrender.bottom_diff, 0);
		if (add_angle_constraint)
		{			
			angle_constraint_loss.Backward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);
			projection_param_loss.Backward_cpu(angle, t_trans, s, gradmodel.pers_global_rot_lb, gradmodel.pers_global_rot_ub, gradmodel.pers_global_trans_lb, gradmodel.pers_global_trans_ub, gradmodel.pers_scale_lb, gradmodel.pers_scale_ub);
		}
		initerror = locationloss_2d.top_data;
		if (add_angle_constraint) initerror += angle_constraint_loss.top_data + projection_param_loss.top_data;
		iter = 0;
		lr = base_lr = 200;
		double ratio_globaloffset = 0.1;
		double ratio_globalz = 0.1;
		double ratio_globalrotation = 0.1;
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
				if (add_angle_constraint)
				{
					for (int i = 0; i < 3; i++) angle[i] -= lr * ratio_angle * projection_param_loss.pers_angle_diff[i];
					for (int i = 0; i < 2; i++) t_trans[i] -= lr * ratio_angle * projection_param_loss.pers_trans_diff[i];
					s -= lr * ratio_angle * projection_param_loss.pers_scale_diff;
				}
				//Projection parameters          angle  s  t_trans
				//Backward Variable				
				for (int i = 0; i < 3; i++)
				{
					if (!gradmodel.limit_pers_angle[i]) angle[i] -= lr * persrender.angle_diff[i]; //not limit perspective_global_rotation_i					
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
				locationloss_2d.Forward_cpu(persrender.top_data, t_gt, visible, gradmodel.inobj);				
				if (add_angle_constraint)
				{
					angle_constraint_loss.Forward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);
					projection_param_loss.Forward_cpu(angle, t_trans, s, gradmodel.pers_global_rot_lb, gradmodel.pers_global_rot_ub, gradmodel.pers_global_trans_lb, gradmodel.pers_global_trans_ub, gradmodel.pers_scale_lb, gradmodel.pers_scale_ub);
				}
				double now_loss = locationloss_2d.top_data + (add_angle_constraint ? angle_constraint_loss.top_data + projection_param_loss.top_data : 0.0);
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
					for (int i = 0; i < 3; i++)
					{
						if (!gradmodel.limit_pers_angle[i]) angle[i] += lr * persrender.angle_diff[i];
					}
					if (!gradmodel.limit_pers_scale) s += lr * 1 * persrender.s_diff;
					for (int i = 0; i < 2; i++)
					{
						if (!gradmodel.limit_pers_trans[i]) t_trans[i] += lr * persrender.t_trans_diff[i];
					}
					if (add_angle_constraint)
					{
						for (int i = 0; i < 3; i++) angle[i] += lr * ratio_angle * projection_param_loss.pers_angle_diff[i];
						for (int i = 0; i < 2; i++) t_trans[i] += lr * ratio_angle * projection_param_loss.pers_trans_diff[i];
						s += lr * ratio_angle * projection_param_loss.pers_scale_diff;
					}
					lr = lr * 0.5;
				}
				else break;
				if (lr < 0.0001) break;				
			}
			
			if ((iter - 1) % 10000== 0)
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
				double now_loss = locationloss_2d.top_data + (add_angle_constraint ? angle_constraint_loss.top_data + projection_param_loss.top_data : 0.0);
				printf("Current Iteration %7d last error: %12.8f 2D error to GT %12.8f current stepsize: %10.6f\n", iter, lastloss, now_loss, lr);				
				printf("--------------------------------------\n");
				disp.Display(rgb_, persrender.top_data, t_gt, 1, 1, 1, 1, visible, gradmodel.inobj, NULL, mul1, mul2, b1, b2, b3, directory);
				printf("camera rotation matrix: \n");
				for (int j = 0; j < 3; j++) printf("%12.6f ", m_rot[j]);
				printf("\n");
				for (int j = 0; j < 3; j++) printf("%12.6f ", m_rot[j + 3]);
				printf("\n");
				printf("scale: %12.6f \n", s);
				printf("t_u: %12.6f t_v: %12.6f\n", t_trans[0], t_trans[1]);				
			}	
			double now_loss = locationloss_2d.top_data + (add_angle_constraint ? angle_constraint_loss.top_data + projection_param_loss.top_data : 0.0);
			if (fabs(now_loss - lastloss) < 1e-10)
			{
				nochange++;
				if (nochange>500) break;
			}
			else nochange = 0;			
			lastloss = now_loss;
					
			//Backpropogate the gradient			
			locationloss_2d.Backward_cpu(persrender.top_data, t_gt, visible, gradmodel.inobj);
			persrender.Backward_cpu(gradmodel.top_data, locationloss_2d.bottom_diff, m_rot, t_trans, s, angle);			
			if (opt_model_param) gradmodel.Backward_cpu(param, persrender.bottom_diff, 0);						
			if (add_angle_constraint)
			{
				angle_constraint_loss.Backward_cpu(param, gradmodel.local_lb, gradmodel.local_ub);
				projection_param_loss.Backward_cpu(angle, t_trans, s, gradmodel.pers_global_rot_lb, gradmodel.pers_global_rot_ub, gradmodel.pers_global_trans_lb, gradmodel.pers_global_trans_ub, gradmodel.pers_scale_lb, gradmodel.pers_scale_ub);
			}
		}
		for (int i = 3; i < ParamNum; i++)
		{
			while (param[i] > PI) param[i] -= 2 * PI;
			while (param[i] < -PI) param[i] += 2 * PI;
		}		
		for (int i = 0; i < 3; i++)
		{
			while (angle[i] > PI) angle[i] -= 2 * PI;
			while (angle[i] < -PI) angle[i] += 2 * PI;
		}
		for (int i = 0; i < ParamNum; i++) bestparam[i] = param[i];
		//for (int i = 0; i < ParamNum; i++) bestparam[i] = 0;
		//bestparam[global_rot_x] = deg2rad(50.0);
		//angle[0] = deg2rad(40.0);
		//angle[1] = deg2rad(20.0);
		//angle[2] = 0.0;
		gradmodel.Forward_cpu(bestparam);		
		persrender.Forward_cpu(gradmodel.top_data, m_rot, t_trans, s, angle);		
		for (int i = 0; i < JointNum * 3; i++) bestlocation[i] = gradmodel.top_data[i];		
		for (int i = 0; i < JointNum * 2; i++) bestprojection[i] = persrender.top_data[i];
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
