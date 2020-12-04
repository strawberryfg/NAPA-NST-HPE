#include <iostream>
#include "basic.h"
double rad2deg(double rad)
{
	return 180.0 * rad / PI;
}

double deg2rad(double deg)
{
	return deg / 180.0 * PI;
}

double *concat_spherelocation(double *spherex, double *spherey, double *spherez)
{
	double *ret = (double *)malloc(sizeof(double) * JointNum * 3);
	for (int i = 0; i < JointNum; i++)
	{
		ret[3 * i] = spherex[i] / mul_factor(directory);
		ret[3 * i + 1] = spherey[i] / mul_factor(directory);
		ret[3 * i + 2] = spherez[i] / mul_factor(directory);
	}
	return ret;
}



int sgn(double x)
{
	if (x>epss) return 1;
	else if (x < -epss) return -1;
	return 0;
}



bool isnan(double x)
{
	if (sgn(fabs(x + 2147483648)) == 0) return true; else return false;
}

void copy(double *a, double *b)
{
	for (int i = 0; i < 3; i++) a[i] = b[i];
}

void subtract(double *a, double *b)
{
	for (int i = 0; i < 3; i++) a[i] -= b[i];
}

void addition(double *a, double *b)
{
	for (int i = 0; i < 3; i++) a[i] += b[i];
}

void divide_single_num(double *a, double b)
{
	for (int i = 0; i < 3; i++) a[i] /= b;
}

double *mul_single_num(double *a, double b)
{
	double *ret = (double *)malloc(sizeof(double)* 3);
	for (int i = 0; i < 3; i++) ret[i] = a[i] * b;
	return ret;
}

double *calc_add(double *a, double *b)
{
	double *res = (double *)malloc(sizeof(double)* 3);
	for (int i = 0; i < 3; i++) res[i] = a[i] + b[i];
	return res;
}

double *calc_sub(double *a, double *b)
{
	double *res = (double *)malloc(sizeof(double)* 3);
	for (int i = 0; i < 3; i++) res[i] = a[i] - b[i];
	return res;
}

double *calc_mat3x3Xvec3(double *a, double *b) //3*3 matrix' multiply 3-d vec
{
	double *res = (double *)malloc(sizeof(double)* 3);
	res[0] = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	res[1] = a[3] * b[0] + a[4] * b[1] + a[5] * b[2];
	res[2] = a[6] * b[0] + a[7] * b[1] + a[8] * b[2];
	return res;
}

double *calc_mat3x3transposeXvec3(double *a, double *b) //3*3 matrix's transpose multiply 3-d vec
{
	double *res = (double *)malloc(sizeof(double)* 3);
	res[0] = a[0] * b[0] + a[3] * b[1] + a[6] * b[2];
	res[1] = a[1] * b[0] + a[4] * b[1] + a[7] * b[2];
	res[2] = a[2] * b[0] + a[5] * b[1] + a[8] * b[2];
	return res;
}

