#pragma once
#include "HumanModel.h"
#include "cv_related.h"
namespace HumanModel
{
	class InitConfiguration
	{
	public:
		cv::Mat rgb;
		//initial configurations are stored in obj
		Model obj;
		double jointgt[JointNum * 2];    // 2D joint location ground truth
		int visible[JointNum];           // = 1 : visible  = 0 : invisible
		double config[13];		


		//set can be modified during the process
		void SetBoneLen(double *bone_len);
		void ReadJointGT(char *jointgt_file);
		void ReadVisibility(char *vis_file);
		void SetInitParam(double *init_param);
		void SetLimitDoF(int *is_limit);
		void SetInObj(int *inobj);   		
		void ReadImageInfo(char *imageinfo_file);
		void ReadImageContent(char *image_name);
	private:		
	};
}