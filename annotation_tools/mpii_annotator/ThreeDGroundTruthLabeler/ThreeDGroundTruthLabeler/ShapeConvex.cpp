#include "ShapeConvex.h"
MatrixXd t3(384, 384);
void loadshapedict(char *directory)
{
	char Bname[maxlen];
	sprintf(Bname, "%s%s", directory, "\\configuration\\B.in", "r");
	FILE *fin_B = fopen(Bname, "r");
	for (int i = 0; i < 384; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			fscanf(fin_B, "%lf", &B[i][j]);
		}
	}
	fclose(fin_B);
	char muname[maxlen];
	sprintf(muname, "%s%s", directory, "\\configuration\\mu.in", "r");
	FILE *fin_mu = fopen(muname, "r");
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			fscanf(fin_mu, "%lf", &all_mu[i][j]);
		}
	}
	fclose(fin_mu);
}
void normalize_xy()
{
#ifdef dividestd
	double all_std_deviation = 0.0;
	for (int i = 0; i < 2; i++)
	{
		double avg = 0.0;
		for (int j = 0; j < 15; j++)
		{
			avg += xy[i][j];
		}
		avg /= 15.0;

		double std_deviation = 0.0;
		for (int j = 0; j < 15; j++)
		{
			std_deviation += pow(xy[i][j] - avg, 2);
		}
		std_deviation = sqrt(std_deviation / 15.0);
		for (int j = 0; j < 15; j++)
		{
			xy[i][j] -= avg;
		}
		all_std_deviation += std_deviation;
	}
	all_std_deviation /= 2.0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 15; j++)
			xy[i][j] /= all_std_deviation;
	}
	//subtract mean and divided by standard deviation
#else
	double fmax[2], fmin[2];
	for (int i = 0; i < 2; i++)
	{
		fmax[i] = -111.0;
		fmin[i] = 111.0;
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			fmin[i] = min(fmin[i], xy[i][j]);
			fmax[i] = max(fmax[i], xy[i][j]);
		}
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			xy[i][j] = (xy[i][j] - fmin[i]) / (fmax[i] - fmin[i]);
		}
	}
	//normalize into [0,1]
#endif
}
void centralize_basis()  //subtract each row of B by its mean
{
	for (int i = 0; i < 384; i++)
	{
		double avg = 0.0;
		for (int j = 0; j < 15; j++)
		{
			avg += B[i][j];
		}
		avg /= 15.0;
		for (int j = 0; j < 15; j++)
		{
			B[i][j] -= avg; //subtract by its mean
		}
	}
}
MatrixXd prox_2norm(MatrixXd Z, double lam, double *normX)
{
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(Z, Eigen::ComputeFullU | Eigen::ComputeFullV);	
	MatrixXd U = svd.matrixU();
	MatrixXd V = svd.matrixV();	
	double w[2];
	w[0] = svd.singularValues()[0];
	w[1] = svd.singularValues()[1];
	if (w[0] + w[1] <= lam)
	{
		w[0] = w[1] = 0.0;
	}
	else if (w[0] - w[1] <= lam)
	{
		w[0] = (w[0] + w[1] - lam) / 2.0;
		w[1] = w[0];
	}
	else
	{
		w[0] = w[0] - lam;
		w[1] = w[1];
	}
	MatrixXd X(2, 3);
	MatrixXd diagw(2, 2);
	diagw(0, 1) = diagw(1, 0) = 0.0;
	diagw(0, 0) = w[0]; diagw(1, 1) = w[1];
	X = U * diagw * V.transpose();	
	*normX = w[0];
	return X;
}

