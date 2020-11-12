#include <iostream>
#include "ModelOptimization.h"
void Optimize_2d(InitConfiguration work, bool whether_to_add_angle_constraint) //projection    without any kind of constraint
{
	GradientDescent_2d t_grad;

	//Read Iteration times
	char itername[maxlen];
	sprintf(itername, "%s%s", directory, "\\configuration\\descent_iterations.in");
	FILE *fin_iter = fopen(itername, "r");
	int times;
	if (fin_iter == NULL) times = 10000;
	else
	{
		fscanf(fin_iter, "%d", &times);
		fclose(fin_iter);
	}	
	//--end of reading

	if (current_image_not_null == false) return;
	work.obj.Setup_ConstantMatrices();
	work.obj.Setup_Transformation();
	double m_rot[6];
	for (int i = 0; i < 6; i++) m_rot[i] = 0.0;	
	double t_trans[2];	
	t_trans[0] = work.obj.init_pers_trans[0];
	t_trans[1] = work.obj.init_pers_trans[1];    
	double angle[3];
	for (int i = 0; i < 3; i++)
	{
		angle[i] = work.obj.init_pers_angle[i];
	}                                             //use pers_angle pers_trans scale
	double t_param[ParamNum];
	for (int i = 0; i < ParamNum; i++) t_param[i] = 0.0;

	//whether to fix model parameters(local rotation degree)   only tuning rotation parameters and translation parameters(weak perspective projection)	
	if (!opt_local_model_param) //do not optimize local parameters(local rotation degree)
	{
		//after optimizing 3d    (so the param can be used here)
		for (int i = 0; i < ParamNum; i++) t_param[i] = opt_param[i]; 
	}	
	//--------------------------------------------------------
	for (int i = 0; i < JointNum; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			printf("%.2f ", work.jointgt[i * 2 + j]);
		}
	}
	printf("\n");

	t_grad.Gradscnt(t_param, work.jointgt, work.visible, work.obj, work.rgb, m_rot, t_trans, work.obj.init_pers_scale, angle, opt_local_model_param, times, directory, whether_to_add_angle_constraint);
	for (int i = 0; i < JointNum * 3; i++)
	{
		opt_location[i] = t_grad.bestlocation[i];
	}
	for (int i = 0; i < JointNum * 2; i++)
	{
		opt_projection[i] = t_grad.bestprojection[i];
	}
	for (int i = 0; i < ParamNum; i++)
	{
		opt_param[i] = t_grad.bestparam[i];
	}
	for (int i = 0; i < 3; i++) opt_angle[i] = t_grad.bestangle[i];
	for (int i = 0; i < 2; i++) opt_trans[i] = t_grad.besttrans[i];
	opt_scale = t_grad.bestscale;	
}

void Optimize_2d_pinhole(InitConfiguration work, bool whether_to_add_angle_constraint, bool is_shapeconvex) //projection    without any kind of constraint
{
	GradientDescent_2d_pinhole t_grad;

	//Read Iteration times
	char itername[maxlen];
	sprintf(itername, "%s%s", directory, "\\configuration\\descent_iterations.in");
	FILE *fin_iter = fopen(itername, "r");
	int times;
	if (fin_iter == NULL) times = 10000;
	else
	{
		fscanf(fin_iter, "%d", &times);
		fclose(fin_iter);
	}
	//--end of reading

	if (current_image_not_null == false) return;
	work.obj.Setup_ConstantMatrices();
	work.obj.Setup_Transformation();
	                    //use pers_angle pers_trans scale
	double t_param[ParamNum];
	for (int i = 0; i < ParamNum; i++) t_param[i] = 0.0;
	int save_is_limited[ParamNum];
	double save_initparam[ParamNum];
	if (is_shapeconvex)
	{		
		for (int i = 0; i < ParamNum; i++) save_is_limited[i] = work.obj.islimited[i];		
		for (int i = 0; i < ParamNum; i++) save_initparam[i] = work.obj.initparam[i];
		//backup the islimited array before modifying

		//for (int i = 3; i < ParamNum; i++) work.obj.islimited[i] = 1;
		for (int i = 3; i < ParamNum; i++) work.obj.initparam[i] = shape_convex_param[i];					
	}
	
	//whether to fix model parameters(local rotation degree)   only tuning rotation parameters and translation parameters(weak perspective projection)	
	if (!opt_local_model_param) //do not optimize local parameters(local rotation degree)
	{
		//after optimizing 3d    (so the param can be used here)
		for (int i = 0; i < ParamNum; i++) t_param[i] = opt_param[i];
	}
	//--------------------------------------------------------
	for (int i = 0; i < JointNum; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			printf("%.2f ", work.jointgt[i * 2 + j]);
		}
	}
	printf("\n");
	t_grad.Gradscnt(t_param, work.jointgt, work.visible, work.obj, work.rgb, opt_local_model_param, times, directory, whether_to_add_angle_constraint);
	for (int i = 0; i < JointNum * 3; i++)
	{
		opt_location[i] = t_grad.bestlocation[i];
	}
	for (int i = 0; i < JointNum * 2; i++)
	{
		opt_projection[i] = t_grad.bestprojection[i];
	}
	for (int i = 0; i < ParamNum; i++)
	{
		opt_param[i] = t_grad.bestparam[i];
	}
	if (is_shapeconvex)
	{
		int save_is_limited[ParamNum];
		for (int i = 0; i < ParamNum; i++) work.obj.islimited[i] = save_is_limited[i];
		for (int i = 0; i < ParamNum; i++) work.SetInitParam(save_initparam);
		//recovery from backup
	}
}

