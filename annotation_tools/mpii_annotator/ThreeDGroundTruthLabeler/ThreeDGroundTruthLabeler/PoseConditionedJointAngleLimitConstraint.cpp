#include "PoseConditionedJointAngleLimitConstraint.h"
double *findClosestValidPoint(double *boundary, double *point, int N)
{
	double min_dist = 1e30;
	int id = 0;
	for (int i = 0; i < N; i++)
	{
		double t_dist = 0.0;
		t_dist = pow(boundary[i * 2] - point[0], 2) + pow(boundary[i * 2 + 1] - point[1], 2);
		if (sgn(t_dist - min_dist) < 0)
		{
			min_dist = t_dist;
			id = i;
		}
	}
	double *ret = (double *)malloc(sizeof(double)* 2);
	ret[0] = boundary[id * 2];
	ret[1] = boundary[id * 2 + 1];
	return ret;
}

double *pointPlane2Sphere(double *xh, double *direction, double desired_norm)
{
	double *ret = (double *)malloc(sizeof(double)* 3);
	for (int i = 0; i < 3; i++)
	{
		if (isnan(xh[i]))
		{
			for (int j = 0; j < 3; j++) ret[j] = -2147483648.0;
			return ret;
		}
	}
	double *vh = (double *)malloc(sizeof(double)* 3);
	for (int i = 0; i < 3; i++) vh[i] = direction[i];   //Ax + By + Cz + D eliminate D
	double xv = xh[0] * vh[0] + xh[1] * vh[1] + xh[2] * vh[2];
	double t = pow(xv, 2) + 1.0 - norm(xh);
	double dis;
	if (t < 1e-12) dis = 0.0;
	else dis = sqrt(t);
	double a1 = -xv + dis;
	double a2 = -xv - dis;
	double *x1 = (double *)malloc(sizeof(double)* 3);
	double *x2 = (double *)malloc(sizeof(double)* 3);
	copy(x1, xh);
	addition(x1, mul_single_num(vh, a1));
	copy(x2, xh);
	addition(x2, mul_single_num(vh, a2));
	if (x1[0] * direction[0] + x1[1] * direction[1] + x1[2] * direction[2] + direction[3] <= 1e-6) ret = mul_single_num(x1, desired_norm);
	else if (x2[0] * direction[0] + x2[1] * direction[1] + x2[2] * direction[2] + direction[3] <= 1e-6)	 ret = mul_single_num(x2, desired_norm);
	return ret;
}