void cross_product(double *a, double *b, double *c)
{
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];
}
void ssr2D3D_alm(double lam, double beta, int id)
{
	double tol = 1e-3;
	bool refine = false;
	bool verb = true;
	centralize_basis(); //centralize B
	int k = 128; //128 basis
	int p = 3;
	double M[2][384]; //product of ci and first two rows of Ri i=0..383 
	memset(M, 0, sizeof(M));
	double C[128]; //weight of each basis
	memset(C, 0, sizeof(C));
	double E[2][15]; // the same shape as W(projection)
	memset(E, 0, sizeof(E));
	double T[2];
	for (int i = 0; i < 2; i++)
	{
		T[i] = 0.0;
		for (int j = 0; j < 15; j++)
		{
			T[i] += xy[i][j];
		}
		T[i] /= 15.0;
	} //T mean(W,2)	
	double Z[2][384]; //auxiliary variable
	double Y[2][384];
	memcpy(Z, M, sizeof(M));
	memcpy(Y, M, sizeof(M));
	double avgabsxy = 0.0, maxabsxy = 0.0;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			avgabsxy += abs(xy[i][j]);
			maxabsxy = max(maxabsxy, xy[i][j]);
		}
	}
	avgabsxy /= (2.0*15.0); //mean
	double mu = 1.0 / avgabsxy;

	memset(BBt, 0, sizeof(BBt));
	for (int i = 0; i < 384; i++)
	{
		for (int j = 0; j < 384; j++)
		{
			for (int k = 0; k < 15; k++)
			{
				BBt[i][j] += B[i][k] * B[j][k]; //B[i][k] * B'[k][j]
			}
		}
	}
	bool flagOD = false;
	if (beta > maxabsxy)
	{
		flagOD = false;
	}
	else
	{
		flagOD = true;
	}
	for (int iter = 0; iter < 100; iter++)
	{		
		double Z0[2][384];
		memcpy(Z0, Z, sizeof(Z));
		double t1[2][15];
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 15; j++)
			{
				t1[i][j] = xy[i][j] - E[i][j] - T[i];
			}
		}
		double t2[2][384];  //(W-E-T*ones(1,p))*B' B' 15 * 384 
		memset(t2, 0, sizeof(t2));
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 384; j++)
			{
				for (int k = 0; k < 15; k++)
				{
					t2[i][j] += t1[i][k] * B[j][k]; // B'[k][j] is B[j][k]
				}
			}
		}
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 384; j++)
			{
				t2[i][j] = t2[i][j] + mu*M[i][j] + Y[i][j];
			}
		}// +mu*M+Y

		for (int i = 0; i < 384; i++)
		{
			for (int j = 0; j < 384; j++)
			{
				t3(i, j) = BBt[i][j] + mu *	(i == j);
			}
		}
		t3 = t3.inverse();
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 384; j++)
			{
				Z[i][j] = 0.0;
				for (int k = 0; k < 384; k++)
				{
					Z[i][j] += t2[i][k] * t3(k, j);
				}
			}
		}

		double Q[2][384];
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 384; j++)
			{
				Q[i][j] = Z[i][j] - Y[i][j] / mu;
			}
		}
		MatrixXd t_Q(2, 3);
		for (int i = 0; i < 128; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					t_Q(j, k) = Q[j][i * 3 + k];
				}
			}
			MatrixXd t_mat = prox_2norm(t_Q, lam / mu, C + i);
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					M[j][3 * i + k] = t_mat(j, k);
				}
			}
		}
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 384; j++)
			{
				Y[i][j] = Y[i][j] + mu*(M[i][j] - Z[i][j]);
			}
		}
		MatrixXd dividend(2, 384);
		double dividend_norm = 0.0;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 384; j++)
			{
				dividend(i, j) = M[i][j] - Z[i][j];
				dividend_norm += pow(dividend(i, j), 2);
			}
		}
		dividend_norm = sqrt(dividend_norm);
		MatrixXd divisor(2, 384);
		double divisor_norm = 0.0;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 384; j++)
			{
				divisor(i, j) = Z0[i][j];
				divisor_norm += pow(divisor(i, j), 2);
			}
		}
		divisor_norm = sqrt(divisor_norm);
		double primres = dividend_norm / divisor_norm;
		//double primres = frobenuis_norm(dividend, 2, 384) / frobenuis_norm(divisor, 2, 384);
		dividend_norm = 0.0;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 384; j++)
			{
				dividend(i, j) = Z[i][j] - Z0[i][j];
				dividend_norm += pow(dividend(i, j), 2);
			}
		}
		dividend_norm = sqrt(dividend_norm);
		double dualres = mu * dividend_norm / divisor_norm;
		//cout << primres <<" "<<dualres<<" "<<mu<< "\n";
		//double dualres = mu*frobenuis_norm(dividend, 2, 384) / frobenuis_norm(divisor, 2, 384);
		if (primres < tol && dualres < tol) //convergent
			break;
		else
		{
			if (primres > 10.0 * dualres) mu = 2 * mu;
			else if (dualres>10.0*primres) mu = mu / 2.0;
		}
	}
	double R[3][384];

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 384; j++)
			R[i][j] = 0.0;
	}
	for (int i = 0; i < 128; i++)
	{
		if (C[i]>1e-6)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					R[j][3 * i + k] = M[j][3 * i + k] / C[i];
				}
			}
			cross_product(&R[0][3 * i], &R[1][3 * i], &R[2][3 * i]);
		}
	}

	for (int i = 0; i < 128; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				for (int l = 0; l < 3; l++)
				{
					intermediate[3 * i + k][3 * j + l] = (i == j) * C[i] * (k == l);
				}
			}
		}
	}
	double Rmulint[3][384];
	memset(Rmulint, 0, sizeof(Rmulint));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 384; j++)
		{
			for (int k = 0; k < 384; k++)
			{
				Rmulint[i][j] += R[i][k] * intermediate[k][j];
			}
		}
	}
	double S[3][15];
	memset(S, 0, sizeof(S));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			for (int k = 0; k < 384; k++)
			{
				S[i][j] += Rmulint[i][k] * B[k][j];
			}
		}
	}	
	int std2shapeconvex[JointNum];
	std2shapeconvex[right_ankle] = 6;      //right_ankle --> right_tibia
	std2shapeconvex[right_knee] = 5;       //right_knee  --> right_femur
	std2shapeconvex[right_hip] = 4;        //right_hip
	std2shapeconvex[left_hip] = 1;         //left_hip
	std2shapeconvex[left_knee] = 2;        //left_knee   --> left_femur
	std2shapeconvex[left_ankle] = 3;       //left_ankle  --> left_tibia
	std2shapeconvex[pelvis] = 0;           //pelvis      --> root
	std2shapeconvex[thorax] = 7;           //thorax
	std2shapeconvex[head_top] = 8;         //head_top
	std2shapeconvex[right_wrist] = 14;     //right_wrist --> right_hand
	std2shapeconvex[right_elbow] = 13;     //right_elbow --> right_humerus
	std2shapeconvex[right_shoulder] = 12;  //right_shoulder --> right_claivcle
	std2shapeconvex[left_shoulder] = 9;    //left_shoulder --> left_claivcle
	std2shapeconvex[left_elbow] = 10;      //left_elbow --> left_humerus
	std2shapeconvex[left_wrist] = 11;      //left_wrist --> left_hand
	std2shapeconvex[upper_neck] = 7;
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			shape_convex_joint[i * 3 + j] = (j >= 1 ? -1.0 : 1.0)* S[j][std2shapeconvex[i]];			
		}		
	}	
	printf("Shape Convex %7d Done", id);
}
void ssr2D3D_wrapper(int method, int id)
{
	double lam = 1.0;
	double beta = 1.0;
	bool verb = false;
	switch (method)
	{
	case convex:
		ssr2D3D_alm(lam, inf, id);
		break;
	default:
		break;
	}
}
double calc_bone_len(double *endpoint_a, double *endpoint_b)
{
	double ret = 0.0;
	for (int i = 0; i < 3; i++)
	{
		ret += pow(endpoint_a[i] - endpoint_b[i], 2);
	}
	ret = sqrt(ret);
	return ret;
}

