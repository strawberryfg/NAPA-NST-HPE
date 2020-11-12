#include "WeakPerspective.h"
#define use_angle
namespace HumanModel
{
	void WeakPerspective::Forward_cpu(double *bottom_data, double *m_rot, double *t_trans, double s, double *angle)
	{
		//angle[0] angle[1] angle[2]  means global_rot_x global_rot_y global_rot_z here and    z x y matrix first two rows mean m_rot[0] m_rot[1] m_rot[2] m_rot[3] m_rot[4] m_rot[5]
		m_rot[0] = 1.0;
		m_rot[1]=0.0;
		m_rot[2]=0.0;
		m_rot[3]=0.0;
		m_rot[4]=1.0;
		m_rot[5]=0.0;
#ifdef use_angle
		m_rot[0] = cos(angle[0]) * cos(angle[2]) + sin(angle[0]) * sin(angle[1]) * sin(angle[2]);
		m_rot[1] = -sin(angle[0]) * cos(angle[1]);
		m_rot[2] = -cos(angle[0]) * sin(angle[2]) + sin(angle[0]) * sin(angle[1]) * cos(angle[2]);
		m_rot[3] = sin(angle[0]) * cos(angle[2]) - cos(angle[0]) * sin(angle[1]) * sin(angle[2]);
		m_rot[4] = cos(angle[0]) * cos(angle[1]);
		m_rot[5] = -sin(angle[0]) * sin(angle[2]) - cos(angle[0]) * sin(angle[1]) * cos(angle[2]);
#endif
		double fac = 1.0 / 1.0;
		for (int i = 0; i < JointNum; i++)
		{
			int Botid = i * 3, Topid = i * 2;
			double x = bottom_data[Botid], y = bottom_data[Botid + 1], z = bottom_data[Botid + 2];
			top_data[Topid] = fac * (s * (m_rot[0] * x + m_rot[1] * y + m_rot[2] * z) + t_trans[0]);
			top_data[Topid + 1] = fac * (s * (m_rot[3] * x + m_rot[4] * y + m_rot[5] * z) + t_trans[1]);			
		}
	}

	void WeakPerspective::Backward_cpu(double *bottom_data, double *top_diff, double *m_rot, double *t_trans, double s, double *angle)
	{		
		m_rot[0] = 1.0;
		m_rot[1]=0.0;
		m_rot[2] = 0.0;
		m_rot[3] = 0.0;
		m_rot[4] = 1.0;
		m_rot[5] = 0.0;
#ifdef use_angle
		m_rot[0] = cos(angle[0]) * cos(angle[2]) + sin(angle[0]) * sin(angle[1]) * sin(angle[2]);
		m_rot[1] = -sin(angle[0]) * cos(angle[1]);
		m_rot[2] = -cos(angle[0]) * sin(angle[2]) + sin(angle[0]) * sin(angle[1]) * cos(angle[2]);
		m_rot[3] = sin(angle[0]) * cos(angle[2]) - cos(angle[0]) * sin(angle[1]) * sin(angle[2]);
		m_rot[4] = cos(angle[0]) * cos(angle[1]);
		m_rot[5] = -sin(angle[0]) * sin(angle[2]) - cos(angle[0]) * sin(angle[1]) * cos(angle[2]);
#endif
		double fac = 1.0 / 1.0;
		t_trans_diff[0] = t_trans_diff[1] = 0.0;
		s_diff = 0.0;
		for (int i = 0; i < 6; i++) m_rot_diff[i] = 0.0;
		for (int i = 0; i < 3; i++) angle_diff[i] = 0.0;
		for (int i = 0; i < JointNum; i++)
		{
			int Botid = i * 3, Topid = i * 2;
			double x = bottom_data[Botid], y = bottom_data[Botid + 1], z = bottom_data[Botid + 2];
			bottom_diff[Botid] =  fac * (top_diff[Topid] * s * m_rot[0] + top_diff[Topid + 1] * s * m_rot[3]);
			bottom_diff[Botid + 1] =  fac * (top_diff[Topid] * s * m_rot[1] + top_diff[Topid + 1] * s * m_rot[4]);
			bottom_diff[Botid + 2] =  fac * (top_diff[Topid] * s * m_rot[2] + top_diff[Topid + 1] * s * m_rot[5]);
			s_diff += fac * (top_diff[Topid] * (m_rot[0] * x + m_rot[1] * y + m_rot[2] * z) + top_diff[Topid + 1] * (m_rot[3] * x + m_rot[4] * y + m_rot[5] * z));
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					m_rot_diff[k * 3 + j] += fac * (top_diff[Topid + k] * s * bottom_data[Botid + j]) * (k==1?1.0:1.0);
				}
			}
			t_trans_diff[0] += fac * top_diff[Topid];
			t_trans_diff[1] += fac * top_diff[Topid + 1];
		}
		
		angle_diff[0] += m_rot_diff[0] * (-sin(angle[0]) * cos(angle[2]) + cos(angle[0]) * sin(angle[1]) * sin(angle[2]));
		angle_diff[1] += m_rot_diff[0] * (sin(angle[0]) * sin(angle[2]) * cos(angle[1]));
		angle_diff[2] += m_rot_diff[0] * (cos(angle[0]) * (-sin(angle[2])) + sin(angle[0]) * sin(angle[1]) * cos(angle[2]));
		angle_diff[0] += m_rot_diff[1] * (-cos(angle[0]) * cos(angle[1]));
		angle_diff[1] += m_rot_diff[1] * (-sin(angle[0]) * -sin(angle[1]));
		angle_diff[0] += m_rot_diff[2] * (sin(angle[0]) * sin(angle[2]) + sin(angle[1]) * cos(angle[2]) * cos(angle[0]));
		angle_diff[1] += m_rot_diff[2] * (sin(angle[0]) * cos(angle[1]) * cos(angle[2]));
		angle_diff[2] += m_rot_diff[2] * (-cos(angle[0]) * cos(angle[2]) + sin(angle[0]) * sin(angle[1]) * -sin(angle[2]));
		angle_diff[0] += m_rot_diff[3] * (cos(angle[0]) * cos(angle[2]) + sin(angle[0]) * sin(angle[1]) * sin(angle[2]));
		angle_diff[1] += m_rot_diff[3] * (-cos(angle[0]) * cos(angle[1]) * sin(angle[2]));
		angle_diff[2] += m_rot_diff[3] * (sin(angle[0]) * -sin(angle[2]) - cos(angle[0]) * sin(angle[1]) * cos(angle[2]));
		angle_diff[0] += m_rot_diff[4] * (-sin(angle[0]) * cos(angle[1]));
		angle_diff[1] += m_rot_diff[4] * (cos(angle[0]) * -sin(angle[1]));
		angle_diff[0] += m_rot_diff[5] * (-cos(angle[0]) * sin(angle[2]) + sin(angle[0]) * sin(angle[1]) * cos(angle[2]));
		angle_diff[1] += m_rot_diff[5] * (-cos(angle[0]) * cos(angle[1]) * cos(angle[2]));
		angle_diff[2] += m_rot_diff[5] * (-sin(angle[0]) * cos(angle[2]) - cos(angle[0]) * sin(angle[1]) * -sin(angle[2]));
		
	}
}