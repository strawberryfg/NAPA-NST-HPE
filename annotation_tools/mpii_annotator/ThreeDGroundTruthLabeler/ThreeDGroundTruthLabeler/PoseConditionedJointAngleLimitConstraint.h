#include "PoseConditionedJointAngleLimit_define.h"
#include "global2local.h"
#include "local2global.h"
//extern from main function
extern double sepplane[parent_num][angle_row_num][angle_col_num][4];
extern double basis_second[parent_num][angle_row_num][angle_col_num][3];
extern double bbx[parent_num][angle_row_num][angle_col_num][4];
extern double boundary_outline[bound_num_all][bound_dimension];
extern bool anglespread[parent_num][angle_row_num][angle_col_num];
extern double *closest_valid_joint_all;

double *findClosestValidPoint(double *boundary, double *point, int N);
double *pointPlane2Sphere(double *xh, double *direction, double desired_norm);
bool *isValid(double *Joint, bool find_closest);