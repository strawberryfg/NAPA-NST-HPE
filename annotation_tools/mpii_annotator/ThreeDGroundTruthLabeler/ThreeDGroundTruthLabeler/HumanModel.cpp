#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "HumanModel.h"

namespace HumanModel
{
	void Model::Setup_ConstantMatrices()
	{		
		const_matr[right_ankle] = numeric::Matrix4d(trans_y, -bonelen[bone_knee_connect_ankle], false);
		const_matr[right_knee] = numeric::Matrix4d(trans_y, -bonelen[bone_hip_connect_knee], false);
		const_matr[right_hip] = numeric::Matrix4d(trans_x, -bonelen[bone_pelvis_connect_hip], false);
		const_matr[left_hip] = numeric::Matrix4d(trans_x, bonelen[bone_pelvis_connect_hip], false);
		const_matr[left_knee] = numeric::Matrix4d(trans_y, -bonelen[bone_hip_connect_knee], false);
		const_matr[left_ankle] = numeric::Matrix4d(trans_y, -bonelen[bone_knee_connect_ankle], false);
		const_matr[pelvis] = numeric::Matrix4d(trans_y, - bonelen[bone_thorax_connect_pelvis], false);
		const_matr[upper_neck] = numeric::Matrix4d(trans_y, bonelen[bone_thorax_connect_upper_neck], false);
		const_matr[head_top] = numeric::Matrix4d(trans_y, bonelen[bone_upper_neck_connect_head_top], false);
		const_matr[right_wrist] = numeric::Matrix4d(trans_y, -bonelen[bone_elbow_connect_wrist], false);
		const_matr[right_elbow] = numeric::Matrix4d(trans_y, -bonelen[bone_shoulder_connect_elbow], false);
		const_matr[right_shoulder] = numeric::Matrix4d(trans_x, -bonelen[bone_thorax_connect_shoulder], false);
		const_matr[left_shoulder] = numeric::Matrix4d(trans_x, bonelen[bone_thorax_connect_shoulder], false);
		const_matr[left_elbow] = numeric::Matrix4d(trans_y, -bonelen[bone_shoulder_connect_elbow], false);
		const_matr[left_wrist] = numeric::Matrix4d(trans_y, -bonelen[bone_elbow_connect_wrist], false);
	}

