#include <iostream>
#include "PoseConditionedJointAngleLimit_define.h"
bool find_in_torso(int id)
{
	for (int i = 0; i < torso_bone_num; i++) if (id == torso_bone[i]) return true;
	return false;
}

double *getNormal(double *x1, double *a, double *x) //calculate Ru¡Áa,u
{
	double *res = (double *)malloc(sizeof(double)* 3);
	if (norm(calc_sub(x, a)) < eps || norm(calc_add(x, a)) < eps)
	{
		res = cross(x, x1);
		//if R*a is parallel to u then the cross product of them could be zero which 
		//will result in a zero vector  so use R*u
		//I think there might also be a small chance that x1 is parallel to x
	}
	else
	{
		res = cross(a, x); //cross product of R*a and u
	}
	normalize(res);
	return res;
}

double *gramschimidt(double *u, double *v, double *w)  //three 3-d vector
{
	double *B = (double *)malloc(sizeof(double)* 9);
	double t_norm = norm(u);
	//first orthogonal basis vector
	B[0] = u[0] / t_norm;
	B[3] = u[1] / t_norm;
	B[6] = u[2] / t_norm;   //1/||n1|| * n1
	// [[B[0] B[3] B[6]] [v0
	//                    v1
	//					  v2]

	//second orthogonal basis vector
	double t = B[0] * v[0] + B[3] * v[1] + B[6] * v[2];
	double *t_vec = (double *)malloc(sizeof(double)* 3);
	t_vec[0] = v[0] - B[0] * t;
	t_vec[1] = v[1] - B[3] * t;
	t_vec[2] = v[2] - B[6] * t;  //v2-n1 (n1^T v2)	
	normalize(t_vec);
	B[1] = t_vec[0];
	B[4] = t_vec[1];
	B[7] = t_vec[2];

	//third orthogonal basis vector
	double t_0 = B[0] * w[0] + B[3] * w[1] + B[6] * w[2];
	double t_1 = B[1] * w[0] + B[4] * w[1] + B[7] * w[2];
	t_vec[0] = w[0] - (B[0] * t_0 + B[1] * t_1);
	t_vec[1] = w[1] - (B[3] * t_0 + B[4] * t_1);
	t_vec[2] = w[2] - (B[6] * t_0 + B[7] * t_1);
	normalize(t_vec);
	B[2] = t_vec[0];
	B[5] = t_vec[1];
	B[8] = t_vec[2];
	return B;
}

double *cart2sph(double *x, double *y, double *z)
{
	double *res = (double *)malloc(sizeof(double)* 3);
	double radius = sqrt(pow(*x, 2) + pow(*y, 2) + pow(*z, 2));
	double elevation = atan2(*z, sqrt(pow(*x, 2) + pow(*y, 2)));
	double azimuth = atan2(*y, *x);
	res[0] = azimuth;
	res[1] = elevation;
	res[2] = radius;
	return res;
}

double *sph2cart(double azimuth, double elevation, double radius)
{
	double *ret = (double *)malloc(sizeof(double)* 3);
	double z = radius * sin(elevation);
	double rcoselev = radius * cos(elevation);
	double x = rcoselev * cos(azimuth);
	double y = rcoselev * sin(azimuth);
	ret[0] = x;
	ret[1] = y;
	ret[2] = z;
	return ret;
}

double *estimateJoint(double *delta, double *base)
{
	double *res = (double *)malloc(sizeof(double)* 3 * joint_num);
	copy(res, base);
	for (int i = 0; i < bone_num; i++)
	{
		copy(res + index(edges_pjcal[i][1]), res + index(edges_pjcal[i][0]));
		subtract(res + index(edges_pjcal[i][1]), delta + index(i));
	}
	return res;
}