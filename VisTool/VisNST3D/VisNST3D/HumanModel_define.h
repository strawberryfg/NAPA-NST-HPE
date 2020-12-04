#pragma once
#define pb push_back
#define mp std::make_pair
#define maxlen 555
#include <cstring>
using namespace std;
enum basic_settings
{
	JointNum = 18, BoneNum = 9, ParamNum = 28, ConstMatrNum = 16, PartNum = 7
, BoneBasedBoneNum = 15
};

enum joint //18
{
	pelvis = 0   ,
	right_hip = 1 ,
	right_knee = 2 ,
	right_ankle = 3 ,
	left_hip = 4    ,
	left_knee = 5    ,
	left_ankle = 6    ,
	upper_neck = 8     ,
	head_top = 10       ,
	left_shoulder = 11 ,
	left_elbow = 12    ,
	left_wrist = 13   ,
	right_shoulder = 14,
	right_elbow = 15    ,
	right_wrist = 16    ,
	thorax = 17     
};


const int bones[BoneBasedBoneNum][2] =
{
	{ right_ankle, right_knee },
			{ right_knee, right_hip },
			{ right_hip, pelvis },
			{ left_hip, pelvis },
			{ left_knee, left_hip },
			{ left_ankle, left_knee },
			{ pelvis, thorax },
			{ upper_neck, thorax },
			{ head_top, upper_neck },
			{ right_wrist, right_elbow },
			{ right_elbow, right_shoulder },
			{ right_shoulder, thorax },
			{ left_shoulder, thorax },
			{ left_elbow, left_shoulder },
			{ left_wrist, left_elbow }

};

/*enum joint // 16
{
	right_ankle,    //0
	right_knee,     //1
	right_hip,      //2
	left_hip,       //3
	left_knee,      //4
	left_ankle,     //5
	pelvis,         //6
	thorax,         //7
	upper_neck,     //8
	head_top,       //9
	right_wrist,    //10
	right_elbow,    //11
	right_shoulder, //12
	left_shoulder,  //13
	left_elbow,     //14
	left_wrist      //15
};*/


enum bone // 9
{
	bone_knee_connect_ankle,          //0
	bone_hip_connect_knee,            //1
	bone_pelvis_connect_hip,          //2
	bone_thorax_connect_pelvis,       //3
	bone_thorax_connect_upper_neck,   //4
	bone_upper_neck_connect_head_top, //5
	bone_elbow_connect_wrist,         //6
	bone_shoulder_connect_elbow,      //7
	bone_thorax_connect_shoulder      //8
};

enum dof //28
{
	global_trans_x,          //0
	global_trans_y,          //1
	global_trans_z,          //2
	global_rot_x,            //3
	global_rot_y,            //4
	global_rot_z,            //5
	right_knee_rot_x,        //6
	right_hip_rot_x,         //7
	right_hip_rot_y,         //8
	right_hip_rot_z,         //9
	left_hip_rot_x,          //10
	left_hip_rot_y,          //11
	left_hip_rot_z,          //12
	left_knee_rot_x,         //13
	upper_neck_rot_x,        //14
	upper_neck_rot_y,        //15
	upper_neck_rot_z,        //16
	head_top_rot_x,          //17
	right_elbow_rot_x,       //18
	right_shoulder_rot_x,    //19
	right_shoulder_rot_y,    //20
	right_shoulder_rot_z,    //21
	left_shoulder_rot_x,     //22
	left_shoulder_rot_y,     //23
	left_shoulder_rot_z,     //24
	left_elbow_rot_x,        //25
	hip_rot_y,               //26
	hip_rot_z                //27
};

const int forward_seq[JointNum] = { 
	thorax,            //0
	upper_neck,        //1
	pelvis,            //2
	head_top,          //3 
	right_shoulder,    //4
	right_elbow,       //5
	right_wrist,       //6
	left_shoulder,     //7
	left_elbow,        //8
	left_wrist,        //9
	right_hip,         //10
	right_knee,        //11
	right_ankle,       //12
	left_hip,          //13
	left_knee,         //14
	left_ankle };      //15
const int prev_seq[JointNum] = { 
	-1,                //0
	thorax,            //1
	thorax,            //2
	upper_neck,        //3
	thorax,            //4
	right_shoulder,    //5
	right_elbow,       //6
	thorax,            //7
	left_shoulder,     //8
	left_elbow,        //9
	-1,                //10
	right_hip,         //11
	right_knee,        //12
	-1,                //13
	left_hip,          //14
	left_knee };       //15

const int body_part[PartNum][2] = { { left_hip, right_hip }, { left_shoulder, right_shoulder }, { left_elbow, right_elbow }, { left_wrist, right_wrist }, { left_knee, right_knee }, { left_ankle, right_ankle }, { head_top, upper_neck } };