	void Model::Setup_Transformation()
	{
		//first clear is necessary
		for (int i = 0; i < JointNum; i++) Homo_mat[i].clear();

		//global center
		Homo_mat[thorax].pb(mp(trans_x, global_trans_x));
		Homo_mat[thorax].pb(mp(trans_y, global_trans_y));
		Homo_mat[thorax].pb(mp(trans_z, global_trans_z));
		Homo_mat[thorax].pb(mp(rot_z, global_rot_z));
		Homo_mat[thorax].pb(mp(rot_x, global_rot_x));
		Homo_mat[thorax].pb(mp(rot_y, global_rot_y));

		//upper_neck
		for (int i = 0; i < Homo_mat[thorax].size(); i++)
			Homo_mat[upper_neck].pb(Homo_mat[thorax][i]);
		Homo_mat[upper_neck].pb(mp(rot_z, upper_neck_rot_z));
		Homo_mat[upper_neck].pb(mp(rot_x, upper_neck_rot_x));
		Homo_mat[upper_neck].pb(mp(rot_y, upper_neck_rot_y));
		Homo_mat[upper_neck].pb(mp(Const_Matr, upper_neck));

		//head_top
		for (int i = 0; i < Homo_mat[upper_neck].size(); i++)
			Homo_mat[head_top].pb(Homo_mat[upper_neck][i]);		
		Homo_mat[head_top].pb(mp(rot_x, head_top_rot_x));				
		Homo_mat[head_top].pb(mp(Const_Matr, head_top));

		//pelvis
		for (int i = 0; i < Homo_mat[thorax].size(); i++)
			Homo_mat[pelvis].pb(Homo_mat[thorax][i]);
		Homo_mat[pelvis].pb(mp(Const_Matr, pelvis));
		Homo_mat[pelvis].pb(mp(rot_z, hip_rot_z));
		Homo_mat[pelvis].pb(mp(rot_y, hip_rot_y));
		// if it is after pelvis then it shouldn't be added first before left_hip or right_hip


		//left_shoulder & right_shoulder
		for (int i = 0; i < Homo_mat[thorax].size(); i++)
		{
			Homo_mat[right_shoulder].pb(Homo_mat[thorax][i]);
			Homo_mat[left_shoulder].pb(Homo_mat[thorax][i]);
		}		
		Homo_mat[right_shoulder].pb(mp(Const_Matr, right_shoulder));		
		Homo_mat[left_shoulder].pb(mp(Const_Matr, left_shoulder));
		//

		//left elbow & right elbow
		for (int i = 0; i < Homo_mat[right_shoulder].size(); i++) //left_shoulder.size  right_shoulder.size
		{
			Homo_mat[right_elbow].pb(Homo_mat[right_shoulder][i]);
			Homo_mat[left_elbow].pb(Homo_mat[left_shoulder][i]);
		}
		Homo_mat[right_elbow].pb(mp(rot_z, right_shoulder_rot_z));
		Homo_mat[right_elbow].pb(mp(rot_x, right_shoulder_rot_x));
		Homo_mat[right_elbow].pb(mp(rot_y, right_shoulder_rot_y));
		Homo_mat[right_elbow].pb(mp(Const_Matr, right_elbow));

		Homo_mat[left_elbow].pb(mp(rot_z, left_shoulder_rot_z));
		Homo_mat[left_elbow].pb(mp(rot_x, left_shoulder_rot_x));
		Homo_mat[left_elbow].pb(mp(rot_y, left_shoulder_rot_y));
		Homo_mat[left_elbow].pb(mp(Const_Matr, left_elbow));
		//

		//left wrist & right wrist
		for (int i = 0; i < Homo_mat[right_elbow].size(); i++)
		{
			Homo_mat[right_wrist].pb(Homo_mat[right_elbow][i]);
			Homo_mat[left_wrist].pb(Homo_mat[left_elbow][i]);
		}		
		Homo_mat[right_wrist].pb(mp(rot_x, right_elbow_rot_x));
		Homo_mat[right_wrist].pb(mp(Const_Matr, right_wrist));
		
		Homo_mat[left_wrist].pb(mp(rot_x, left_elbow_rot_x));
		Homo_mat[left_wrist].pb(mp(Const_Matr, left_wrist));
		//

		//left hip & right hip
		for (int i = 0; i < Homo_mat[pelvis].size(); i++)
		{
			Homo_mat[right_hip].pb(Homo_mat[pelvis][i]);
			Homo_mat[left_hip].pb(Homo_mat[pelvis][i]);
		}
		Homo_mat[right_hip].pb(mp(Const_Matr, right_hip));
		Homo_mat[left_hip].pb(mp(Const_Matr, left_hip));

		//left knee & right knee
		for (int i = 0; i < Homo_mat[right_hip].size(); i++)
		{
			Homo_mat[right_knee].pb(Homo_mat[right_hip][i]);
			Homo_mat[left_knee].pb(Homo_mat[left_hip][i]);
		}
		Homo_mat[right_knee].pb(mp(rot_z, right_hip_rot_z));
		Homo_mat[right_knee].pb(mp(rot_x, right_hip_rot_x));
		Homo_mat[right_knee].pb(mp(rot_y, right_hip_rot_y));
		Homo_mat[right_knee].pb(mp(Const_Matr, right_knee));

		Homo_mat[left_knee].pb(mp(rot_z, left_hip_rot_z));
		Homo_mat[left_knee].pb(mp(rot_x, left_hip_rot_x));
		Homo_mat[left_knee].pb(mp(rot_y, left_hip_rot_y));
		Homo_mat[left_knee].pb(mp(Const_Matr, left_knee));
		//

		//left ankle & right ankle
		for (int i = 0; i < Homo_mat[right_knee].size(); i++)
		{
			Homo_mat[right_ankle].pb(Homo_mat[right_knee][i]);
			Homo_mat[left_ankle].pb(Homo_mat[left_knee][i]);
		}		
		Homo_mat[right_ankle].pb(mp(rot_x, right_knee_rot_x));
		Homo_mat[right_ankle].pb(mp(Const_Matr, right_ankle));
		
		Homo_mat[left_ankle].pb(mp(rot_x, left_knee_rot_x));
		Homo_mat[left_ankle].pb(mp(Const_Matr, left_ankle));
		//
	}

