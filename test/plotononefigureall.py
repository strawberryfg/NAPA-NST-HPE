import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d
import pylab
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.pyplot import *
from mpl_toolkits.mplot3d import *
from numpy import *
from matplotlib import pyplot

def read_file(filepath, N, arr):
    with open(filepath) as input_file:
    	 i = 0
    	 for line in input_file:
    	 	 if line=='\n':
    	 	 	continue
    	 	 arr[i] = float(line) / 100.0
    	 	 i += 1
    	 	 if i>=N:
                    break

with open('plotononefigureconfiguration.in') as conf_input:
	 conf = conf_input.read().split('\n')

part_name = ["ankle", "knee", "elbow", "wrist", "head", "hip", "shoulder", "total"]

for epoch in range(0, 8):

    num_of_method = int(conf[0])
    num_of_x = int(conf[1])
    percentage_file = []
    for i in range(0, num_of_method):
    	ts = conf[i + 2].replace("*", part_name[epoch])
    	percentage_file.append(ts) #conf[i + 2])
    method_name = []
    for i in range(0, num_of_method):
    	method_name.append(conf[num_of_method + i + 2])
    save_name = conf[2 * num_of_method + 2]
    save_name = save_name.replace("*", part_name[epoch])

    thresh = np.zeros((num_of_method, num_of_x))
    for i in range(0, num_of_method):
    	read_file(percentage_file[i], num_of_x, thresh[i])

    fig_thresh = plt.figure(figsize=(9,7))
    ax_thresh = fig_thresh.add_subplot(111)
    color=('r','g','k','c','b','m','y','#FFC0CB', '#FFA500')
    for i in range(0, num_of_method):
    	X = np.linspace(0.0 , num_of_x / 100.0 , 100, endpoint=True)        
    	plt.xticks(np.linspace(0.0 , 1.0, 10, endpoint= False))
    	plt.xlim([0,1.0])
    	ax_thresh.plot(X, thresh[i], label=method_name[i], c=color[i])

    plt.xlabel('Normalized distance')
    plt.ylabel('Detection rate %')        
    plt.title(save_name)
    ax_thresh.grid()
    ax_thresh.legend(loc='center', 
              fancybox=True, shadow=True, ncol=1)

    fig_thresh.show()
    fig_thresh.savefig(save_name+'.png')
    plt.close()