void prepare_2d_landmark(InitConfiguration t_init, int id)
{
	double *t_gt = normalize_gt_2d(t_init.jointgt, t_init.obj); //normalize to [0,1] in image crop window
	int std2shapeconvex[JointNum];
	memset(std2shapeconvex, 0, sizeof(std2shapeconvex));
	std2shapeconvex[right_ankle] = 6;      //right_ankle --> right_tibia
	std2shapeconvex[right_knee] = 5;       //right_knee  --> right_femur
	std2shapeconvex[right_hip] = 4;        //right_hip
	std2shapeconvex[left_hip] = 1;         //left_hip
	std2shapeconvex[left_knee] = 2;        //left_knee   --> left_femur
	std2shapeconvex[left_ankle] = 3;       //left_ankle  --> left_tibia
	std2shapeconvex[pelvis] = 0;           //pelvis      --> root
	std2shapeconvex[thorax] = 7;           //thorax
	std2shapeconvex[head_top] = 8;         //head_top
	std2shapeconvex[right_wrist] = 14;     //right_wrist --> right_hand
	std2shapeconvex[right_elbow] = 13;     //right_elbow --> right_humerus
	std2shapeconvex[right_shoulder] = 12;  //right_shoulder --> right_claivcle
	std2shapeconvex[left_shoulder] = 9;    //left_shoulder --> left_claivcle
	std2shapeconvex[left_elbow] = 10;      //left_elbow --> left_humerus
	std2shapeconvex[left_wrist] = 11;      //left_wrist --> left_hand
	for (int i = 0; i < JointNum; i++)
	{
		if (i == upper_neck) continue; //there is no suitable counterpart for upper_neck in the MPII definition
		xy[0][std2shapeconvex[i]] = t_gt[i * 2];
		xy[1][std2shapeconvex[i]] = t_gt[i * 2 + 1];
	}
#ifdef need_normalize
	normalize_xy();
#endif
	ssr2D3D_wrapper(convex, id); //after shape convex we can finally fit a kinematic human model to the shape convex result 
}