bool *isValid(double *Joint, bool find_closest)
{
	bool *res = (bool *)malloc(sizeof(bool)* bone_num);
	for (int i = 0; i < bone_num; i++) res[i] = true;
	double *dJoint = (double *)malloc(sizeof(double)*bone_num * 3);
	for (int i = 0; i < bone_num; i++)
	{
		copy(dJoint + index(i), Joint + index(edges_pjcal[i][0]));
		subtract(dJoint + index(i), Joint + index(edges_pjcal[i][1]));
		//Joint[edges[i][0]][]-Joint[edges[i][1]][] relative depths
	}
	double *t_dJoint = global2local(dJoint); //transform from global coordinate to local coordinate	
	double *angles = (double *)malloc(sizeof(double)* bone_num * 2); //row_id and col_id of bones(for saving parent's)

	for (int i = 0; i < parent_num; i++) //bones excluding torso bones
	{
		double *sph = (double *)malloc(sizeof(double)* 3);
		double *child_bone = (double *)malloc(sizeof(double)* 3);
		copy(child_bone, t_dJoint + index(children_bone[i]));
		//azimuth elevation radiius		
		sph = cart2sph(child_bone + 0, child_bone + 1, child_bone + 2);
		divide_single_num(child_bone, sph[2]); //chldB=chldB/ri so as to make the local vector "unit" vector norm=1		
		//here divides
		double azm = rad2deg(sph[0]);
		double elv = rad2deg(sph[1]);
		int azm_id = (int)floor((azm + 180.0) / occupancy_matrix_jmp + 1.0) - 1.0; //-180.0->180.0
		int elv_id = (int)floor((elv + 90.0) / occupancy_matrix_jmp + 1.0) - 1.0;  //-90.0->90.0  each jmp stands for one column or row		
		angles[children_bone[i] * 2] = azm_id;     //row_id		
		angles[children_bone[i] * 2 + 1] = elv_id; //col_id		
		if (find_in_torso(children_bone[i]))
		{
			if (!anglespread[i][azm_id][elv_id])
				res[children_bone[i]] = false; //invalid
		}
		else
		{
			azm_id = angles[parent_bone[i] * 2];
			elv_id = angles[parent_bone[i] * 2 + 1];
			double *v = (double *)malloc(sizeof(double)* 4), t_norm = 0.0;
			bool occur_nan = false;
			for (int j = 0; j < 4; j++)
			{
				v[j] = sepplane[i][azm_id][elv_id][j];
				if (isnan(v[j]))
				{
					occur_nan = true;
					res[children_bone[i]] = false; //invalid
					break;
				}
				t_norm += (j < 3 ? pow(v[j], 2) : 0.0);
			}

			if (!occur_nan)
			{
				t_norm = sqrt(t_norm); //get the norm of first 3 dimensions of the vector
				for (int j = 0; j < 4; j++) v[j] = v[j] / t_norm; //normalize this vector
				double *b = (double *)malloc(sizeof(double)* 4);
				copy(b, child_bone);
				b[3] = 1.0; //[A B C D] [x 
				//           y
				//		     z
				//   		 1]

				if (sgn(calc_vec4Xvec4(v, b))>0)   //norm vector * vector
				{
					res[children_bone[i]] = false;
				} //b^T n + d < 0 : valid   it's a separating plane
				else
				{
					//projection onto separating plane check whether it is within the bounding box range
					double *e1 = (double *)malloc(sizeof(double)* 3);
					for (int j = 0; j < 3; j++) e1[j] = v[j];     //b1
					double *e2 = (double *)malloc(sizeof(double)* 3);
					for (int j = 0; j < 3; j++) e2[j] = basis_second[i][azm_id][elv_id][j]; //b2
					double *e3 = (double *)malloc(sizeof(double)* 3);
					e3 = cross(e1, e2);
					double *T = (double *)malloc(sizeof(double)* 9);
					T = gramschimidt(e1, e2, e3);
					double *u = (double *)malloc(sizeof(double)* 2);
					u = calc_mat3x3last2coltransposeXvec3(T, child_bone);
					if (u[0]<bbx[i][azm_id][elv_id][0] || u[0]>bbx[i][azm_id][elv_id][1] || u[1]<bbx[i][azm_id][elv_id][2] || u[1]>bbx[i][azm_id][elv_id][3])
						//out of the rectangular bounding bix
					{
						res[children_bone[i]] = false;
					}
				}
			}
		}
	}
	if (find_closest)
	{
		double azm_edge[121];
		double elv_edge[61];
		for (int i = 0; i < 120; i++) azm_edge[i] = -180 + 3 * i;
		for (int i = 0; i < 60; i++) elv_edge[i] = -90 + 3 * i;
		double *angles2 = (double *)malloc(sizeof(double)* bone_num * 2);
		for (int i = 0; i < bone_num * 2; i++) angles2[i] = angles[i];
		double *t_dJoint2 = (double *)malloc(sizeof(double)* bone_num * 3);
		for (int i = 0; i < bone_num * 3; i++) t_dJoint2[i] = t_dJoint[i]; //use save
		for (int i = 0; i < bone_num; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				cout << t_dJoint[i * 3 + j] << " ";
			}
			cout << "\n";
		}
		for (int i = 0; i < parent_num; i++)
		{
			double *child_bone = (double *)malloc(sizeof(double)* 3);
			copy(child_bone, t_dJoint + index(children_bone[i]));
			double t_radius = norm(child_bone);
			divide_single_num(child_bone, t_radius); // / norm
			if (find_in_torso(children_bone[i])) //is torso : binary occupancy matrix
			{
				if (!res[children_bone[i]]) //if angle is invalid
				{
					double *angi = (double *)malloc(sizeof(double)* 2);
					angi[0] = angles2[children_bone[i] * 2];
					angi[1] = angles2[children_bone[i] * 2 + 1];
					double *angi2 = (double *)malloc(sizeof(double)* 2);
					angi2 = findClosestValidPoint(boundary_outline[i], angi, bound_point_num[i]);
					angles2[children_bone[i] * 2] = angi2[0];
					angles2[children_bone[i] * 2 + 1] = angi2[1]; //replace with the closest valid point in the boundary outline
					//new azimuth and new elevation (new row and column index)
					double theta = deg2rad(azm_edge[(int)angi2[0]]);
					double phi = deg2rad(elv_edge[(int)angi2[1]]);   // e.g. convert -177 degree to rad
					double *t_cart = sph2cart(theta, phi, t_radius); //t_norm   retain the fixed bone length
					copy(t_dJoint2 + index(children_bone[i]), t_cart); //new 3d location(maintain bone length new azimuth angle and elevation angle)
				}
			}
			else//not torso
			{
				if (!res[children_bone[i]])
				{
					int azm_id = angles2[parent_bone[i] * 2];      //parent bone may be torso and may be modified already
					int elv_id = angles2[parent_bone[i] * 2 + 1];
					double *v = (double *)malloc(sizeof(double)* 4), t_norm = 0.0;
					bool occur_nan = false;
					for (int j = 0; j < 4; j++)
					{
						v[j] = sepplane[i][azm_id][elv_id][j];
						if (isnan(v[j]))
						{
							occur_nan = true;
							break;
						}
						t_norm += (j < 3 ? pow(v[j], 2) : 0.0);
					}
					if (occur_nan) continue;
					//v = squeeze(sepPlane{i}(t_p,p_p,:));
					//v = v/norm(v(1:3));
					t_norm = sqrt(t_norm); //get the norm of first 3 dimensions of the vector
					for (int j = 0; j < 4; j++) v[j] = v[j] / t_norm; //normalize this vector
					double *b = (double *)malloc(sizeof(double)* 4);
					copy(b, child_bone);
					b[3] = 1.0; //[A B C D] [x 
					//           y
					//		     z
					//   		 1]

					//projection onto separating plane check whether it is within the bounding box range
					double *e1 = (double *)malloc(sizeof(double)* 3);
					for (int j = 0; j < 3; j++) e1[j] = v[j];     //b1
					double *e2 = (double *)malloc(sizeof(double)* 3);
					for (int j = 0; j < 3; j++) e2[j] = basis_second[i][azm_id][elv_id][j]; //b2
					double *e3 = (double *)malloc(sizeof(double)* 3);
					e3 = cross(e1, e2);
					double *T = (double *)malloc(sizeof(double)* 9);
					T = gramschimidt(e1, e2, e3);
					double *new_xyz = (double *)malloc(sizeof(double)* 3);
					double *u = (double *)malloc(sizeof(double)* 2);
					if (sgn(calc_vec4Xvec4(v, b))>0)   //norm vector * vector b^Tn+d>0  project it to the plane
					{
						//T(:,2:3)'*chldB							
						u = calc_mat3x3last2coltransposeXvec3(T, child_bone);
						//T(:2,3) *  (T(:,2:3)'*chldB)
						new_xyz = calc_mat3x3last2colXvec2(T, u);
						copy(child_bone, new_xyz);
					}
					u = calc_mat3x3last2coltransposeXvec3(T, child_bone);   //projection 2 dimension
					if (u[0] < bbx[i][azm_id][elv_id][0] + 0.06) u[0] = bbx[i][azm_id][elv_id][0] + 0.06;
					if (u[0] > bbx[i][azm_id][elv_id][1] - 0.06) u[0] = bbx[i][azm_id][elv_id][1] - 0.06;
					if (u[1] < bbx[i][azm_id][elv_id][2] + 0.06) u[1] = bbx[i][azm_id][elv_id][2] + 0.06;
					if (u[1] > bbx[i][azm_id][elv_id][3] - 0.06) u[1] = bbx[i][azm_id][elv_id][3] - 0.06;
					double norm_u = sqrt(pow(u[0], 2) + pow(u[1], 2));
					if (sgn(norm_u - 1.0) > 0) //if norm(u)>1 valid points only lie inside a unit circle
					{
						u[0] /= norm_u; u[1] /= norm_u;
					}
					//after moving u inside the bounding-box can acquire the new 3d location
					new_xyz = calc_mat3x3last2colXvec2(T, u);
					new_xyz = pointPlane2Sphere(new_xyz, v, t_radius);
					double *sph = (double *)malloc(sizeof(double)* 3);
					//azimuth elevation radiius		
					sph = cart2sph(new_xyz, new_xyz + 1, new_xyz + 2);
					double azm = rad2deg(sph[0]);
					double elv = rad2deg(sph[1]);
					double new_azm_id = (int)floor((azm + 180.0) / occupancy_matrix_jmp + 1.0) - 1.0; //-180.0->180.0
					double new_elv_id = (int)floor((elv + 90.0) / occupancy_matrix_jmp + 1.0) - 1.0;  //-90.0->90.0  each jmp stands for one column or row		

					if ((i == 6 || i == 9) && !anglespread[i][(int)new_azm_id][(int)new_elv_id]) //occupancy matrix item = 0  
					{
						double *thetas = (double *)malloc(sizeof(double)*bound_point_num[i]);
						double *phis = (double *)malloc(sizeof(double)*bound_point_num[i]);
						double *boundarypoints_x = (double *)malloc(sizeof(double)*bound_point_num[i]);
						double *boundarypoints_y = (double *)malloc(sizeof(double)*bound_point_num[i]);
						double *boundarypoints_z = (double *)malloc(sizeof(double)*bound_point_num[i]);
						double *boundarypoints_one = (double *)malloc(sizeof(double)*bound_point_num[i]);
						double **boundarypoints = (double **)malloc(sizeof(double *)* 4);
						boundarypoints[0] = boundarypoints_x;
						boundarypoints[1] = boundarypoints_y;
						boundarypoints[2] = boundarypoints_z;
						boundarypoints[3] = boundarypoints_one;
						for (int j = 0; j < bound_point_num[i]; j++)
						{
							thetas[j] = deg2rad(azm_edge[(int)boundary_outline[i][j * 2]]);
							phis[j] = deg2rad(elv_edge[(int)boundary_outline[i][j * 2 + 1]]);
							double *t_cart = sph2cart(thetas[j], phis[j], 1);
							boundarypoints[0][j] = t_cart[0];
							boundarypoints[1][j] = t_cart[1];
							boundarypoints[2][j] = t_cart[2];
							boundarypoints[3][j] = 1.0;
						}
						// filter points on the wrong side of the sepPlane
						vector<int> ind;
						for (int j = 0; j < bound_point_num[i]; j++)
						{
							double *t_b = (double *)malloc(sizeof(double)* 4);
							t_b[0] = boundarypoints[0][j];
							t_b[1] = boundarypoints[1][j];
							t_b[2] = boundarypoints[2][j];
							t_b[3] = boundarypoints[3][j];
							if (sgn(calc_vec4Xvec4(v, t_b)) < 0) ind.push_back(j);
						} //ind = v'*[bndryPts; ones(1,length(Xs))]<0;
						int t_n = ind.size();
						double *new_bound_x = (double *)malloc(sizeof(double)* t_n);
						double *new_bound_y = (double *)malloc(sizeof(double)* t_n);
						double **new_bound = (double **)malloc(sizeof(double *)* 2);
						new_bound[0] = new_bound_x;
						new_bound[1] = new_bound_y;
						for (int j = 0; j < t_n; j++)
						{
							double *boundary_wrongside_j = (double *)malloc(sizeof(double)* 3);
							for (int k = 0; k < 3; k++) boundary_wrongside_j[k] = boundarypoints[k][ind[j]];  //notice ind[j]
							double *t_bound = calc_mat3x3last2coltransposeXvec3(T, boundary_wrongside_j); //bndryU = T(:,2:3)'*bndryPts(:, ind);                    
							new_bound[0][j] = t_bound[0];
							new_bound[1][j] = t_bound[1];
							if (new_bound[0][j] < bbx[i][azm_id][elv_id][0] + 0.06) new_bound[0][j] = bbx[i][azm_id][elv_id][0] + 0.06;
							if (new_bound[0][j] > bbx[i][azm_id][elv_id][1] - 0.06) new_bound[0][j] = bbx[i][azm_id][elv_id][1] - 0.06;
							if (new_bound[1][j] < bbx[i][azm_id][elv_id][2] + 0.06) new_bound[1][j] = bbx[i][azm_id][elv_id][2] + 0.06;
							if (new_bound[1][j] > bbx[i][azm_id][elv_id][3] - 0.06) new_bound[1][j] = bbx[i][azm_id][elv_id][3] - 0.06;
						}
						double *flat_bound = (double *)malloc(sizeof(double)* t_n * 2);
						for (int j = 0; j < t_n; j++)
						{
							flat_bound[j * 2] = new_bound[0][j];
							flat_bound[j * 2 + 1] = new_bound[1][j];
						}
						double *closest_projection = findClosestValidPoint(flat_bound, u, t_n);
						new_xyz = calc_mat3x3last2colXvec2(T, closest_projection);
						new_xyz = pointPlane2Sphere(new_xyz, v, t_radius);
						double *sph = (double *)malloc(sizeof(double)* 3);
						//azimuth elevation radiius		
						sph = cart2sph(new_xyz, new_xyz + 1, new_xyz + 2);
						double azm = rad2deg(sph[0]);
						double elv = rad2deg(sph[1]);
						new_azm_id = (int)floor((azm + 180.0) / occupancy_matrix_jmp + 1.0) - 1.0; //-180.0->180.0
						new_elv_id = (int)floor((elv + 90.0) / occupancy_matrix_jmp + 1.0) - 1.0;  //-90.0->90.0  each jmp stands for one column or row		
					}
					copy(t_dJoint2 + index(children_bone[i]), new_xyz); //new 3d location(maintain bone length new azimuth angle and elevation angle)	
					angles2[children_bone[i] * 2] = new_azm_id;
					angles2[children_bone[i] * 2 + 1] = new_elv_id;
				}
			}
		}
		double *ds = (double *)malloc(sizeof(double)* bone_num * 3);
		ds = local2global(t_dJoint2);

		double *base = (double *)malloc(sizeof(double)* 3);
		copy(base, Joint); //belly  origin  as base
		closest_valid_joint_all = estimateJoint(ds, base);
	}
	return res;
}