#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <omp.h>
#include "HumanModel.h"
#include "basic.h"
using namespace cv;
namespace HumanModel
{
	#define yello Scalar(0,255,255)
	#define red Scalar(0,0,255)
	#define blue Scalar(255, 191, 0)
	#define green Scalar(0,255,0)
	#define black Scalar(0,0,0)
	#define white Scalar(255,255,255)

	const int SIZE2 = 224;	
	const int edges2[EdgeNum][2] =
	{ { right_knee, right_ankle },    //0
	  { right_hip, right_knee },      //1
	  { pelvis, right_hip },          //2
	  { pelvis, left_hip },           //3
	  { left_hip, left_knee },        //4
	  { left_knee, left_ankle },      //5
	  { pelvis, thorax },             //6
	  { thorax, upper_neck },         //7
	  { upper_neck, head_top },       //8
	  { right_elbow, right_wrist },   //9
	  { right_shoulder, right_elbow },//10
	  { thorax, right_shoulder },     //11
	  { thorax, left_shoulder },      //12
	  { left_shoulder, left_elbow },  //13
	  { left_elbow, left_wrist }      //14
	};
	const Scalar color_joint_gt2[JointNum] = 
	{ 
		red,   //0  right_ankle
		red,   //1  right_knee
		red,   //2  right_hip
		yello, //3  left_hip
		yello, //4  left_knee
		yello, //5  left_ankle
		red,   //6  pelvis
		red,   //7  thorax
		red,   //8  upper_neck
		red,   //9  head_top
		red,   //10 right_wrist
		red,   //11 right_elbow
		red,   //12 right_shoulder
		yello, //13 left_shoulder
		yello, //14 left_elbow
		yello};//15 left_wrist
	const Scalar color_joint_pred2[JointNum] = { 
		blue,  //0  right_ankle
		blue,  //1  right_knee
		blue,  //2  right_hip
		green, //3  left_hip
		green, //4  left_knee
		green, //5  left_ankle
		blue,  //6  pelvis
		blue,  //7  thorax
		blue,  //8  upper_neck
		blue,  //9  head_top
		blue,  //10 right_wrist
		blue,  //11 right_elbow
		blue,  //12 right_shoulder
		green, //13 left_shoulder
		green, //14 left_elbow
		green};//15 left_wrist
	const Scalar color_bone_gt2[EdgeNum] = { 
		red,   //0  right_knee      ---     right_ankle
		red,   //1  right_hip       ---     right_knee
		red,   //2  pelvis          ---     right_hip
		yello, //3  pelvis          ---     left_hip
		yello, //4  left_hip        ---     left_knee
		yello, //5  left_knee       ---     left_ankle
		red,   //6  pelvis          ---     thorax
		red,   //7  thorax          ---     upper_neck
		red,   //8  upper_neck      ---     head_top
		red,   //9  right_elbow     ---     right_wrist
		red,   //10 right_shoulder  ---     right_elbow
		red,   //11 thorax          ---     right_shoulder
		yello, //12 thorax          ---     left_shoulder
		yello, //13 left_shoulder   ---     left_elbow
		yello};//14 left_elbow      ---     left_wrist
	const Scalar color_bone_pred2[EdgeNum] = { 
		blue,  //0  right_knee      ---     right_ankle
		blue,  //1  right_hip       ---     right_knee
		blue,  //2  pelvis          ---     right_hip
		green, //3  pelvis          ---     left_hip
		green, //4  left_hip        ---     left_knee
		green, //5  left_knee       ---     left_ankle
		blue,  //6  pelvis          ---     thorax
		blue,  //7  thorax          ---     upper_neck
		blue,  //8  upper_neck      ---     head_top
		blue,  //9  right_elbow     ---     right_wrist
		blue,  //10 right_shoulder  ---     right_elbow
		blue,  //11 thorax          ---     right_shoulder
		green, //12 thorax          ---     left_shoulder
		green, //13 left_shoulder   ---     left_elbow
		green};//14 left_elbow      ---     left_wrist

	class DisplayJoint
	{
	public:
		//predictions, ground truth, waitKey(0) or waitKey(1), whether to show predictions, whether to show ground truth, visbility of joints, whether to display joint, where to save image output 
		void Display(Mat rgb, double *pred, double *real, int waitopt, int showpred, int showgt, int showimg, int *vis, int *inobj, char *outputname, const double *mul1, const double *mul2, const double *b1, const double *b2, const double *b3, char *directory);
		void DrawRing(Mat rgb, int joint_id, double *pred, double *real, int *vis, int *inobj, const double *mul1, const double *mul2, const double *b1, const double *b2, const double *b3);
	private:
		Point2d GetPoint(double slope, double stx, double sty, double newx);
		double Convert(double origin_value, const double mul1, const double mul2, const double b1, const double b2, const double b3);
		void DottedLine(Mat rgb, Point2d st, Point2d en, Scalar color, int thickness);		
	};
}