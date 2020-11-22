#Test config
lb = 0
ub = 230
import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d
import pylab as pl
import math
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.pyplot import *
from mpl_toolkits.mplot3d import *
from numpy import *
from matplotlib import pyplot
model_name_str = ['merge_muse_candy_feathers_composition_vii','m113_9', 'm124_4_new', 'm128_4', 'm130_4', 'm129_4', 'm132_13', 'merge_muse_candy', 'merge_muse_candy_feathers']# ['addmympii', 'addselfsup', 'integralblwonst' ,'m114_16', 'nomympii' ,'noselfsup', 'm116_1', 'm115_10', 'm116_4',  'm117_8', 'm118_15', 'm119_18', 'm120_24', 'm121_16', 'm122_3', 'm123_2', 'm124_2', 'm124_4']
#model_name = 'm116_1'
for model_name in model_name_str:
	###Test config
	joint_pred_file_prefix = "D:/results/" + model_name + "/2d/"
	joint_gt_file_prefix = "testset/annot/"
	save_prefix = "D:/results/" + model_name + "/pckh/"

	###Basic settings
	J = 18
	max_norm_distance = 100#50
	part_num = 8
	figure_title = ('PCKh head, MPII', 'PCKh shoulder, MPII', 'PCKh elbow, MPII', 'PCKh wrist, MPII', 'PCKh hip, MPII', 'PCKh knee, MPII', 'PCKh ankle, MPII', 'PCKh total, MPII')		

	body_part_name=('head','shoulder','elbow','wrist','hip','knee','ankle','total')
	obj = [[8, 10], [11, 14], [12, 15], [13, 16], [1,4], [2,5], [3,6]]

	###Denominator
	N = ub - lb + 1
	num = np.zeros(part_num)
	for i in range(0, part_num):
		num[i] = N

	###

	#arrays to compute pckh
	joint_pred = np.zeros((N, J * 2))
	joint_gt = np.zeros((N, J * 2))
	joint_vis = np.zeros((N, J))
	joint_head_bbx = np.zeros((N, 4))
	refdist = np.zeros((N, 1))
	dist = np.zeros((N, J))
	pck = np.zeros((J + 1, max_norm_distance + 1)) 
	pckh = np.zeros((part_num, max_norm_distance + 1))




	for img_idx in range(lb, ub):
		joint_pred_file = joint_pred_file_prefix + str(img_idx) + ".txt"
		joint_gt_file = joint_gt_file_prefix + str(img_idx) + ".txt"
		ave = np.zeros(part_num)
		thresh = np.zeros((part_num, max_norm_distance))

		idx = img_idx - lb
		
		# read joint pred
		f = open(joint_pred_file, 'r')
		for i in range(0, J):
			var = f.readline()
			#print(var)
			var = var.split(' ')

			t_d = 0
			for j in range(len(var)):
				if var[j] != '\n' and len(var[j]) > 0:
					t_num = float(var[j]) 

					joint_pred[idx][i * 2 + t_d] = float(var[j])
					t_d += 1
			
	                    
		f.close()
		
		# read joint gt along with head bbx
		f = open(joint_gt_file, 'r')
		for i in range(0, J):
			var = f.readline()
			var = var.split(' ')
			joint_gt[idx][i * 2 + 0] = float(var[0])
			joint_gt[idx][i * 2 + 1] = float(var[1])
			#print(joint_gt[idx][i * 2 + 0], ' ', joint_gt[idx][i * 2 + 1])
			if joint_gt[idx][i * 2 + 0] > 0 and joint_gt[idx][i * 2 + 1] > 0:
				joint_vis[idx][i] = 1
			else:
				joint_vis[idx][i] = 0
		#print(joint_vis[idx])
		var = f.readline().split(' ')
		joint_head_bbx[idx][0] = float(var[0])
		joint_head_bbx[idx][1] = float(var[1])
		joint_head_bbx[idx][2] = float(var[2])
		joint_head_bbx[idx][3] = float(var[3])

		f.close()
		
		refdist[idx] = 0.6 * sqrt(pow(joint_head_bbx[idx][0] - joint_head_bbx[idx][2], 2) + pow(joint_head_bbx[idx][1] - joint_head_bbx[idx][3], 2))
		for j in range(0, J):
			dist[idx][j] = math.sqrt(math.pow(joint_gt[idx][j * 2] - joint_pred[idx][j * 2], 2) + math.pow(joint_gt[idx][j * 2 + 1] - joint_pred[idx][j * 2 + 1], 2))	
			#print(idx,' ',j,' ',dist[idx][j])
			dist[idx][j] /= refdist[idx]	

	# Count for all at once
	for j in range(0, J):
		if j == 7 or j == 9:
			continue
		for k in range(0, max_norm_distance + 1):
			cnt = 0
			tot_valid = 0
			for i in range(0, N):
				if joint_vis[i][j] != 0:
				   tot_valid = tot_valid + 1
				   if dist[i][j] <= k / 100.0: 
	                              cnt += 1
			cnt /= float(tot_valid)
			pck[j][k] += 100 * cnt
		
	#TOTAL PCKh
	for k in range(0, max_norm_distance + 1):
		cnt = 0
		tot_valid = 0
		for i in range(0, N):
			for j in range(0, J):
				if joint_vis[i][j] != 0:
				   tot_valid = tot_valid + 1
				   if dist[i][j] <= k /100.0:
				      cnt += 1				
		cnt /= float(tot_valid)	

		pck[J][k] = 100 * cnt

	#part 2 joints average except TOTAL so part_num - 1
	for i in range(0, part_num - 1):	
		for j in range(0, max_norm_distance + 1):
			pckh[i][j] = (pck[obj[i][0]][j] + pck[obj[i][1]][j]) / 2.0

	# use pck[J] which is pck[18] to denote TOTAL PCKh for the sake of convenience
	for i in range(0, max_norm_distance + 1):
	    pckh[part_num - 1][i] = pck[J][i]    
		
	for i in range(0, part_num):

		fig = plt.figure()
		ax = fig.add_subplot(111)
		X = np.linspace(0.0 , max_norm_distance / 100.0 , max_norm_distance + 1, endpoint=True)        
		plt.ylim([0, 1])
		plt.xticks(np.linspace(0.0 , 1.0, 10, endpoint=False))
		plt.xlim([0, 1.0])
		plt.yticks(np.linspace(0, 100, 11, endpoint=True))
		plt.xlabel('Normalized distance')
		plt.ylabel('Detection rate %')
		plt.title(figure_title[i])
	    
		ax.plot(X, pckh[i],  c='r')
		ax.grid()
		
		fig.savefig(save_prefix + body_part_name[i] + 'PCKh' + '.png')        
		fileid = open(save_prefix + body_part_name[i] + '_percentage' + '.txt', 'wt')
		for j in range(0, max_norm_distance):
			fileid.write('{:12.6f}\n'.format(pckh[i][j]))
		fileid.close()
		plt.close()