void FitModelToShapeConvex()
{
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
	//First set the bone length
	Model shape_convex_obj;	
	shape_convex_obj.bonelen[bone_knee_connect_ankle] = 
		                          (calc_bone_len(shape_convex_joint + index(right_ankle), shape_convex_joint + index(right_knee)) +
		                           calc_bone_len(shape_convex_joint + index(left_ankle), shape_convex_joint + index(left_knee))) / 2.0;
	shape_convex_obj.bonelen[bone_hip_connect_knee] =
		                          (calc_bone_len(shape_convex_joint + index(right_hip), shape_convex_joint + index(right_knee)) +
		                           calc_bone_len(shape_convex_joint + index(left_hip), shape_convex_joint + index(left_knee))) / 2.0;
	shape_convex_obj.bonelen[bone_pelvis_connect_hip] =
	                  	          (calc_bone_len(shape_convex_joint + index(pelvis), shape_convex_joint + index(right_hip)) +
		                           calc_bone_len(shape_convex_joint + index(pelvis), shape_convex_joint + index(left_hip))) / 2.0;
	shape_convex_obj.bonelen[bone_thorax_connect_pelvis] =
		                           calc_bone_len(shape_convex_joint + index(thorax), shape_convex_joint + index(pelvis));
	shape_convex_obj.bonelen[bone_thorax_connect_upper_neck] =
		                           calc_bone_len(shape_convex_joint + index(thorax), shape_convex_joint + index(upper_neck));
	shape_convex_obj.bonelen[bone_upper_neck_connect_head_top] =
		                           calc_bone_len(shape_convex_joint + index(upper_neck), shape_convex_joint + index(head_top));
	shape_convex_obj.bonelen[bone_elbow_connect_wrist] =
		                          (calc_bone_len(shape_convex_joint + index(right_elbow), shape_convex_joint + index(right_wrist)) +
		                           calc_bone_len(shape_convex_joint + index(left_elbow), shape_convex_joint + index(left_wrist))) / 2.0;
	shape_convex_obj.bonelen[bone_shoulder_connect_elbow] =
		                          (calc_bone_len(shape_convex_joint + index(right_shoulder), shape_convex_joint + index(right_elbow)) +
		                           calc_bone_len(shape_convex_joint + index(left_shoulder), shape_convex_joint + index(left_elbow))) / 2.0;
	shape_convex_obj.bonelen[bone_thorax_connect_shoulder] =
		                          (calc_bone_len(shape_convex_joint + index(thorax), shape_convex_joint + index(right_shoulder)) +
		                           calc_bone_len(shape_convex_joint + index(thorax), shape_convex_joint + index(left_shoulder))) / 2.0;	
	//Then set up shape_convex_obj
	shape_convex_obj.Setup_ConstantMatrices();
	shape_convex_obj.Setup_Transformation();
	for (int i = 0; i < ParamNum; i++) shape_convex_obj.initparam[i] = 0.0; //global z = 0 works fine
	for (int i = 0; i < JointNum; i++) shape_convex_obj.inobj[i] = 1;
	int t_visible[JointNum]; //as a parameter to pass into 
	for (int i = 0; i < JointNum; i++) t_visible[i] = 1;
	//Now it's ready to use gradient descent to fit a 3D model to the result of Shape Convex
	double t_param[ParamNum];
	for (int i = 0; i < ParamNum; i++) t_param[i] = 0.0;
	Mat t_rgb;
	GradientDescent_3d_pinhole t_grad;	
	t_grad.Gradscnt(t_param, shape_convex_joint, shape_convex_joint, t_visible, shape_convex_obj, t_rgb, times, directory, false, false);  //optimize 3d no 2d at all
	for (int i = 3; i < ParamNum; i++) shape_convex_param[i] = t_grad.bestparam[i];	
} 