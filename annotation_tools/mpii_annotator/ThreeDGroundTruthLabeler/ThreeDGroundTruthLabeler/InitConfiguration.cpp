#include "InitConfiguration.h"
namespace HumanModel
{		
	void InitConfiguration::SetBoneLen(double *bone_len)
	{		
		for (int i = 0; i < BoneNum; i++) obj.bonelen[i] = bone_len[i];				
	}

	void InitConfiguration::ReadJointGT(char *jointgt_file) //why not use set because ground truth modification should be cautious
	{
		FILE *finjointgt = fopen(jointgt_file, "r");		
		for (int j = 0; j < JointNum; j++)
		{
			fscanf(finjointgt, "%*s");
			for (int k = 0; k < 2; k++) //while k==2 means the visibility of that joint
			{
				fscanf(finjointgt, "%lf", &jointgt[j * 2 + k]);
			}						
		}
		fclose(finjointgt);
	}

	void InitConfiguration::ReadVisibility(char *vis_file)
	{
		FILE *finvis = fopen(vis_file, "r");		
		for (int j = 0; j < JointNum; j++)
		{
			double t;
			fscanf(finvis, "%*s %lf", &t);
			visible[j] = (int)t;		
		}
		fclose(finvis);
	}

	void InitConfiguration::SetInitParam(double *init_param)
	{		
		for (int i = 0; i < ParamNum; i++)	
		{
			obj.initparam[i] = init_param[i];			
		}
	}

	void InitConfiguration::SetLimitDoF(int *is_limited)
	{				
		for (int i = 0; i < ParamNum; i++) obj.islimited[i] = is_limited[i];		
	}
	
	void InitConfiguration::SetInObj(int *in_obj)
	{		
		for (int i = 0; i < JointNum; i++) obj.inobj[i] = in_obj[i];		
	}

	void InitConfiguration::ReadImageInfo(char *imageinfo_file)	
	{				
		FILE *finimginfo = fopen(imageinfo_file, "r");
		for (int j = 0; j < 13; j++) fscanf(finimginfo, "%*s %lf", &config[j]);
		obj.crop_size = config[2];
		obj.bbx_x1    = config[3];
		obj.bbx_y1    = config[4];
		obj.bbx_x2    = config[5];
		obj.bbx_y2    = config[6];
		obj.width     = config[11];
		obj.height    = config[12];		
		fclose(finimginfo);
	}

	void InitConfiguration::ReadImageContent(char *image_name)
	{
		rgb = cv::imread(image_name);
	}
}