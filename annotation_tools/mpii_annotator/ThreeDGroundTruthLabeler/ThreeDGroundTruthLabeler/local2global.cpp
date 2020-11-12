#include "local2global.h"
double *local2global(double *dJoint)
{
	double *shoulder = (double *)malloc(sizeof(double)* 3);
	copy(shoulder, dJoint + index(4));
	subtract(shoulder, dJoint + index(1)); //dJoint[4][]-dJoint[1][]	
	double *hip = (double *)malloc(sizeof(double)* 3);
	copy(hip, dJoint + index(12));
	subtract(hip, dJoint + index(8));
	double *u = (double *)malloc(sizeof(double)* 3);
	double *v = (double *)malloc(sizeof(double)* 3), *w = (double *)malloc(sizeof(double)* 3);
	double *R = (double *)malloc(sizeof(double)* 9); //rotation matrix of parent bone
	double *t_local = (double *)malloc(sizeof(double)* bone_num * 3);
	for (int i = 0; i < torso_bone_num; i++)
	{
		copy(t_local + index(torso[i]), dJoint + index(torso[i]));
	}
	for (int i = 0; i < parent_num; i++)
	{
		if (find_in_torso(children_bone[i]))
		{
			if (i == 0 || i == 2 || i == 4) //2 5 7 index of edges r_shoulder->neck l_shoulder->neck neck->face
				copy(u, shoulder);
			else
				copy(u, hip);
			normalize(u);
			copy(v, t_local); //first belly->neck back_bone 
			normalize(v);
		}
		else
		{
			copy(u, t_local + index(parent_bone[i])); //u=dJoint(:,parent_bone[i]);
			normalize(u);
			v = getNormal(calc_mat3x3Xvec3(R, (double *)d_static_Joint + index(parent_bone[i])), calc_mat3x3Xvec3(R, (double *)static_a), u);
		}
		w = cross(u, v);
		normalize(w);
		R = gramschimidt(u, v, w); //get the rotation matrix of this current bone
		double *tmp = calc_mat3x3Xvec3(R, dJoint + index(children_bone[i])); //R * current_bone
		copy(t_local + index(children_bone[i]), tmp);
	}
	return t_local;
}