	numeric::Matrix4d Model::GetMatrix(matrix_operation opt, int id, bool is_gradient, double *bottom_data)
	{
		return opt == Const_Matr ? const_matr[id] : numeric::Matrix4d(opt, islimited[id] ? initparam[id] : bottom_data[Bid + id] + initparam[id], is_gradient);
	}

	void Model::Forward(numeric::Matrix4d mat, int i, int prevsize, double *bottom_data)
	{
		for (int r = prevsize; r < Homo_mat[i].size(); r++) mat = mat * GetMatrix(Homo_mat[i][r].first, Homo_mat[i][r].second, false, bottom_data);
		prevmat[i] = mat;
		joint[i] = prevmat[i] * numeric::Vector4d(0.0, 0.0, 0.0, 1.0);
	}

	void Model::Forward_cpu(double *bottom_data)
	{
		for (int i = 0; i < JointNum; i++)
		{
			int id = forward_seq[i];
			numeric::Matrix4d mat;
			if (prev_seq[i] != -1) mat = prevmat[prev_seq[i]];
			Forward(mat, id, prev_seq[i] == -1 ? 0 : Homo_mat[prev_seq[i]].size(), bottom_data);
		}
		for (int i = 0; i < JointNum; i++) for (int j = 0; j < 3; j++) top_data[i * 3 + j] = joint[i][j];
	}

	void Model::Backward(int i, double *bottom_data)
	{
		std::vector<std::pair<matrix_operation, int> > mat = Homo_mat[i];
		numeric::Matrix4d m_left[ParamNum * 3];
		numeric::Vector4d v_right[ParamNum * 3];
		v_right[mat.size() - 1] = numeric::Vector4d(0.0, 0.0, 0.0, 1.0);
		for (int r = mat.size() - 2; r >= 0; r--) v_right[r] = GetMatrix(mat[r + 1].first, mat[r + 1].second, false, bottom_data) * v_right[r + 1];
		m_left[0] = numeric::Matrix4d(); //Identity matrix
		for (int r = 1; r < mat.size(); r++) m_left[r] = m_left[r - 1] * GetMatrix(mat[r - 1].first, mat[r - 1].second, false, bottom_data);
		for (int r = 0; r < mat.size(); r++) if (mat[r].first != Const_Matr) f[i][mat[r].second] = islimited[mat[r].second] ? numeric::Vector4d(0.0, 0.0, 0.0, 1.0) : m_left[r] * GetMatrix(mat[r].first, mat[r].second, true, bottom_data) * v_right[r];
	}

	void Model::Backward_cpu(double *bottom_data, double *top_diff, int iter)
	{
		for (int i = 0; i < JointNum; i++)
		{
			for (int j = 0; j < ParamNum; j++)
			{
				f[i][j].x[0] = f[i][j].x[1] = f[i][j].x[2] = f[i][j].x[3] = 0.0; //crucial
			}
			Backward(i, bottom_data);
		}
		for (int j = 0; j < ParamNum; j++)
		{
			bottom_diff[Bid + j] = 0;
			for (int i = 0; i < JointNum; i++)
			{
				int Tid = i * 3;			
				double t = 0.0;
				for (int k = 0; k < 3; k++) bottom_diff[Bid + j] += f[i][j][k] * top_diff[Tid + k];
			}
		}
	}
}