double calc_vec4Xvec4(double *a, double *b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

double *calc_mat3x3last2coltransposeXvec3(double *a, double *b) //a is 3*3 but we calculate a(:,2:3)' * b
{
	double *res = (double *)malloc(sizeof(double)* 3);
	res[0] = a[1] * b[0] + a[4] * b[1] + a[7] * b[2];
	res[1] = a[2] * b[0] + a[5] * b[1] + a[8] * b[2];
	return res;
}

double *calc_mat3x3last2colXvec2(double *a, double *b) //a(:,2:3) * b
{
	double *res = (double *)malloc(sizeof(double)* 3);
	res[0] = a[1] * b[0] + a[2] * b[1];
	res[1] = a[4] * b[0] + a[5] * b[1];
	res[2] = a[7] * b[0] + a[8] * b[1];
	return res;
}

double norm(double *a)
{
	double t_norm = 0.0;
	for (int i = 0; i < 3; i++) t_norm += pow(a[i], 2);
	t_norm = sqrt(t_norm);
	return t_norm;
}

double *cross(double *a, double *b)
{
	double *res = (double *)malloc(sizeof(double)* 3);
	res[0] = a[1] * b[2] - a[2] * b[1]; //a2b3-a3b2
	res[1] = a[2] * b[0] - a[0] * b[2]; //b1a3-a1b3
	res[2] = a[0] * b[1] - a[1] * b[0]; //a1b2-a2b1
	return res;
}

void normalize(double *a)
{
	double t_norm = norm(a);
	for (int i = 0; i < 3; i++) a[i] /= t_norm;
}

int index(int x)
{
	return x * 3;
}

bool has_difference(double *comp_0, double *comp_1, int arr_len)
{
	//if true : has difference
	for (int i = 0; i < arr_len; i++)
	{
		if (sgn(comp_0[i] - comp_1[i])) return true;  //!=0   means not equal		
	}
	return false; //array comp_0 is different from comp_1
}

double shift_ratio(char *directory)
{
	char shift_rationame[maxlen];
	sprintf(shift_rationame, "%s%s", directory, "\\configuration\\shift_ratio.in");
	FILE *fin_shiftratio = fopen(shift_rationame, "r");
	if (fin_shiftratio == NULL) return 0.3;
	else
	{
		double ret;
		fscanf(fin_shiftratio, "%lf", &ret);
		fclose(fin_shiftratio);
		return ret; //read the ratio from file		
	}
}

double show_image_size(char *directory)
{
	char show_image_sizename[maxlen];
	sprintf(show_image_sizename, "%s%s", directory, "\\configuration\\show_image_size.in");
	FILE *fin_showimagesize = fopen(show_image_sizename, "r");
	if (fin_showimagesize == NULL) return 500.0;
	else
	{
		double ret;
		fscanf(fin_showimagesize, "%lf", &ret);
		fclose(fin_showimagesize);
		return ret; 
	}
}

double sphere_radius(char *directory)
{
	char sphere_radiusname[maxlen];
	sprintf(sphere_radiusname, "%s%s", directory, "\\configuration\\sphere_radius.in");
	FILE *fin_sphereradius = fopen(sphere_radiusname, "r");
	if (fin_sphereradius == NULL) return 0.1;
	else
	{
		double ret;
		fscanf(fin_sphereradius, "%lf", &ret);
		fclose(fin_sphereradius);
		return ret; 
	}
}

double circle_radius(char *directory)
{
	char circle_radiusname[maxlen];
	sprintf(circle_radiusname, "%s%s", directory, "\\configuration\\circle_radius.in");
	FILE *fin_circleradius = fopen(circle_radiusname, "r");
	if (fin_circleradius == NULL) return 3.0;
	else
	{
		double ret;
		fscanf(fin_circleradius, "%lf", &ret);
		fclose(fin_circleradius);
		return ret;
	}
}

double line_width_3d(char *directory)
{
	char line_width3dname[maxlen];
	sprintf(line_width3dname, "%s%s", directory, "\\configuration\\line_width_3d.in");
	FILE *fin_linewidth3d = fopen(line_width3dname, "r");
	if (fin_linewidth3d == NULL) return 10.0;
	else
	{
		double ret;
		fscanf(fin_linewidth3d, "%lf", &ret);
		fclose(fin_linewidth3d);
		return ret;
	}
}

double line_width_2d(char *directory)
{
	char line_width2dname[maxlen];
	sprintf(line_width2dname, "%s%s", directory, "\\configuration\\line_width_2d.in");
	FILE *fin_linewidth2d = fopen(line_width2dname, "r");
	if (fin_linewidth2d == NULL) return 4.0;
	else
	{
		double ret;
		fscanf(fin_linewidth2d, "%lf", &ret);
		fclose(fin_linewidth2d);
		return ret;
	}
}

double mul_factor(char *directory)
{
	char mul_factorname[maxlen];
	sprintf(mul_factorname, "%s%s", directory, "\\configuration\\mul_factor.in");
	FILE *fin_mulfactor = fopen(mul_factorname, "r");
	if (fin_mulfactor == NULL) return 7.0;
	else
	{
		double ret;
		fscanf(fin_mulfactor, "%lf", &ret);
		fclose(fin_mulfactor);
		return ret;		
	}
}

bool fig_edges(char *directory)
{
	char fig_edgesname[maxlen];
	sprintf(fig_edgesname, "%s%s", directory, "\\configuration\\fig_edges.in");
	FILE *fin_figedges = fopen(fig_edgesname, "r");
	if (fin_figedges == NULL) return true;
	else
	{
		int ret;
		fscanf(fin_figedges, "%d", &ret);
		fclose(fin_figedges);
		return (bool)ret;		
	}
}

int GetBackupId(char *directory)
{
	char backupidname[maxlen];
	sprintf(backupidname, "%s%s", directory, "\\configuration\\saveindex.in");
	FILE *fin_backupid = fopen(backupidname, "r");
	int ret_id;
	fscanf(fin_backupid, "%d", &ret_id);
	fclose(fin_backupid);
	return ret_id;	
}