void Optimize_3d(InitConfiguration work, double *joint_3d, bool whether_to_add_angle_constraint) //projection    without any kind of constraint
{
	GradientDescent_3d t_grad;

	//Read Iteration times
	char itername[maxlen];
	sprintf(itername, "%s%s", directory, "\\configuration\\descent_iterations.in");
	FILE *fin_iter = fopen(itername, "r");
	int times;
	if (fin_iter == NULL) times = 10000;
	else
	{
		fscanf(fin_iter, "%d", &times);
		fclose(fin_iter);
	}
	
	//--end of reading

	if (current_image_not_null == false) return;
	work.obj.Setup_ConstantMatrices();
	work.obj.Setup_Transformation();
	double m_rot[6];
	for (int i = 0; i < 6; i++) m_rot[i] = 0.0;	
	double t_trans[2];	
	t_trans[0] = work.obj.init_pers_trans[0];
	t_trans[1] = work.obj.init_pers_trans[1];
	double angle[3];
	for (int i = 0; i < 3; i++) angle[i] = work.obj.init_pers_angle[i];	
	double t_param[ParamNum];
	for (int i = 0; i < ParamNum; i++) t_param[i] = 0.0;
	t_grad.Gradscnt(t_param, joint_3d, work.jointgt, work.visible, work.obj, work.rgb, m_rot, t_trans, work.obj.init_pers_scale, angle, times, directory, false);	
	//provide both joint_3d and 2d ground truth
	opt_local_model_param = false; //fix them (pers_rotation pers_translation pers_scale)
	for (int i = 0; i < ParamNum; i++)
	{
		opt_param[i] = t_grad.bestparam[i];   //save optimized local parameters(for optimize 2d)
	}
	Optimize_2d(work, whether_to_add_angle_constraint); //will save 
	opt_local_model_param = true; //allow it to be tuned again   (recovery)
}

void Optimize_3d_pinhole(InitConfiguration work, double *joint_3d, bool whether_to_add_angle_constraint) //projection    without any kind of constraint
{
	GradientDescent_3d_pinhole t_grad;

	//Read Iteration times
	char itername[maxlen];
	sprintf(itername, "%s%s", directory, "\\configuration\\descent_iterations.in");
	FILE *fin_iter = fopen(itername, "r");
	int times;
	if (fin_iter == NULL) times = 10000;
	else
	{
		fscanf(fin_iter, "%d", &times);
		fclose(fin_iter);
	}

	//--end of reading

	if (current_image_not_null == false) return;
	work.obj.Setup_ConstantMatrices();
	work.obj.Setup_Transformation();
	
	double t_param[ParamNum];
	for (int i = 0; i < ParamNum; i++) t_param[i] = 0.0;
	t_grad.Gradscnt(t_param, joint_3d, work.jointgt, work.visible, work.obj, work.rgb, times, directory, false, true);
	//provide both joint_3d and 2d ground truth
	opt_local_model_param = false; //fix them (pers_rotation pers_translation pers_scale)
	for (int i = 0; i < ParamNum; i++)
	{
		opt_param[i] = t_grad.bestparam[i];   //save optimized local parameters(for optimize 2d)
	}
	Optimize_2d_pinhole(work, whether_to_add_angle_constraint, false); //will save 
	opt_local_model_param = true; //allow it to be tuned again   (recovery)
}