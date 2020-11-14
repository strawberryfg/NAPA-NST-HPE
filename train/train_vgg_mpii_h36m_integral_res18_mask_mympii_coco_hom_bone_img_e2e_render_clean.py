st_epoch = 126
model_str = "125_4"
folder_prefix = "m118"
on_the_fly_mod = 100


import numpy as np
import torch
import torch.nn as nn
from torch.nn import functional as F

#Compositional Part
std_neg_gt_thorax_arr = np.array([0.099808, 0.114870, 0.363336])
avg_neg_gt_thorax_arr = np.array([0.001901, -0.253125, -1.955116])
avg_gt_bone_arr = np.array([-0.011002,
                             0.203671,
                             0.054641,
                             0.011081,
                             0.241752,
                            -0.001770,
                             0.035494,
                             0.000022,
                            -0.000552,
                            -0.035496,
                            -0.000032,
                             0.000544,
                            -0.016262,
                             0.241034,
                            -0.004212,
                             0.007038,
                             0.205022,
                             0.052795,
                             0.000471,
                             0.380163,
                            -0.019875,
                            -0.001045,
                            -0.044337,
                             0.000661,
                            -0.002024,
                            -0.186339,
                             0.033024,
                            -0.016583,
                             0.073317,
                            -0.048659,
                             0.015415,
                             0.171697,
                             0.002787,
                             0.066536,
                             0.000573,
                            -0.002698,
                            -0.066538,
                            -0.000564,
                             0.002683,
                            -0.016018,
                             0.175343,
                             0.003213,
                             0.013102,
                             0.079182,
                            -0.050601])

std_gt_bone_arr = np.array([0.123118,
                            0.121942,
                            0.124030,
                            0.143290,
                            0.135595,
                            0.158152,
                            0.063711,
                            0.022660,
                            0.064554,
                            0.063710,
                            0.022652,
                            0.064557,
                            0.141042,
                            0.135641,
                            0.160715,
                            0.121440,
                            0.121611,
                            0.124870,
                            0.138395,
                            0.113654,
                            0.128882,
                            0.024645,
                            0.026819,
                            0.040076,
                            0.105419,
                            0.078530,
                            0.096968,
                            0.122277,
                            0.123592,
                            0.155742,
                            0.098509,
                            0.089480,
                            0.122735,
                            0.117091,
                            0.046648,
                            0.205375,
                            0.117087,
                            0.046648,
                            0.205378,
                            0.095904,
                            0.086500,
                            0.121688,
                            0.121242,
                            0.120674,
                            0.155680])

def unnorm_neg_thorax(neg_thorax):
    ret = neg_thorax[:,:] * std_neg_gt_thorax_arr[:] + avg_neg_gt_thorax_arr[:]
    return ret 

def unnorm_bone(norm_bone):
    ret = norm_bone[:,:] * std_gt_bone_arr[:] + avg_gt_bone_arr[:]
    return ret

def bone2joint(bottom_data):
    top_data = np.zeros((bottom_data.shape[0], 18 * 3), dtype=np.float)
    for t in range(bottom_data.shape[0]):
        for i in range(18):
            x = i
            while x != thorax:
                cur_bone = int(which_bone[x][int(parent[x])])
                for j in range(3):
                    top_data[t][i * 3 + j] += -bottom_data[t][cur_bone * 3 + j]
                x = int(parent[x])            
                                        
    return top_data

def pinhole_proj(bottom_data):
    top_data = np.zeros((bottom_data.shape[0], 18 * 2), dtype=np.float)
    size = 224
    focus = 300
    for t in range(bottom_data.shape[0]):
        for i in range(18):    
            x = bottom_data[t][i * 3]
            y = bottom_data[t][i * 3 + 1]
            z = bottom_data[t][i * 3 + 2]
            top_data[t][i * 2] = focus * x / z / size + 0.5
            top_data[t][i * 2 + 1] = -focus * y / z / size + 0.5
    return top_data

#Compositional Bone Pose Regeression
pelvis = 0     #0
right_hip = 1      #1  
right_knee = 2     #2
right_ankle = 3    #3 
left_hip = 4      #4 
left_knee = 5     #5
left_ankle = 6    #6
upper_neck = 8       #8
head_top = 10       #10 
left_shoulder = 11 #11
left_elbow = 12    #12
left_wrist = 13   #13
right_shoulder = 14 #14 
right_elbow = 15    #15
right_wrist = 16    #16
thorax = 17     #17
parent = np.zeros((18))
parent[right_ankle] = right_knee
parent[right_knee] = right_hip
parent[right_hip] = pelvis
parent[pelvis] = thorax
parent[thorax] = -1
parent[left_ankle] = left_knee
parent[left_knee] = left_hip
parent[left_hip] = pelvis
parent[right_wrist] = right_elbow
parent[right_elbow] = right_shoulder
parent[right_shoulder] = thorax
parent[left_wrist] = left_elbow
parent[left_elbow] = left_shoulder
parent[left_shoulder] = thorax
parent[head_top] = upper_neck
parent[upper_neck] = thorax
BoneBasedBoneNum = 15


right_ankle_mpii = 0
right_knee_mpii = 1
right_hip_mpii = 2
left_hip_mpii = 3
left_knee_mpii = 4
left_ankle_mpii = 5
pelvis_mpii = 6
thorax_mpii = 7
upper_neck_mpii = 8
head_top_mpii = 9
right_wrist_mpii = 10
right_elbow_mpii = 11
right_shoulder_mpii = 12
left_shoulder_mpii = 13
left_elbow_mpii = 14
left_wrist_mpii = 15



bones_mpii =(
            ( right_ankle_mpii, right_knee_mpii ),     #0
            ( right_knee_mpii, right_hip_mpii ),       #1
            ( right_hip_mpii, pelvis_mpii ),           #2
            ( left_hip_mpii, pelvis_mpii ),            #3 
            ( left_knee_mpii, left_hip_mpii ),         #4
            ( left_ankle_mpii, left_knee_mpii ),       #5
            ( pelvis_mpii, thorax_mpii ),              #6
            ( upper_neck_mpii, thorax_mpii ),          #7
            ( head_top_mpii, upper_neck_mpii ),        #8
            ( right_wrist_mpii, right_elbow_mpii ),    #9
            ( right_elbow_mpii, right_shoulder_mpii ), #10
            ( right_shoulder_mpii, thorax_mpii ),      #11
            ( left_shoulder_mpii, thorax_mpii ),       #12
            ( left_elbow_mpii, left_shoulder_mpii ),   #13
            ( left_wrist_mpii, left_elbow_mpii )       #14
        )

bones =(
            ( right_ankle, right_knee ),     #0
            ( right_knee, right_hip ),       #1
            ( right_hip, pelvis ),           #2
            ( left_hip, pelvis ),            #3 
            ( left_knee, left_hip ),         #4
            ( left_ankle, left_knee ),       #5
            ( pelvis, thorax ),              #6
            ( upper_neck, thorax ),          #7
            ( head_top, upper_neck ),        #8
            ( right_wrist, right_elbow ),    #9
            ( right_elbow, right_shoulder ), #10
            ( right_shoulder, thorax ),      #11
            ( left_shoulder, thorax ),       #12
            ( left_elbow, left_shoulder ),   #13
            ( left_wrist, left_elbow )       #14
        )

which_bone = np.zeros((18, 18))
for i in range(BoneBasedBoneNum):
    which_bone[bones[i][0]][bones[i][1]] = i;

import math
import random

from datetime import datetime
from collections import defaultdict

random.seed(datetime.now())
import os
import os.path as osp

#from google.colab.patches import cv2_imshow
import cv2
import torchvision.transforms as transforms
from matplotlib.pyplot import *

# 0. Data Source Macros
HUMAN36M = 0
MPII = 1
MYMPII = 2
VAL2017 = 3

data_source_str = ['Human3.6M', 'MPII', 'MyMPII']
mympii_minz = -1.1702
mympii_maxz = 1.0009


def rgb_to_srgb(image):
    small_u = image*12.92
    big_u = torch.pow(image,.416)*1.055-0.055
    return torch.where(image<=0.0031308, small_u, big_u)


# 0. Color HSV space
class HSVLoss(nn.Module):
    def __init__(self, h=0, s=1, v=0.7, eps=1e-7, threshold_h=0.03, threshold_sv=0.1):
        super(HSVLoss, self).__init__()
        self.hsv = [h, s, v]
        self.loss = nn.L1Loss(reduction='none')
        self.eps = eps

        self.threshold_h = threshold_h
        self.threshold_sv = threshold_sv

    def get_hsv(self, im):
        img = im * 0.5 + 0.5
        hue = torch.Tensor(im.shape[0], im.shape[2], im.shape[3]).to(im.device)

        hue[ img[:,2]==img.max(1)[0] ] = 4.0 + ( (img[:,0]-img[:,1]) / ( img.max(1)[0] - img.min(1)[0] + self.eps) ) [ img[:,2]==img.max(1)[0] ]
        hue[ img[:,1]==img.max(1)[0] ] = 2.0 + ( (img[:,2]-img[:,0]) / ( img.max(1)[0] - img.min(1)[0] + self.eps) ) [ img[:,1]==img.max(1)[0] ]
        hue[ img[:,0]==img.max(1)[0] ] = (0.0 + ( (img[:,1]-img[:,2]) / ( img.max(1)[0] - img.min(1)[0] + self.eps) ) [ img[:,0]==img.max(1)[0] ]) % 6

        hue[img.min(1)[0]==img.max(1)[0]] = 0.0
        hue = hue/6

        saturation = ( img.max(1)[0] - img.min(1)[0] ) / ( img.max(1)[0] + self.eps )
        saturation[ img.max(1)[0]==0 ] = 0

        value = img.max(1)[0]
        return hue, saturation, value

    def get_rgb_from_hsv(self):
        C = self.hsv[2] * self.hsv[1]
        X = C * ( 1 - abs( (self.hsv[0]*6)%2 - 1 ) )
        m = self.hsv[2] - C

        if self.hsv[0] < 1/6:
            R_hat, G_hat, B_hat = C, X, 0
        elif self.hsv[0] < 2/6:
            R_hat, G_hat, B_hat = X, C, 0
        elif self.hsv[0] < 3/6:
            R_hat, G_hat, B_hat = 0, C, X
        elif self.hsv[0] < 4/6:
            R_hat, G_hat, B_hat = 0, X, C
        elif self.hsv[0] < 5/6:
            R_hat, G_hat, B_hat = X, 0, C
        elif self.hsv[0] <= 6/6:
            R_hat, G_hat, B_hat = C, 0, X

        R, G, B = (R_hat+m), (G_hat+m), (B_hat+m)
        
        return R, G, B
    
    
    def forward(self, input):
        h, s, v = self.get_hsv(input)

        target_h = torch.Tensor(h.shape).fill_(self.hsv[0]).to(input.device).type_as(h)
        target_s = torch.Tensor(s.shape).fill_(self.hsv[1]).to(input.device).type_as(s)
        target_v = torch.Tensor(v.shape).fill_(self.hsv[2]).to(input.device).type_as(v)

        loss_h = self.loss(h, target_h)
        loss_h[loss_h<self.threshold_h] = 0.0
        loss_h = loss_h.mean()

        if loss_h < self.threshold_h*3:
            loss_h = torch.Tensor([0]).to(input.device)
        
        loss_s = self.loss(s, target_s).mean()
        if loss_h.item() > self.threshold_sv:   
            loss_s = torch.Tensor([0]).to(input.device)

        loss_v = self.loss(v, target_v).mean()
        if loss_h.item() > self.threshold_sv:   
            loss_v = torch.Tensor([0]).to(input.device)

        return loss_h + 4e-1*loss_s + 4e-1*loss_v

# 0. KL JS
def _kl_div_2d(p, q):
    # D_KL(P || Q)
    batch, chans, height, width = p.shape
    unsummed_kl = F.kl_div(
        q.view(batch * chans, height * width).log(),
        p.view(batch * chans, height * width),
        reduction='none',
    )
    kl_values = unsummed_kl.sum(-1).view(batch, chans)
    return kl_values


def _js_div_2d(p, q):
    # JSD(P || Q)
    m = 0.5 * (p + q)
    return 0.5 * _kl_div_2d(p, m) + 0.5 * _kl_div_2d(q, m)


def _reduce_loss(losses, reduction):
    if reduction == 'none':
        return losses
    else:
        return torch.mean(losses) if reduction == 'mean' else torch.sum(losses)

def js_div_loss_2d(
        input: torch.Tensor,
        target: torch.Tensor,
        reduction: str = 'mean'
):
    r"""Calculates the Jensen-Shannon divergence loss between heatmaps.

    Arguments:
        input (torch.Tensor): the input tensor.
        target (torch.Tensor): the target tensor.
        reduction (string, optional): Specifies the reduction to apply to the
          output: ``'none'`` | ``'mean'`` | ``'sum'``. ``'none'``: no reduction
          will be applied, ``'mean'``: the sum of the output will be divided by
          the number of elements in the output, ``'sum'``: the output will be
          summed. Default: ``'mean'``.

    Shape:
        - Input: :math:`(B, N, H, W)`
        - Target: :math:`(B, N, H, W)`, same shape as the input
    """
    return _reduce_loss(_js_div_2d(target, input), reduction)



def kl_div_loss_2d(
        input: torch.Tensor,
        target: torch.Tensor,
        reduction: str = 'mean'
):
    r"""Calculates the Kullback-Leibler divergence loss between heatmaps.

    Arguments:
        input (torch.Tensor): the input tensor.
        target (torch.Tensor): the target tensor.
        reduction (string, optional): Specifies the reduction to apply to the
          output: ``'none'`` | ``'mean'`` | ``'sum'``. ``'none'``: no reduction
          will be applied, ``'mean'``: the sum of the output will be divided by
          the number of elements in the output, ``'sum'``: the output will be
          summed. Default: ``'mean'``.

    Shape:
        - Input: :math:`(B, N, H, W)`
        - Target: :math:`(B, N, H, W)`, same shape as the input
    """
    return _reduce_loss(_kl_div_2d(target, input), reduction)


def _isArrayLike(obj):
    return hasattr(obj, '__iter__') and hasattr(obj, '__len__')

# 0. Hyper-parameters
#optimization param
TV_WEIGHT = 1e-7

per_iter_save_num = 500

max_iter = 5
show_iter = 20
show_cnt = 1
per_epoch_iters = 100 #100 * batch_size per epoh
img_size = 224
output_shape = img_size // 4
bbox_3d_shape = (2000, 2000, 2000) # depth, height, width
depth_dim = 64
    
batch_size = 2
pixel_mean = (0.485, 0.456, 0.406)
pixel_std = (0.229, 0.224, 0.225)

#pixel_mean = (0.48501961, 0.45795686, 0.40760392)
#pixel_std = (1, 1, 1)

pixel_mean_neg = (-0.485, -0.456, -0.406)
pixel_std_neg = (1.0 / 0.229, 1.0 / 0.224, 1.0 / 0.225)

#pixel_mean_neg = (-48501961, -0.45795686, -0.40760392)
#pixel_std_neg = (1.0 / 1.0, 1.0 / 1.0, 1.0 / 1.0)

vis_dir = "vis/"


# gram matrix and loss
class GramMatrix(nn.Module):
    def forward(self, input):
        b, c, h, w = input.size()
        F = input.view(b, c, h * w)
        G = torch.bmm(F, F.transpose(1,2)) 
        G.div_(h * w)
        return G

class GramMSELoss(nn.Module):
    def forward(self, input, target):
        out = nn.MSELoss()(GramMatrix()(input), target)
        return(out)

## Visualization
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import matplotlib as mpl
def vis_keypoints(img, kps, kps_lines, kp_thresh=0.4, alpha=1):

    # Convert from plt 0-1 RGBA colors to 0-255 BGR colors for opencv.
    cmap = plt.get_cmap('rainbow')
    colors = [cmap(i) for i in np.linspace(0, 1, len(kps_lines) + 2)]
    colors = [(c[2] * 255, c[1] * 255, c[0] * 255) for c in colors]

    # Perform the drawing on a copy of the image, to allow for blending.
    kp_mask = np.copy(img)

    # Draw the keypoints.
    for l in range(len(kps_lines)):
        i1 = kps_lines[l][0]
        i2 = kps_lines[l][1]
        p1 = kps[i1, 0].astype(np.int32), kps[i1, 1].astype(np.int32)
        p2 = kps[i2, 0].astype(np.int32), kps[i2, 1].astype(np.int32)
        #print(i1, i2, p1, p2)
        if 1:
            cv2.line(
                kp_mask, p1, p2,
                color=colors[l], thickness=2, lineType=cv2.LINE_AA)
        if 1:
            cv2.circle(
                kp_mask, p1,
                radius=3, color=colors[l], thickness=-1, lineType=cv2.LINE_AA)
        if 1:
            cv2.circle(
                kp_mask, p2,
                radius=3, color=colors[l], thickness=-1, lineType=cv2.LINE_AA)
    
    # Blend the keypoints.
    return cv2.addWeighted(img, 1.0 - alpha, kp_mask, alpha, 0)

def vis_3dske(ske3d, kps_lines,):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # Convert from plt 0-1 RGBA colors to 0-255 BGR colors for opencv.
    cmap = plt.get_cmap('rainbow')
    colors = [cmap(i) for i in np.linspace(0, 1, len(ske3d) + 2)]
    colors = [np.array((c[2], c[1], c[0])) for c in colors]

    for i in range(len(ske3d)):
        z = ske3d[i][0]
        y = ske3d[i][1]
        x = ske3d[i][2]
        ax.scatter3D(x, -z, -y, c = colors[i])
                    #ax.plot(x, z, -y, c=colors[0], linewidth=2)
     
    ax.set_title('3dske vis')
    ax.set_xlabel('X Label')
    ax.set_ylabel('Z Label')
    ax.set_zlabel('Y Label')
    ax.legend()

    plt.show()
    cv2.waitKey(0)


def vis_3d_skeleton(kpt_3d, kpt_3d_vis, kps_lines, filename=None):

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # Convert from plt 0-1 RGBA colors to 0-255 BGR colors for opencv.
    cmap = plt.get_cmap('rainbow')
    colors = [cmap(i) for i in np.linspace(0, 1, len(kps_lines) + 2)]
    colors = [np.array((c[2], c[1], c[0])) for c in colors]

    for l in range(len(kps_lines)):
        i1 = kps_lines[l][0]
        i2 = kps_lines[l][1]
        x = np.array([kpt_3d[i1,0], kpt_3d[i2,0]])
        y = np.array([kpt_3d[i1,1], kpt_3d[i2,1]])
        z = np.array([kpt_3d[i1,2], kpt_3d[i2,2]])

        if kpt_3d_vis[i1,0] > 0 and kpt_3d_vis[i2,0] > 0:
            ax.plot(x, z, y, c=colors[l], linewidth=2)
        if kpt_3d_vis[i1,0] > 0:
            ax.scatter(kpt_3d[i1,0], kpt_3d[i1,2], kpt_3d[i1,1], c=colors[l], marker='o')
        if kpt_3d_vis[i2,0] > 0:
            ax.scatter(kpt_3d[i2,0], kpt_3d[i2,2], kpt_3d[i2,1], c=colors[l], marker='o')

    #x_r = np.array([0, input_shape[1]], dtype=np.float32)
    #y_r = np.array([0, input_shape[0]], dtype=np.float32)
    #z_r = np.array([0, 1], dtype=np.float32)
    
    if filename is None:
        ax.set_title('3D vis')
    else:
        ax.set_title(filename)

    ax.set_xlabel('X Label')
    ax.set_ylabel('Z Label')
    ax.set_zlabel('Y Label')
    #ax.set_xlim([0,input_shape[1]])
    #ax.set_ylim([0,1])
    #ax.set_zlim([-input_shape[0],0])
    ax.legend()

    plt.show()
    cv2.waitKey(1)


# Pose utils


def lineseg_dist(p, a, b):

    # normalized tangent vector
    d = np.divide(b - a, np.linalg.norm(b - a))

    # signed parallel distance components
    s = np.dot(a - p, d)
    t = np.dot(p - b, d)

    # clamped parallel distance
    h = np.maximum.reduce([s, t, 0])

    # perpendicular distance component
    c = np.cross(p - a, d)

    return np.hypot(h, np.linalg.norm(c))


def cam2pixel(cam_coord, f, c):

    x = cam_coord[..., 0] / cam_coord[..., 2] * f[0] + c[0]
    y = cam_coord[..., 1] / cam_coord[..., 2] * f[1] + c[1]
    z = cam_coord[..., 2]
    
    return x,y,z

def pixel2cam(pixel_coord, f, c):

    x = (pixel_coord[:, 0] - c[0]) / f[0] * pixel_coord[:, 2]
    y = (pixel_coord[:, 1] - c[1]) / f[1] * pixel_coord[:, 2]
    z = pixel_coord[:, 2]
    return x, y, z

def rigid_align(A, B):
    R, t = rigid_transform_3D(A, B)
    A2 = np.transpose(np.dot(R, np.transpose(A))) + t
    return A2


def process_world_coordinate(joint_world, root_idx, joint_num, R, T, f, c):

    # project world coordinates to image space
    joint_cam = np.zeros((joint_num, 3))
    for i in range(joint_num):
        joint_cam[i] = np.dot(R, joint_world[i] - T)
    center_cam = joint_cam[root_idx]

    # Subtract center depth
    joint_img = np.zeros((joint_num, 3))
    joint_img[:, 0], joint_img[:, 1], joint_img[:, 2] = cam2pixel(joint_cam, f, c)
    joint_img[:, 2] = joint_img[:, 2] - center_cam[2]
    joint_vis = np.ones((joint_num,1))
    
    ## bbox3d
    # build 3D bounding box centered on center_cam, sized with bbox_3d_shape
    bbox3d_lt = center_cam - np.array([bbox_3d_shape[2] / 2, bbox_3d_shape[1] / 2, 0])
    bbox3d_rb = center_cam + np.array([bbox_3d_shape[2] / 2, bbox_3d_shape[1] / 2, 0])

    # back-project 3D BBox to 2D image
    bbox2d_l, bbox2d_t, _ = cam2pixel(bbox3d_lt, f, c)
    bbox2d_r, bbox2d_b, _ = cam2pixel(bbox3d_rb, f, c)
    bbox = np.array([bbox2d_l, bbox2d_t, bbox2d_r-bbox2d_l+1, bbox2d_b-bbox2d_t+1])

    return joint_img, joint_cam, joint_vis, center_cam, bbox

def warp_coord_to_original(joint_out, bbox, center_cam):

    # joint_out: output from soft-argmax
    x = joint_out[:, 0] / output_shape * bbox[2] + bbox[0]
    y = joint_out[:, 1] / output_shape * bbox[3] + bbox[1]
    z = (joint_out[:, 2] / depth_dim * 2. - 1.) * (bbox_3d_shape[2]/2.) + center_cam[2]

    return x, y, z


# helper functions
def transform_joint_to_other_db(src_joint, src_name, dst_name):

    src_joint_num = len(src_name)
    dst_joint_num = len(dst_name)

    new_joint = np.zeros(((dst_joint_num,) + src_joint.shape[1:]))

    for src_idx in range(len(src_name)):
        name = src_name[src_idx]
        if name in dst_name:
            dst_idx = dst_name.index(name)
            new_joint[dst_idx] = src_joint[src_idx]

    return new_joint



## Data parallel
"""Encoding Data Parallel"""
import threading
import functools
from torch.autograd import Variable, Function
import torch.cuda.comm as comm
from torch.nn.parallel.data_parallel import DataParallel
from torch.nn.parallel.parallel_apply import get_a_var
from torch.nn.parallel._functions import ReduceAddCoalesced, Broadcast

torch_ver = torch.__version__[:3]

__all__ = ['allreduce', 'DataParallelModel', 'DataParallelCriterion',
           'patch_replication_callback']

def allreduce(*inputs):
    """Cross GPU all reduce autograd operation for calculate mean and
    variance in SyncBN.
    """
    return AllReduce.apply(*inputs)


class AllReduce(Function):
    @staticmethod
    def forward(ctx, num_inputs, *inputs):
        ctx.num_inputs = num_inputs
        ctx.target_gpus = [inputs[i].get_device() for i in range(0, len(inputs), num_inputs)]
        inputs = [inputs[i:i + num_inputs]
                 for i in range(0, len(inputs), num_inputs)]
        # sort before reduce sum
        inputs = sorted(inputs, key=lambda i: i[0].get_device())
        results = comm.reduce_add_coalesced(inputs, ctx.target_gpus[0])
        outputs = comm.broadcast_coalesced(results, ctx.target_gpus)
        return tuple([t for tensors in outputs for t in tensors])

    @staticmethod
    def backward(ctx, *inputs):
        inputs = [i.data for i in inputs]
        inputs = [inputs[i:i + ctx.num_inputs]
                 for i in range(0, len(inputs), ctx.num_inputs)]
        results = comm.reduce_add_coalesced(inputs, ctx.target_gpus[0])
        outputs = comm.broadcast_coalesced(results, ctx.target_gpus)
        return (None,) + tuple([Variable(t) for tensors in outputs for t in tensors])


class Reduce(Function):
    @staticmethod
    def forward(ctx, *inputs):
        ctx.target_gpus = [inputs[i].get_device() for i in range(len(inputs))]
        inputs = sorted(inputs, key=lambda i: i.get_device())
        return comm.reduce_add(inputs)

    @staticmethod
    def backward(ctx, gradOutput):
        return Broadcast.apply(ctx.target_gpus, gradOutput)


class DataParallelModel(DataParallel):
    """Implements data parallelism at the module level.

    This container parallelizes the application of the given module by
    splitting the input across the specified devices by chunking in the
    batch dimension.
    In the forward pass, the module is replicated on each device,
    and each replica handles a portion of the input. During the backwards pass, gradients from each replica are summed into the original module.
    Note that the outputs are not gathered, please use compatible
    :class:`encoding.parallel.DataParallelCriterion`.

    The batch size should be larger than the number of GPUs used. It should
    also be an integer multiple of the number of GPUs so that each chunk is
    the same size (so that each GPU processes the same number of samples).

    Args:
        module: module to be parallelized
        device_ids: CUDA devices (default: all devices)

    Reference:
        Hang Zhang, Kristin Dana, Jianping Shi, Zhongyue Zhang, Xiaogang Wang, Ambrish Tyagi,
        Amit Agrawal. Context Encoding for Semantic Segmentation.
        *The IEEE Conference on Computer Vision and Pattern Recognition (CVPR) 2018*

    Example::

        >>> net = encoding.nn.DataParallelModel(model, device_ids=[0, 1, 2])
        >>> y = net(x)
    """
    def gather(self, outputs, output_device):
        return outputs

    def replicate(self, module, device_ids):
        modules = super(DataParallelModel, self).replicate(module, device_ids)
        execute_replication_callbacks(modules)
        return modules



class DataParallelCriterion(DataParallel):
    """
    Calculate loss in multiple-GPUs, which balance the memory usage for
    Semantic Segmentation.

    The targets are splitted across the specified devices by chunking in
    the batch dimension. Please use together with :class:`encoding.parallel.DataParallelModel`.

    Reference:
        Hang Zhang, Kristin Dana, Jianping Shi, Zhongyue Zhang, Xiaogang Wang, Ambrish Tyagi,
        Amit Agrawal. Context Encoding for Semantic Segmentation.
        *The IEEE Conference on Computer Vision and Pattern Recognition (CVPR) 2018*

    Example::

        >>> net = encoding.nn.DataParallelModel(model, device_ids=[0, 1, 2])
        >>> criterion = encoding.nn.DataParallelCriterion(criterion, device_ids=[0, 1, 2])
        >>> y = net(x)
        >>> loss = criterion(y, target)
    """
    def forward(self, inputs, *targets, **kwargs):
        # input should be already scatterd
        # scattering the targets instead
        # if not self.device_ids:
            # return self.module(inputs, *targets, **kwargs)
        targets, kwargs = self.scatter(targets, kwargs, self.device_ids)
        if len(self.device_ids) == 1:
            return self.module(inputs, *targets[0])
        replicas = self.replicate(self.module, self.device_ids[:len(inputs)])
        outputs = _criterion_parallel_apply(replicas, inputs, targets, kwargs)
        return Reduce.apply(*outputs) / len(outputs)

        #return self.gather(outputs, self.output_device).mean()


def _criterion_parallel_apply(modules, inputs, targets, kwargs_tup=None, devices=None):
    assert len(modules) == len(inputs)
    assert len(targets) == len(inputs)
    if kwargs_tup:
        assert len(modules) == len(kwargs_tup)
    else:
        kwargs_tup = ({},) * len(modules)
    if devices is not None:
        assert len(modules) == len(devices)
    else:
        devices = [None] * len(modules)

    lock = threading.Lock()
    results = {}
    if torch_ver != "0.3":
        grad_enabled = torch.is_grad_enabled()

    def _worker(i, module, input, target, kwargs, device=None):
        if torch_ver != "0.3":
            torch.set_grad_enabled(grad_enabled)
        if device is None:
            device = get_a_var(input).get_device()
        try:
            with torch.cuda.device(device):
                output = module(input, *target)
            with lock:
                results[i] = output
        except Exception as e:
            with lock:
                results[i] = e

    if len(modules) > 1:
        threads = [threading.Thread(target=_worker,
                                    args=(i, module, input, target,
                                          kwargs, device),)
                   for i, (module, input, target, kwargs, device) in
                   enumerate(zip(modules, inputs, targets, kwargs_tup, devices))]

        for thread in threads:
            thread.start()
        for thread in threads:
            thread.join()
    else:
        _worker(0, modules[0], inputs[0], kwargs_tup[0], devices[0])

    outputs = []
    for i in range(len(inputs)):
        output = results[i]
        if isinstance(output, Exception):
            raise output
        outputs.append(output)
    return outputs


###########################################################################
# Adapted from Synchronized-BatchNorm-PyTorch.
# https://github.com/vacancy/Synchronized-BatchNorm-PyTorch
#
class CallbackContext(object):
    pass


def execute_replication_callbacks(modules):
    """
    Execute an replication callback `__data_parallel_replicate__` on each module created
    by original replication.

    The callback will be invoked with arguments `__data_parallel_replicate__(ctx, copy_id)`

    Note that, as all modules are isomorphism, we assign each sub-module with a context
    (shared among multiple copies of this module on different devices).
    Through this context, different copies can share some information.

    We guarantee that the callback on the master copy (the first copy) will be called ahead
    of calling the callback of any slave copies.
    """
    master_copy = modules[0]
    nr_modules = len(list(master_copy.modules()))
    ctxs = [CallbackContext() for _ in range(nr_modules)]

    for i, module in enumerate(modules):
        for j, m in enumerate(module.modules()):
            if hasattr(m, '__data_parallel_replicate__'):
                m.__data_parallel_replicate__(ctxs[j], i)


def patch_replication_callback(data_parallel):
    """
    Monkey-patch an existing `DataParallel` object. Add the replication callback.
    Useful when you have customized `DataParallel` implementation.

    Examples:
        > sync_bn = SynchronizedBatchNorm1d(10, eps=1e-5, affine=False)
        > sync_bn = DataParallel(sync_bn, device_ids=[0, 1])
        > patch_replication_callback(sync_bn)
        # this is equivalent to
        > sync_bn = SynchronizedBatchNorm1d(10, eps=1e-5, affine=False)
        > sync_bn = DataParallelWithCallback(sync_bn, device_ids=[0, 1])
    """

    assert isinstance(data_parallel, DataParallel)

    old_replicate = data_parallel.replicate

    @functools.wraps(old_replicate)
    def new_replicate(module, device_ids):
        modules = old_replicate(module, device_ids)
        execute_replication_callbacks(modules)
        return modules

    data_parallel.replicate = new_replicate




def rotate_2d(pt_2d, rot_rad):
    x = pt_2d[0]
    y = pt_2d[1]
    sn, cs = np.sin(rot_rad), np.cos(rot_rad)
    xx = x * cs - y * sn
    yy = x * sn + y * cs
    return np.array([xx, yy], dtype=np.float32)

def trans_point2d(pt_2d, trans):
    src_pt = np.array([pt_2d[0], pt_2d[1], 1.]).T
    dst_pt = np.dot(trans, src_pt)
    return dst_pt[0:2]

def gen_trans_from_patch_cv(c_x, c_y, src_width, src_height, dst_width, dst_height, scale, rot, inv=False):
    # augment size with scale
    src_w = src_width * scale
    src_h = src_height * scale
    src_center = np.array([c_x, c_y], dtype=np.float32)
    # augment rotation
    rot_rad = np.pi * rot / 180
    src_downdir = rotate_2d(np.array([0, src_h * 0.5], dtype=np.float32), rot_rad)
    src_rightdir = rotate_2d(np.array([src_w * 0.5, 0], dtype=np.float32), rot_rad)

    dst_w = dst_width
    dst_h = dst_height
    dst_center = np.array([dst_w * 0.5, dst_h * 0.5], dtype=np.float32)
    dst_downdir = np.array([0, dst_h * 0.5], dtype=np.float32)
    dst_rightdir = np.array([dst_w * 0.5, 0], dtype=np.float32)

    src = np.zeros((3, 2), dtype=np.float32)
    src[0, :] = src_center
    src[1, :] = src_center + src_downdir
    src[2, :] = src_center + src_rightdir

    dst = np.zeros((3, 2), dtype=np.float32)
    dst[0, :] = dst_center
    dst[1, :] = dst_center + dst_downdir
    dst[2, :] = dst_center + dst_rightdir

    if inv:
        trans = cv2.getAffineTransform(np.float32(dst), np.float32(src))
    else:
        trans = cv2.getAffineTransform(np.float32(src), np.float32(dst))

    return trans

def generate_patch_image(cvimg, bbox, do_flip, scale, rot):
    img = cvimg.copy()
    img_height, img_width, img_channels = img.shape

    bb_c_x = float(bbox[0] + 0.5*bbox[2])
    bb_c_y = float(bbox[1] + 0.5*bbox[3])
    bb_width = float(bbox[2])
    bb_height = float(bbox[3])

    if do_flip:
        img = img[:, ::-1, :]
        bb_c_x = img_width - bb_c_x - 1

    trans = gen_trans_from_patch_cv(bb_c_x, bb_c_y, bb_width, bb_height, img_size, img_size, scale, rot, inv=False)
    img_patch = cv2.warpAffine(img, trans, (img_size, img_size), flags=cv2.INTER_LINEAR)
    #print(img_patch)
    
    img_patch = img_patch[:,:,::-1].copy()
    img_patch = img_patch.astype(np.float32)
    return img_patch, trans


def get_aug_config():
    
    scale_factor = 0.25
    rot_factor = 30
    color_factor = 0.0
    
    scale = np.clip(np.random.randn(), -1.0, 1.0) * scale_factor + 1.0
    rot = np.clip(np.random.randn(), -2.0,
                  2.0) * rot_factor if random.random() <= 0.6 else 0
    do_flip = random.random() <= 0.5
    c_up = 1.0 + color_factor
    c_low = 1.0 - color_factor
    color_scale = [random.uniform(c_low, c_up), random.uniform(c_low, c_up), random.uniform(c_low, c_up)]

    return scale, rot, do_flip, color_scale

do_aug = True

log_interval = 1

import matplotlib.pyplot as plt
import matplotlib.image as mpimg

# 1. losses
def LocalContrastNorm(image,radius=9):
    """
    image: torch.Tensor , .shape => (1,channels,height,width) 
    
    radius: Gaussian filter size (int), odd
    """
    if radius%2 == 0:
        radius += 1
    def get_gaussian_filter(kernel_shape):
        x = np.zeros(kernel_shape, dtype='float64')
 
        def gauss(x, y, sigma=2.0):
            Z = 2 * np.pi * sigma ** 2
            return  1. / Z * np.exp(-(x ** 2 + y ** 2) / (2. * sigma ** 2))
 
        mid = np.floor(kernel_shape[-1] / 2.)
        for kernel_idx in range(0, kernel_shape[1]):
            for i in range(0, kernel_shape[2]):
                for j in range(0, kernel_shape[3]):
                    x[0, kernel_idx, i, j] = gauss(i - mid, j - mid)
 
        return x / np.sum(x)
    
    n,c,h,w = image.shape[0],image.shape[1],image.shape[2],image.shape[3]

    gaussian_filter = torch.Tensor(get_gaussian_filter((1,c,radius,radius)))
    filtered_out = F.conv2d(image,gaussian_filter,padding=radius-1)
    mid = int(np.floor(gaussian_filter.shape[2] / 2.))
    ### Subtractive Normalization
    centered_image = image - filtered_out[:,:,mid:-mid,mid:-mid]
    
    ## Variance Calc
    sum_sqr_image = F.conv2d(centered_image.pow(2),gaussian_filter,padding=radius-1)
    s_deviation = sum_sqr_image[:,:,mid:-mid,mid:-mid].sqrt()
    per_img_mean = s_deviation.mean()
    
    ## Divisive Normalization
    divisor = np.maximum(per_img_mean.numpy(),s_deviation.numpy())
    divisor = np.maximum(divisor, 1e-4)
    new_image = centered_image / torch.Tensor(divisor)
    return new_image

def _assert_no_grad(tensor):
    assert not tensor.requires_grad, \
        "nn criterions don't compute the gradient w.r.t. targets - please " \
        "mark these tensors as not requiring gradients"

def soft_argmax(heatmaps, joint_num):
    assert isinstance(heatmaps, torch.Tensor)

    heatmaps = heatmaps.reshape((-1, joint_num, depth_dim*output_shape * output_shape))
    heatmaps = F.softmax(heatmaps, 2)
    heatmaps = heatmaps.reshape((-1, joint_num, depth_dim, output_shape, output_shape))

    accu_x = heatmaps.sum(dim=(2,3))
    accu_y = heatmaps.sum(dim=(2,4))
    accu_z = heatmaps.sum(dim=(3,4))

    accu_x = accu_x * torch.cuda.comm.broadcast(torch.arange(1,output_shape+1).type(torch.cuda.FloatTensor), devices=[accu_x.device.index])[0]
    accu_y = accu_y * torch.cuda.comm.broadcast(torch.arange(1,output_shape+1).type(torch.cuda.FloatTensor), devices=[accu_y.device.index])[0]
    accu_z = accu_z * torch.cuda.comm.broadcast(torch.arange(1,depth_dim+1).type(torch.cuda.FloatTensor), devices=[accu_z.device.index])[0]

    accu_x = accu_x.sum(dim=2, keepdim=True) -1
    accu_y = accu_y.sum(dim=2, keepdim=True) -1
    accu_z = accu_z.sum(dim=2, keepdim=True) -1

    coord_out = torch.cat((accu_x, accu_y, accu_z), dim=2)
    #print(coord_out)
    return coord_out

class JointLocationLoss(nn.Module):
    def __init__(self):
        super(JointLocationLoss, self).__init__()

    def forward(self, heatmap_out, gt_coord, gt_vis, gt_have_depth, count_only_depth = False):
        
        joint_num = gt_coord.shape[1]
        coord_out = soft_argmax(heatmap_out, joint_num)

        _assert_no_grad(gt_coord)
        _assert_no_grad(gt_vis)
        _assert_no_grad(gt_have_depth)
        
        loss = torch.abs(coord_out - gt_coord) * gt_vis
        
        if count_only_depth == True:
            loss = (loss[:,:,2] * gt_have_depth)/1.
        else:
            loss = (loss[:,:,0] + loss[:,:,1] + loss[:,:,2] * gt_have_depth)/3.
        
        return loss.mean()


# 2. construct dataset
import torch
from torch.utils.data import Dataset
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torchvision import datasets, transforms
from PIL import Image
import math
import copy



momentum = 0.9

epochs = 10000000

import numpy as np


#class for MPII
import json

## coco stuff
class COCO:
    def __init__(self, annotation_file=None):
        """
        Constructor of Microsoft COCO helper class for reading and visualizing annotations.
        :param annotation_file (str): location of annotation file
        :param image_folder (str): location to the folder that hosts images.
        :return:
        """
        # load dataset
        self.dataset,self.anns,self.cats,self.imgs = dict(),dict(),dict(),dict()
        self.imgToAnns, self.catToImgs = defaultdict(list), defaultdict(list)
        if not annotation_file == None:
            print('loading annotations into memory...')
            tic = time.time()
            dataset = json.load(open(annotation_file, 'r'))
            assert type(dataset)==dict, 'annotation file format {} not supported'.format(type(dataset))
            print('Done (t={:0.2f}s)'.format(time.time()- tic))
            self.dataset = dataset
            self.createIndex()

    def createIndex(self):
        # create index
        print('creating index...')
        anns, cats, imgs = {}, {}, {}
        imgToAnns,catToImgs = defaultdict(list),defaultdict(list)
        if 'annotations' in self.dataset:
            for ann in self.dataset['annotations']:
                imgToAnns[ann['image_id']].append(ann)
                anns[ann['id']] = ann

        if 'images' in self.dataset:
            for img in self.dataset['images']:
                imgs[img['id']] = img

        if 'categories' in self.dataset:
            for cat in self.dataset['categories']:
                cats[cat['id']] = cat

        if 'annotations' in self.dataset and 'categories' in self.dataset:
            for ann in self.dataset['annotations']:
                catToImgs[ann['category_id']].append(ann['image_id'])

        print('index created!')

        # create class members
        self.anns = anns
        self.imgToAnns = imgToAnns
        self.catToImgs = catToImgs
        self.imgs = imgs
        self.cats = cats

    def info(self):
        """
        Print information about the annotation file.
        :return:
        """
        for key, value in self.dataset['info'].items():
            print('{}: {}'.format(key, value))

    def getAnnIds(self, imgIds=[], catIds=[], areaRng=[], iscrowd=None):
        """
        Get ann ids that satisfy given filter conditions. default skips that filter
        :param imgIds  (int array)     : get anns for given imgs
               catIds  (int array)     : get anns for given cats
               areaRng (float array)   : get anns for given area range (e.g. [0 inf])
               iscrowd (boolean)       : get anns for given crowd label (False or True)
        :return: ids (int array)       : integer array of ann ids
        """
        imgIds = imgIds if _isArrayLike(imgIds) else [imgIds]
        catIds = catIds if _isArrayLike(catIds) else [catIds]

        if len(imgIds) == len(catIds) == len(areaRng) == 0:
            anns = self.dataset['annotations']
        else:
            if not len(imgIds) == 0:
                lists = [self.imgToAnns[imgId] for imgId in imgIds if imgId in self.imgToAnns]
                anns = list(itertools.chain.from_iterable(lists))
            else:
                anns = self.dataset['annotations']
            anns = anns if len(catIds)  == 0 else [ann for ann in anns if ann['category_id'] in catIds]
            anns = anns if len(areaRng) == 0 else [ann for ann in anns if ann['area'] > areaRng[0] and ann['area'] < areaRng[1]]
        if not iscrowd == None:
            ids = [ann['id'] for ann in anns if ann['iscrowd'] == iscrowd]
        else:
            ids = [ann['id'] for ann in anns]
        return ids

    def getCatIds(self, catNms=[], supNms=[], catIds=[]):
        """
        filtering parameters. default skips that filter.
        :param catNms (str array)  : get cats for given cat names
        :param supNms (str array)  : get cats for given supercategory names
        :param catIds (int array)  : get cats for given cat ids
        :return: ids (int array)   : integer array of cat ids
        """
        catNms = catNms if _isArrayLike(catNms) else [catNms]
        supNms = supNms if _isArrayLike(supNms) else [supNms]
        catIds = catIds if _isArrayLike(catIds) else [catIds]

        if len(catNms) == len(supNms) == len(catIds) == 0:
            cats = self.dataset['categories']
        else:
            cats = self.dataset['categories']
            cats = cats if len(catNms) == 0 else [cat for cat in cats if cat['name']          in catNms]
            cats = cats if len(supNms) == 0 else [cat for cat in cats if cat['supercategory'] in supNms]
            cats = cats if len(catIds) == 0 else [cat for cat in cats if cat['id']            in catIds]
        ids = [cat['id'] for cat in cats]
        return ids

    def getImgIds(self, imgIds=[], catIds=[]):
        '''
        Get img ids that satisfy given filter conditions.
        :param imgIds (int array) : get imgs for given ids
        :param catIds (int array) : get imgs with all given cats
        :return: ids (int array)  : integer array of img ids
        '''
        imgIds = imgIds if _isArrayLike(imgIds) else [imgIds]
        catIds = catIds if _isArrayLike(catIds) else [catIds]

        if len(imgIds) == len(catIds) == 0:
            ids = self.imgs.keys()
        else:
            ids = set(imgIds)
            for i, catId in enumerate(catIds):
                if i == 0 and len(ids) == 0:
                    ids = set(self.catToImgs[catId])
                else:
                    ids &= set(self.catToImgs[catId])
        return list(ids)

    def loadAnns(self, ids=[]):
        """
        Load anns with the specified ids.
        :param ids (int array)       : integer ids specifying anns
        :return: anns (object array) : loaded ann objects
        """
        if _isArrayLike(ids):
            return [self.anns[id] for id in ids]
        elif type(ids) == int:
            return [self.anns[ids]]

    def loadCats(self, ids=[]):
        """
        Load cats with the specified ids.
        :param ids (int array)       : integer ids specifying cats
        :return: cats (object array) : loaded cat objects
        """
        if _isArrayLike(ids):
            return [self.cats[id] for id in ids]
        elif type(ids) == int:
            return [self.cats[ids]]

    def loadImgs(self, ids=[]):
        """
        Load anns with the specified ids.
        :param ids (int array)       : integer ids specifying img
        :return: imgs (object array) : loaded img objects
        """
        if _isArrayLike(ids):
            return [self.imgs[id] for id in ids]
        elif type(ids) == int:
            return [self.imgs[ids]]

    def showAnns(self, anns, draw_bbox=False):
        """
        Display the specified annotations.
        :param anns (array of object): annotations to display
        :return: None
        """
        if len(anns) == 0:
            return 0
        if 'segmentation' in anns[0] or 'keypoints' in anns[0]:
            datasetType = 'instances'
        elif 'caption' in anns[0]:
            datasetType = 'captions'
        else:
            raise Exception('datasetType not supported')
        if datasetType == 'instances':
            ax = plt.gca()
            ax.set_autoscale_on(False)
            polygons = []
            color = []
            for ann in anns:
                c = (np.random.random((1, 3))*0.6+0.4).tolist()[0]
                if 'segmentation' in ann:
                    if type(ann['segmentation']) == list:
                        # polygon
                        for seg in ann['segmentation']:
                            poly = np.array(seg).reshape((int(len(seg)/2), 2))
                            polygons.append(Polygon(poly))
                            color.append(c)
                    else:
                        # mask
                        t = self.imgs[ann['image_id']]
                        if type(ann['segmentation']['counts']) == list:
                            rle = maskUtils.frPyObjects([ann['segmentation']], t['height'], t['width'])
                        else:
                            rle = [ann['segmentation']]
                        m = maskUtils.decode(rle)
                        img = np.ones( (m.shape[0], m.shape[1], 3) )
                        if ann['iscrowd'] == 1:
                            color_mask = np.array([2.0,166.0,101.0])/255
                        if ann['iscrowd'] == 0:
                            color_mask = np.random.random((1, 3)).tolist()[0]
                        for i in range(3):
                            img[:,:,i] = color_mask[i]
                        ax.imshow(np.dstack( (img, m*0.5) ))
                if 'keypoints' in ann and type(ann['keypoints']) == list:
                    # turn skeleton into zero-based index
                    sks = np.array(self.loadCats(ann['category_id'])[0]['skeleton'])-1
                    kp = np.array(ann['keypoints'])
                    x = kp[0::3]
                    y = kp[1::3]
                    v = kp[2::3]
                    for sk in sks:
                        if np.all(v[sk]>0):
                            plt.plot(x[sk],y[sk], linewidth=3, color=c)
                    plt.plot(x[v>0], y[v>0],'o',markersize=8, markerfacecolor=c, markeredgecolor='k',markeredgewidth=2)
                    plt.plot(x[v>1], y[v>1],'o',markersize=8, markerfacecolor=c, markeredgecolor=c, markeredgewidth=2)

                if draw_bbox:
                    [bbox_x, bbox_y, bbox_w, bbox_h] = ann['bbox']
                    poly = [[bbox_x, bbox_y], [bbox_x, bbox_y+bbox_h], [bbox_x+bbox_w, bbox_y+bbox_h], [bbox_x+bbox_w, bbox_y]]
                    np_poly = np.array(poly).reshape((4,2))
                    polygons.append(Polygon(np_poly))
                    color.append(c)

            p = PatchCollection(polygons, facecolor=color, linewidths=0, alpha=0.4)
            ax.add_collection(p)
            p = PatchCollection(polygons, facecolor='none', edgecolors=color, linewidths=2)
            ax.add_collection(p)
        elif datasetType == 'captions':
            for ann in anns:
                print(ann['caption'])

    def loadRes(self, resFile):
        """
        Load result file and return a result api object.
        :param   resFile (str)     : file name of result file
        :return: res (obj)         : result api object
        """
        res = COCO()
        res.dataset['images'] = [img for img in self.dataset['images']]

        print('Loading and preparing results...')
        tic = time.time()
        if type(resFile) == str or (PYTHON_VERSION == 2 and type(resFile) == unicode):
            anns = json.load(open(resFile))
        elif type(resFile) == np.ndarray:
            anns = self.loadNumpyAnnotations(resFile)
        else:
            anns = resFile
        assert type(anns) == list, 'results in not an array of objects'
        annsImgIds = [ann['image_id'] for ann in anns]
        assert set(annsImgIds) == (set(annsImgIds) & set(self.getImgIds())), \
               'Results do not correspond to current coco set'
        if 'caption' in anns[0]:
            imgIds = set([img['id'] for img in res.dataset['images']]) & set([ann['image_id'] for ann in anns])
            res.dataset['images'] = [img for img in res.dataset['images'] if img['id'] in imgIds]
            for id, ann in enumerate(anns):
                ann['id'] = id+1
        elif 'bbox' in anns[0] and not anns[0]['bbox'] == []:
            res.dataset['categories'] = copy.deepcopy(self.dataset['categories'])
            for id, ann in enumerate(anns):
                bb = ann['bbox']
                x1, x2, y1, y2 = [bb[0], bb[0]+bb[2], bb[1], bb[1]+bb[3]]
                if not 'segmentation' in ann:
                    ann['segmentation'] = [[x1, y1, x1, y2, x2, y2, x2, y1]]
                ann['area'] = bb[2]*bb[3]
                ann['id'] = id+1
                ann['iscrowd'] = 0
        elif 'segmentation' in anns[0]:
            res.dataset['categories'] = copy.deepcopy(self.dataset['categories'])
            for id, ann in enumerate(anns):
                # now only support compressed RLE format as segmentation results
                ann['area'] = maskUtils.area(ann['segmentation'])
                if not 'bbox' in ann:
                    ann['bbox'] = maskUtils.toBbox(ann['segmentation'])
                ann['id'] = id+1
                ann['iscrowd'] = 0
        elif 'keypoints' in anns[0]:
            res.dataset['categories'] = copy.deepcopy(self.dataset['categories'])
            for id, ann in enumerate(anns):
                s = ann['keypoints']
                x = s[0::3]
                y = s[1::3]
                x0,x1,y0,y1 = np.min(x), np.max(x), np.min(y), np.max(y)
                ann['area'] = (x1-x0)*(y1-y0)
                ann['id'] = id + 1
                ann['bbox'] = [x0,y0,x1-x0,y1-y0]
        print('DONE (t={:0.2f}s)'.format(time.time()- tic))

        res.dataset['annotations'] = anns
        res.createIndex()
        return res

    def download(self, tarDir = None, imgIds = [] ):
        '''
        Download COCO images from mscoco.org server.
        :param tarDir (str): COCO results directory name
               imgIds (list): images to be downloaded
        :return:
        '''
        if tarDir is None:
            print('Please specify target directory')
            return -1
        if len(imgIds) == 0:
            imgs = self.imgs.values()
        else:
            imgs = self.loadImgs(imgIds)
        N = len(imgs)
        if not os.path.exists(tarDir):
            os.makedirs(tarDir)
        for i, img in enumerate(imgs):
            tic = time.time()
            fname = os.path.join(tarDir, img['file_name'])
            if not os.path.exists(fname):
                urlretrieve(img['coco_url'], fname)
            print('downloaded {}/{} images (t={:0.1f}s)'.format(i, N, time.time()- tic))

    def loadNumpyAnnotations(self, data):
        """
        Convert result data from a numpy array [Nx7] where each row contains {imageID,x1,y1,w,h,score,class}
        :param  data (numpy.ndarray)
        :return: annotations (python nested list)
        """
        print('Converting ndarray to lists...')
        assert(type(data) == np.ndarray)
        print(data.shape)
        assert(data.shape[1] == 7)
        N = data.shape[0]
        ann = []
        for i in range(N):
            if i % 1000000 == 0:
                print('{}/{}'.format(i,N))
            ann += [{
                'image_id'  : int(data[i, 0]),
                'bbox'  : [ data[i, 1], data[i, 2], data[i, 3], data[i, 4] ],
                'score' : data[i, 5],
                'category_id': int(data[i, 6]),
                }]
        return ann

    def annToRLE(self, ann):
        """
        Convert annotation which can be polygons, uncompressed RLE to RLE.
        :return: binary mask (numpy 2D array)
        """
        t = self.imgs[ann['image_id']]
        h, w = t['height'], t['width']
        segm = ann['segmentation']
        if type(segm) == list:
            # polygon -- a single object might consist of multiple parts
            # we merge all parts into one mask rle code
            rles = maskUtils.frPyObjects(segm, h, w)
            rle = maskUtils.merge(rles)
        elif type(segm['counts']) == list:
            # uncompressed RLE
            rle = maskUtils.frPyObjects(segm, h, w)
        else:
            # rle
            rle = ann['segmentation']
        return rle

    def annToMask(self, ann):
        """
        Convert annotation which can be polygons, uncompressed RLE, or RLE to binary mask.
        :return: binary mask (numpy 2D array)
        """
        rle = self.annToRLE(ann)
        m = maskUtils.decode(rle)
        return m

#class for my MPII labeled data
class MyMPII(Dataset):

    def __init__(self, is_train):
        self.is_train = is_train
        self.content_path_prefix = "D:/mpii/img/"
        
        self.joints_name = ('R_Ankle', 'R_Knee', 'R_Hip', 'L_Hip', 'L_Knee', 'L_Ankle', 'Pelvis', 'Thorax', 'Neck', 'Head', 'R_Wrist', 'R_Elbow', 'R_Shoulder', 'L_Shoulder', 'L_Elbow', 'L_Wrist')
        self.flip_pairs = ( (0, 5), (1, 4), (2, 3), (10, 15), (11, 14), (12, 13) )
        self.skeleton = ( (0, 1), (1, 2), (2, 6), (7, 12), (12, 11), (11, 10), (5, 4), (4, 3), (3, 6), (7, 13), (13, 14), (14, 15), (6, 7), (7, 8), (8, 9) )
        self.ske_colors = (( 85, 152, 188 ),                 
                            ( 199, 153, 96 ),                
                            ( 191, 141, 113 ),               
                            ( 119, 165, 232 ),               
                            ( 140, 139, 66 ),                 
                            ( 84, 83, 232 ),                  
                            ( 234, 218, 188 ),                
                            ( 178, 179, 252 ),               
                            ( 234, 235, 84 ),                 
                            ( 84, 100, 130 ),                
                            ( 230, 30, 210 ),                
                            ( 60, 200, 155),                  
                            ( 180, 20, 60 ),                 
                            ( 128, 128, 128 ),   
                            ( 238,190,77))
    
        self.lr_skeleton = ( ((0,0),(0,0)) ) # dumy value
        self.joints_have_depth = True
        self.transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=pixel_mean, std=pixel_std)])
        if self.is_train:
            self.do_aug = True
        else:
            self.do_aug = False
        self.do_aug = False
        self.lbd_contents = 0
        self.ubd_contents = 25923
        
        if self.is_train == False:
            self.lbd_contents = 0
            self.ubd_contents = 10
        self.minz = -1.1702
        self.maxz = 1.0009
        self.root_id = 7 #thorax
        self.joint_num = 16
        self.jt = np.loadtxt('D:/mpii/gt_joint_2d_train_all.txt')
        self.jt_3d = np.loadtxt('D:/mpii/gt_joint_3d_train_all.txt')

    # The component to load data from the data base
    def load_data(self):
        data = []
        for content_id in range(self.lbd_contents, self.ubd_contents):
            # load image
            content_path_prefix = self.content_path_prefix
            content_path = content_path_prefix + str(content_id) + ".png"
            #content_img = cv2.imread(content_path, cv2.IMREAD_COLOR | cv2.IMREAD_IGNORE_ORIENTATION)
            #if not isinstance(content_img, np.ndarray):
            #    raise IOError("Fail to read %s" % content_path)
            joint_vis = np.zeros((self.joint_num, 1), dtype=np.float)
            for j in range(self.joint_num):
                if self.jt[content_id * self.joint_num + j][0] >= 0.0 and self.jt[content_id * self.joint_num + j][0] <= 1.0 and self.jt[content_id * self.joint_num + j][1] >= 0.0 and self.jt[content_id * self.joint_num + j][1] <= 1.0:
                    joint_vis[j][0] = 1.0
                else:
                    joint_vis[j][0] = 0.0
            joint_img = np.zeros((self.joint_num, 3), dtype=np.float)
            root_z = self.jt_3d[content_id * self.joint_num + self.root_id][2]
            joint_bone = np.zeros((BoneBasedBoneNum * 3), dtype=np.float)
            neg_thorax = np.zeros((3), dtype=np.float)
            thorax_id = 7
            for j in range(3):
                neg_thorax[j] = (-self.jt_3d[content_id * self.joint_num + thorax_id][j] - avg_neg_gt_thorax_arr[j]) / std_neg_gt_thorax_arr[j]
            for j in range(self.joint_num):
                joint_img[j][:2] = self.jt[content_id * self.joint_num + j][:] * 224.0#0.0 - 1.0
                joint_img[j][2] = (self.jt_3d[content_id * self.joint_num + j][2] - root_z) * 1000.0 #h36m -1000, 1000 bbx
                
            #gt bone
            for j in range(BoneBasedBoneNum):
                Uid = content_id * self.joint_num + bones_mpii[j][0]
                Vid = content_id * self.joint_num + bones_mpii[j][1]
                for l in range(3):
                    joint_bone[j * 3 + l] = self.jt_3d[Vid][l] - self.jt_3d[Uid][l]
                    joint_bone[j * 3 + l] = (joint_bone[j * 3 + l] - avg_gt_bone_arr[j * 3 + l]) / std_gt_bone_arr[j * 3 + l]
            f = np.ones(2)
            f[0] = 300.0
            f[1] = 300.0
            c = np.ones(2)
            c[0] = 0.5
            c[1] = 0.5
            center_cam = np.ones(3)
            center_cam[:] = self.jt_3d[content_id * self.joint_num + self.root_id][:]
            data.append({
                'img_path': content_path,
                'bbox': [0.0, 0.0, 224.0, 224.0],
                'joint_img': joint_img, # [org_img_x, org_img_y, 0]
                'joint_cam': np.ones(joint_img.shape), # dummy value
                'joint_vis': joint_vis,
                'center_cam': center_cam, # dummy value
                'f': f, # dummy value
                'c': c, # dummy value,
                'data_source': np.array([2], dtype=np.float32),
                'joint_bone':  joint_bone, #gt bone
                'neg_thorax':  neg_thorax #gt neg thorax
            })
        return data


class COCOTrain2017Dataset(object):
    
    def __init__(self, data_split):
        self.data_split = data_split
        self.img_dir = osp.join('D:/', 'train2017', 'train2017')
        self.annot_path = osp.join('D:/', 'annotations', 'person_keypoints_train2017.json')

        self.joint_num = 17
        self.joints_name = ['Nose', 'L_Eye', 'R_Eye', 'L_Ear', 'R_Ear', 'L_Shoulder', 'R_Shoulder', 'L_Elbow', 'R_Elbow', 'L_Wrist', 'R_Wrist',
    'L_Hip', 'R_Hip', 'L_Knee', 'R_Knee', 'L_Ankle', 'R_Ankle']
    
        self.flip_pairs = ( (0, 5), (1, 4), (2, 3), (10, 15), (11, 14), (12, 13) )
        self.skeleton = ( (5, 7), (6, 8), (7, 9), (8, 10), (11, 13), (13, 15), (12, 14), (14, 16) )
        self.lr_skeleton = ( ((0,0),(0,0)) ) # dumy value
        self.joints_have_depth = False

    
    def load_data(self, score=False):
        coco = COCO(self.annot_path)
        data = []
        for aid in coco.anns.keys():
            ann = coco.anns[aid]
            joints = ann['keypoints']
 
            if (ann['image_id'] not in coco.imgs) or ann['iscrowd'] or (np.sum(joints[2::3]) == 0) or (ann['num_keypoints'] == 0):
                continue
           
            # sanitize bboxes
            x, y, w, h = ann['bbox']
            img = coco.loadImgs(ann['image_id'])[0]
            width, height = img['width'], img['height']
            x1 = np.max((0, x))
            y1 = np.max((0, y))
            x2 = np.min((width - 1, x1 + np.max((0, w - 1))))
            y2 = np.min((height - 1, y1 + np.max((0, h - 1))))
            if ann['area'] > 0 and x2 >= x1 and y2 >= y1:
                bbox = [x1, y1, x2-x1, y2-y1]
            else:
                continue
            w = bbox[2]
            h = bbox[3]
            c_x = bbox[0] + w/2.
            c_y = bbox[1] + h/2.
            
            sz = max(w, h)
            w = sz
            h = sz 
            bbox[2] = w
            bbox[3] = h
            bbox[0] = c_x - bbox[2]/2.
            bbox[1] = c_y - bbox[3]/2.

            joint_img = np.array(ann['keypoints']).reshape(self.joint_num,3)
            joint_vis = joint_img[:,2].copy().reshape(-1,1)
            joint_vis[:,:] = 0
            joint_img[:,2] = 0
            
            imgname = coco.imgs[ann['image_id']]['file_name']
            img_path = osp.join(self.img_dir, imgname)
            joint_bone = np.zeros((BoneBasedBoneNum * 3), dtype=np.float)
            neg_thorax = np.zeros((3), dtype=np.float)
            
            data.append({
                'img_path': img_path,
                'bbox': bbox,
                'joint_img': joint_img, # [org_img_x, org_img_y, 0]
                'joint_cam': np.ones(joint_img.shape), # dummy value
                'joint_vis': joint_vis,
                'center_cam': np.ones(3), # dummy value
                'f': np.ones(2), # dummy value
                'c': np.ones(2), # dummy value,
                'data_source': np.array([3], dtype=np.float32),
                'joint_bone': joint_bone,
                'neg_thorax': neg_thorax
            })


        return data


class COCOVal2017Dataset(object):
    
    def __init__(self, data_split):
        self.data_split = data_split
        self.img_dir = osp.join('D:/', 'val2017')
        self.annot_path = osp.join('D:/', 'annotations', 'person_keypoints_val2017.json')

        self.joint_num = 17
        self.joints_name = ['Nose', 'L_Eye', 'R_Eye', 'L_Ear', 'R_Ear', 'L_Shoulder', 'R_Shoulder', 'L_Elbow', 'R_Elbow', 'L_Wrist', 'R_Wrist',
    'L_Hip', 'R_Hip', 'L_Knee', 'R_Knee', 'L_Ankle', 'R_Ankle']
    
        self.flip_pairs = ( (0, 5), (1, 4), (2, 3), (10, 15), (11, 14), (12, 13) )
        self.skeleton = ( (5, 7), (6, 8), (7, 9), (8, 10), (11, 13), (13, 15), (12, 14), (14, 16) )
        self.lr_skeleton = ( ((0,0),(0,0)) ) # dumy value
        self.joints_have_depth = False

    
    def load_data(self, score=False):
        coco = COCO(self.annot_path)
        data = []
        for aid in coco.anns.keys():
            ann = coco.anns[aid]
            joints = ann['keypoints']
 
            if (ann['image_id'] not in coco.imgs) or ann['iscrowd'] or (np.sum(joints[2::3]) == 0) or (ann['num_keypoints'] == 0):
                continue
           
            # sanitize bboxes
            x, y, w, h = ann['bbox']
            img = coco.loadImgs(ann['image_id'])[0]
            width, height = img['width'], img['height']
            x1 = np.max((0, x))
            y1 = np.max((0, y))
            x2 = np.min((width - 1, x1 + np.max((0, w - 1))))
            y2 = np.min((height - 1, y1 + np.max((0, h - 1))))
            if ann['area'] > 0 and x2 >= x1 and y2 >= y1:
                bbox = [x1, y1, x2-x1, y2-y1]
            else:
                continue
            w = bbox[2]
            h = bbox[3]
            c_x = bbox[0] + w/2.
            c_y = bbox[1] + h/2.
            
            sz = max(w, h)
            w = sz
            h = sz 
            bbox[2] = w
            bbox[3] = h
            bbox[0] = c_x - bbox[2]/2.
            bbox[1] = c_y - bbox[3]/2.

            joint_img = np.array(ann['keypoints']).reshape(self.joint_num,3)
            joint_vis = joint_img[:,2].copy().reshape(-1,1)
            joint_vis[:,:] = 0
            joint_img[:,2] = 0
            
            imgname = coco.imgs[ann['image_id']]['file_name']
            img_path = osp.join(self.img_dir, imgname)
            joint_bone = np.zeros((BoneBasedBoneNum * 3), dtype=np.float)
            neg_thorax = np.zeros((3), dtype=np.float)
            
            data.append({
                'img_path': img_path,
                'bbox': bbox,
                'joint_img': joint_img, # [org_img_x, org_img_y, 0]
                'joint_cam': np.ones(joint_img.shape), # dummy value
                'joint_vis': joint_vis,
                'center_cam': np.ones(3), # dummy value
                'f': np.ones(2), # dummy value
                'c': np.ones(2), # dummy value,
                'data_source': np.array([3], dtype=np.float32),
                'joint_bone': joint_bone,
                'neg_thorax': neg_thorax
            })


        return data

#class for mpii
class MPII:

    def __init__(self, data_split):
        self.data_split = data_split
        self.img_dir = osp.join('mpii')
        self.train_annot_path = osp.join('mpii', 'annotations', 'train.json')
        self.test_annot_path = osp.join('mpii', 'annotations', 'test.json')
        self.joint_num = 16
        self.joints_name = ('R_Ankle', 'R_Knee', 'R_Hip', 'L_Hip', 'L_Knee', 'L_Ankle', 'Pelvis', 'Thorax', 'Neck', 'Head', 'R_Wrist', 'R_Elbow', 'R_Shoulder', 'L_Shoulder', 'L_Elbow', 'L_Wrist')
        self.flip_pairs = ( (0, 5), (1, 4), (2, 3), (10, 15), (11, 14), (12, 13) )
        self.skeleton = ( (0, 1), (1, 2), (2, 6), (7, 12), (12, 11), (11, 10), (5, 4), (4, 3), (3, 6), (7, 13), (13, 14), (14, 15), (6, 7), (7, 8), (8, 9) )
        self.lr_skeleton = ( ((0,0),(0,0)) ) # dumy value

        self.joints_have_depth = False

    def load_data(self):
        
        if self.data_split == 'train':
            #print('Loading ', self.train_annot_path)
        
            db = COCO(self.train_annot_path)
        elif self.data_split == 'test':
            db = COCO(self.test_annot_path)
        else:
            print('Unknown data subset')
            assert 0

        data = []
        for aid in db.anns.keys():
            ann = db.anns[aid]

            if (ann['image_id'] not in db.imgs) or ann['iscrowd'] or (ann['num_keypoints'] == 0):
                continue

            # sanitize bboxes
            x, y, w, h = ann['bbox']
            img = db.loadImgs(ann['image_id'])[0]
            width, height = img['width'], img['height']
            x1 = np.max((0, x))
            y1 = np.max((0, y))
            x2 = np.min((width - 1, x1 + np.max((0, w - 1))))
            y2 = np.min((height - 1, y1 + np.max((0, h - 1))))
            if ann['area'] > 0 and x2 >= x1 and y2 >= y1:
                bbox = np.array([x1, y1, x2-x1, y2-y1])
            else:
                continue
            # aspect ratio preserving bbox
            w = bbox[2]
            h = bbox[3]
            c_x = bbox[0] + w/2.
            c_y = bbox[1] + h/2.
            
            aspect_ratio = 1.0
            if w > aspect_ratio * h:
                h = w / aspect_ratio
            elif w < aspect_ratio * h:
                w = h * aspect_ratio
            bbox[2] = w#*1.25
            bbox[3] = h#*1.25
            bbox[0] = c_x - bbox[2]/2.
            bbox[1] = c_y - bbox[3]/2.

            # joints and vis
            if self.data_split == 'train':
                joint_img = np.array(ann['keypoints']).reshape(self.joint_num,3)
                joint_vis = joint_img[:,2].copy().reshape(-1,1)
                joint_img[:,2] = 0
            else:
                joint_img = np.zeros((self.joint_num, 3), dtype=np.float)
                joint_vis = np.zeros((self.joint_num, 1), dtype=np.float)

            imgname = db.imgs[ann['image_id']]['file_name']
            img_path = osp.join(self.img_dir, imgname)
            joint_bone = np.zeros((BoneBasedBoneNum * 3), dtype=np.float)
            neg_thorax = np.zeros((3), dtype=np.float)
            
            data.append({
                'img_path': img_path,
                'bbox': bbox,
                'joint_img': joint_img, # [org_img_x, org_img_y, 0]
                'joint_cam': np.ones(joint_img.shape), # dummy value
                'joint_vis': joint_vis,
                'center_cam': np.ones(3), # dummy value
                'f': np.ones(2), # dummy value
                'c': np.ones(2), # dummy value,
                'data_source': np.array([1], dtype=np.float32),
                'joint_bone': joint_bone,
                'neg_thorax': neg_thorax
            })

        return data

#class for h36m
import scipy.io as sio
import scipy as sp

class Human36M:
    def __init__(self, data_split, spl_ratio = 64, subject_list = [1, 5, 6, 7, 8], action_idx = (2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16), subaction_idx = (1, 2), camera_idx = (1, 2, 3, 4)):
        self.data_split = data_split
        self.data_dir = "D:/h36m/H36MDemo/SET_YOUR_OUTPUT_DIRECTORY/" #
        
        self.spl_ratio = spl_ratio
        self.subsampling = self.get_subsampling_ratio(data_split)
        self.joint_num = 18
        self.joints_name = (
            'Pelvis',     #0
            'R_Hip',      #1  
            'R_Knee',     #2
            'R_Ankle',    #3 
            'L_Hip',      #4 
            'L_Knee',     #5
            'L_Ankle',    #6
            'Torso',      #7
            'Neck',       #8
            'Nose',       #9
            'Head',       #10 
            'L_Shoulder', #11
            'L_Elbow',    #12
            'L_Wrist',    #13
            'R_Shoulder', #14 
            'R_Elbow',    #15
            'R_Wrist',    #16
            'Thorax')     #17


        self.flip_pairs = ( (1, 4), (2, 5), (3, 6), (14, 11), (15, 12), (16, 13) )
        self.skeleton = ( (0, 7), (7, 8), (8, 9), (9, 10), (8, 11), (11, 12), (12, 13), (8, 14), (14, 15), (15, 16), (0, 1), (1, 2), (2, 3), (0, 4), (4, 5), (5, 6) )
        self.lr_skeleton = ( ((8,11),(8,14)), ((11,12),(14,15)), ((12,13),(15,16)), ((0,1),(0,4)), ((1,2),(4,5)), ((2,3),(5,6)) )
        self.eval_joint = (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)
        self.joints_have_depth = True
        self.subject_list = subject_list
        self.action_idx = action_idx
        self.subaction_idx = subaction_idx
        self.camera_idx = camera_idx
        self.action_name = ['Directions', 'Discussion', 'Eating', 'Greeting', 'Phoning', 'Posing', 'Purchases', 'Sitting', 'SittingDown', 'Smoking', 'Photo', 'Waiting', 'Walking', 'WalkDog', 'WalkTogether']
       
        self.root_idx = self.joints_name.index('Pelvis')
        self.lshoulder_idx = self.joints_name.index('L_Shoulder')
        self.rshoulder_idx = self.joints_name.index('R_Shoulder')
    
    def get_subsampling_ratio(self, data_split):

        if data_split == 'train':
            return self.spl_ratio#5
        elif data_split == 'test':
            return 64
        else:
            assert 0, print('Unknown subset')

    def load_h36m_annot_file(self, annot_file):
        
        data = sio.loadmat(annot_file)
        joint_world = data['pose3d_world'] # 3D world coordinates of keypoints
        R = data['R'] # extrinsic
        T = np.reshape(data['T'],(3)) # extrinsic
        f = np.reshape(data['f'],(-1)) # focal legnth
        c = np.reshape(data['c'],(-1)) # principal points
        img_heights = np.reshape(data['img_height'],(-1))
        img_widths = np.reshape(data['img_width'],(-1))
       
        # add thorax
        thorax = (joint_world[:, self.lshoulder_idx, :] + joint_world[:, self.rshoulder_idx, :]) * 0.5
        thorax = thorax.reshape((thorax.shape[0], 1, thorax.shape[1]))
        joint_world = np.concatenate((joint_world, thorax), axis=1)

        return joint_world, R, T, f, c, img_widths, img_heights

    def _H36FolderName(self, subject_id, act_id, subact_id, camera_id):
        return "s_%02d_act_%02d_subact_%02d_ca_%02d" % \
               (subject_id, act_id, subact_id, camera_id)

    def _H36ImageName(self, folder_name, frame_id):
        return "%s_%06d.jpg" % (folder_name, frame_id + 1)

    def _AllHuman36Folders(self, subject_list):
        folders = []
        for i in subject_list:
            for j in self.action_idx:
                for m in self.subaction_idx:
                    for n in self.camera_idx:
                        folders.append(self._H36FolderName(i, j, m, n))
        return folders

    def _sample_dataset(self, data_split):
        if data_split == 'train':
            folders = self._AllHuman36Folders(self.subject_list)
        elif data_split == 'test':
            folders = self._AllHuman36Folders([9, 11])
        else:
            print("Unknown subset")
            assert 0

        return folders


    def load_data(self):

        folders = self._sample_dataset(self.data_split)
        data = []
        for folder in folders:
            
            if folder == 's_11_act_02_subact_02_ca_01':
                continue

            folder_dir = osp.join(self.data_dir, folder)
            
            # load ground truth
            joint_world, R, T, f, c, img_widths, img_heights = self.load_h36m_annot_file(osp.join(folder_dir, 'h36m_meta.mat'))
            img_num = np.shape(joint_world)[0]

            for n in range(0, img_num, self.subsampling):
                
                img_path = osp.join(folder_dir, self._H36ImageName(folder, n))
                joint_img, joint_cam, joint_vis, center_cam, bbox = process_world_coordinate(joint_world[n], self.root_idx, self.joint_num, R, T, f, c)
                img_width = img_widths[n]
                img_height = img_heights[n]
                joint_bone = np.zeros((BoneBasedBoneNum * 3), dtype=np.float)
                neg_thorax = np.zeros((3), dtype=np.float)
            
                data.append({
                    'img_path': img_path,
                    'bbox': bbox, 
                    'joint_img': joint_img, # [org_img_x, org_img_y, depth - root_depth]
                    'joint_cam': joint_cam, # [X, Y, Z] in camera coordinate
                    'joint_vis': joint_vis,
                    'center_cam': center_cam, # [X, Y, Z] in camera coordinate
                    'f': f,
                    'c': c,
                    'data_source': np.array([0], dtype=np.float32),
                    'joint_bone':  joint_bone, #dummy value
                    'neg_thorax':  neg_thorax #gt neg thorax
                })

        return data

    def evaluate(self, preds, result_dir):

        print() 
        print('Evaluation start...')

        gts = self.load_data()

        assert len(gts) == len(preds)

        sample_num = len(gts)
        joint_num = self.joint_num
        
        p1_error = np.zeros((sample_num, joint_num, 3)) # PA MPJPE (protocol #1 metric)
        p2_error = np.zeros((sample_num, joint_num, 3)) # MPJPE (protocol #2 metroc)
        p1_error_action = [ [] for _ in range(len(self.action_idx)) ] # PA MPJPE for each action
        p2_error_action = [ [] for _ in range(len(self.action_idx)) ] # MPJPE error for each action
        pred_to_save = []
        for n in range(sample_num):
            
            gt = gts[n]
            f = gt['f']
            c = gt['c']
            bbox = gt['bbox']
            gt_3d_center = gt['center_cam']
            gt_3d_kpt = gt['joint_cam']
            gt_vis = gt['joint_vis'].copy()

            # restore coordinates to original space
            pre_2d_kpt = preds[n].copy()
            pre_2d_kpt[:,0], pre_2d_kpt[:,1], pre_2d_kpt[:,2] = warp_coord_to_original(pre_2d_kpt, bbox, gt_3d_center)

            vis = False
            if vis:
                cvimg = cv2.imread(gt['img_path'], cv2.IMREAD_COLOR | cv2.IMREAD_IGNORE_ORIENTATION)
                filename = str(random.randrange(1,500))
                tmpimg = cvimg.copy().astype(np.uint8)
                tmpkps = np.zeros((3,joint_num))
                tmpkps[0,:], tmpkps[1,:] = pre_2d_kpt[:,0], pre_2d_kpt[:,1]
                tmpkps[2,:] = 1
                tmpimg = vis_keypoints(tmpimg, tmpkps, self.skeleton)
                cv2.imwrite(osp.join(vis_dir, filename + '_output.jpg'), tmpimg)

            # back project to camera coordinate system
            pre_3d_kpt = np.zeros((joint_num,3))
            pre_3d_kpt[:,0], pre_3d_kpt[:,1], pre_3d_kpt[:,2] = pixel2cam(pre_2d_kpt, f, c)

            vis = False
            if vis:
                vis_3d_skeleton(pre_3d_kpt, gt_vis, self.skeleton, filename)

            # root joint alignment
            pre_3d_kpt = pre_3d_kpt - pre_3d_kpt[self.root_idx]
            gt_3d_kpt  = gt_3d_kpt - gt_3d_kpt[self.root_idx]

            # rigid alignment for PA MPJPE (protocol #1)
            pre_3d_kpt_align = rigid_align(pre_3d_kpt, gt_3d_kpt)
 
            # prediction save
            pred_to_save.append({'pred': pre_3d_kpt,
                                 'align_pred': pre_3d_kpt_align,
                                 'gt': gt_3d_kpt})
           
            # error save
            p1_error[n] = np.power(pre_3d_kpt_align - gt_3d_kpt,2) # PA MPJPE (protocol #1)
            p2_error[n] = np.power(pre_3d_kpt - gt_3d_kpt,2)  # MPJPE (protocol #2)

            img_name = gt['img_path']
            action_idx = int(img_name[img_name.find('act')+4:img_name.find('act')+6]) - 2
            p1_error_action[action_idx].append(p1_error[n].copy())
            p2_error_action[action_idx].append(p2_error[n].copy())


        # total error calculate
        p1_error = np.take(p1_error, self.eval_joint, axis=1)
        p2_error = np.take(p2_error, self.eval_joint, axis=1)
        p1_error = np.mean(np.power(np.sum(p1_error,axis=2),0.5))
        p2_error = np.mean(np.power(np.sum(p2_error,axis=2),0.5))

        p1_eval_summary = 'Protocol #1 error (PA MPJPE) >> %.2f' % (p1_error)
        p2_eval_summary = 'Protocol #2 error (MPJPE) >> %.2f' % (p2_error)
        print()
        print(p1_eval_summary)
        print(p2_eval_summary)

        # error for each action calculate
        p1_action_eval_summary = 'Protocol #1 error (PA MPJPE) for each action: \n'
        for i in range(len(p1_error_action)):
            err = np.array(p1_error_action[i])
            err = np.take(err, self.eval_joint, axis=1)
            err = np.mean(np.power(np.sum(err,axis=2),0.5))

            action_name = self.action_name[i]
            p1_action_eval_summary += (action_name + ': %.2f\n' % err)

            
        p2_action_eval_summary = 'Protocol #2 error (MPJPE) for each action: \n'
        for i in range(len(p2_error_action)):
            err = np.array(p2_error_action[i])
            err = np.take(err, self.eval_joint, axis=1)
            err = np.mean(np.power(np.sum(err,axis=2),0.5))

            action_name = self.action_name[i]
            p2_action_eval_summary += (action_name + ': %.2f\n' % err)
        print()
        print(p1_action_eval_summary)
        print(p2_action_eval_summary)
       
        # result save
        f_pred_3d_kpt = open(osp.join(result_dir, 'pred_3d_kpt.txt'), 'w')
        f_pred_3d_kpt_align = open(osp.join(result_dir, 'pred_3d_kpt_align.txt'), 'w')
        f_gt_3d_kpt = open(osp.join(result_dir, 'gt_3d_kpt.txt'), 'w')
        for i in range(len(pred_to_save)):
            for j in range(joint_num):
                for k in range(3):
                    f_pred_3d_kpt.write('%.3f ' % pred_to_save[i]['pred'][j][k])
                    f_pred_3d_kpt_align.write('%.3f ' % pred_to_save[i]['align_pred'][j][k])
                    f_gt_3d_kpt.write('%.3f ' % pred_to_save[i]['gt'][j][k])
            f_pred_3d_kpt.write('\n')
            f_pred_3d_kpt_align.write('\n')
            f_gt_3d_kpt.write('\n')
        f_pred_3d_kpt.close()
        f_pred_3d_kpt_align.close()
        f_gt_3d_kpt.close()

        f_eval_result = open(osp.join(result_dir, 'eval_result.txt'), 'w')
        f_eval_result.write(p1_eval_summary)
        f_eval_result.write('\n')
        f_eval_result.write(p2_eval_summary)
        f_eval_result.write('\n')
        f_eval_result.write(p1_action_eval_summary)
        f_eval_result.write('\n')
        f_eval_result.write(p2_action_eval_summary)
        f_eval_result.write('\n')
        f_eval_result.close()


# class for jointdataset

#gt_mapping_cylinder = np.zeros((depth_dim, output_shape, output_shape),dtype=np.float32)
       
class JointDataset(Dataset):
    def __init__(self, db, is_train):
        
        #Config (multiple datasets databases)
        if isinstance(db, list):
            self.multiple_db = True
            self.db = [d.load_data() for d in db]
            self.joints_name = [d.joints_name for d in db]
            self.joint_num = [d.joint_num for d in db]
            self.skeleton = [d.skeleton for d in db]
            self.lr_skeleton = [d.lr_skeleton for d in db]
            self.flip_pairs = [d.flip_pairs for d in db]
            self.joints_have_depth = [d.joints_have_depth for d in db]
        else:
            self.multiple_db = False
            self.db = db.load_data()
            self.joint_num = db.joint_num
            self.skeleton = db.skeleton
            self.lr_skeleton = db.lr_skeleton
            self.flip_pairs = db.flip_pairs
            self.joints_have_depth = db.joints_have_depth
        #print(self.skeleton)        
        #compute JointDatasetLength
        if self.multiple_db:
            self.dataset_len = max([len(db) for db in self.db]) * len(self.db)
        else:
            self.dataset_len = len(self.db)

        self.is_train = is_train
        self.style_path_prefix = "allstyles/"
        self.style_path_suffix = ".png"
        self.transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=pixel_mean, std=pixel_std)])
        if self.is_train:
            self.do_aug = True
        else:
            self.do_aug = False
        self.do_aug = False
        
        self.id_styles = []
        self.lbd_styles = 0
        self.ubd_styles = 228
        for i in range(self.dataset_len):
            self.id_styles.append(random.randint(self.lbd_styles, self.ubd_styles))
        
        self.ske_colors = (( 85, 152, 188 ),                 
                           ( 199, 153, 96 ),                
                           ( 191, 141, 113 ),               
                           ( 119, 165, 232 ),               
                           ( 140, 139, 66 ),                 
                           ( 84, 83, 232 ),                  
                           ( 234, 218, 188 ),                
                           ( 178, 179, 252 ),               
                           ( 234, 235, 84 ),                 
                           ( 84, 100, 130 ),                
                           ( 230, 30, 210 ),                
                           ( 60, 200, 155),                  
                           ( 180, 20, 60 ),                 
                           ( 128, 128, 128 ),   
                           ( 238,190, 77),
                           ( 40, 169, 35),
                           ( 77, 55, 176))
    
    def __len__(self):
        return self.dataset_len


    def __getitem__(self, idx):
        # 0. First get the data from the database (merging 2 datasets)
        if self.multiple_db:
            db_idx = idx // max([len(db) for db in self.db])

            joint_num = self.joint_num[db_idx]
            skeleton = self.skeleton[db_idx]
            lr_skeleton = self.lr_skeleton[0]
            flip_pairs = self.flip_pairs[db_idx]
            joints_have_depth = self.joints_have_depth[db_idx]

            ref_joints_name = self.joints_name[0]
            joints_name = self.joints_name[db_idx]
            
            item_idx = idx % max([len(db) for db in self.db]) % len(self.db[db_idx])
            data = copy.deepcopy(self.db[db_idx][item_idx])
            
        else:
            joint_num = self.joint_num
            skeleton = self.skeleton
            lr_skeleton = self.lr_skeleton
            flip_pairs = self.flip_pairs
            joints_have_depth = self.joints_have_depth

            data = copy.deepcopy(self.db[idx])


        #1. load image
        # Get style image
        style_id = self.id_styles[idx]
        style_path_prefix = self.style_path_prefix 
        style_path = style_path_prefix + str(style_id) + self.style_path_suffix
        style_img = cv2.imread(style_path, cv2.IMREAD_COLOR | cv2.IMREAD_IGNORE_ORIENTATION)
        if not isinstance(style_img, np.ndarray):
            raise IOError("Fail to read %s" % style_path)
        
        #style_img = cv2.resize(style_img, (img_size, img_size))
        style_height, style_width, style_channels = style_img.shape

        style_img = style_img / 255.0
        # Get content image 
        content_img = cv2.imread(data['img_path'], cv2.IMREAD_COLOR | cv2.IMREAD_IGNORE_ORIENTATION)
        if not isinstance(content_img, np.ndarray):
            raise IOError("Fail to read %s" % data['img_path'])
        img_height, img_width, img_channels = content_img.shape
        content_img = content_img / 255.0
        #cv2.imshow("s",content_img)
        #cv2.waitKey(1)
        #2. get aug params
        if self.do_aug:
            scale, rot, do_flip, color_scale = get_aug_config()
        else:
            scale, rot, do_flip, color_scale = 1.0, 0, False, [1.0, 1.0, 1.0]

        # Read bbx (especially for mpii) to crop the patches
        bbox = data['bbox']
        joint_img = data['joint_img']
        data_source = data['data_source']
        #print('ori: ', joint_img)
        #print('done-----------')
        joint_vis = data['joint_vis']
        #joint_depth = data['joint_depth']
        joint_bone = data['joint_bone'] # no need to transform to another dataset system
        neg_thorax = data['neg_thorax']
        style_patch, trans = generate_patch_image(style_img, [0, 0, style_width, style_height], do_flip, scale, rot)
        for j in range(img_channels):
            style_patch[:, :, j] = np.clip(style_patch[:, :, j] * color_scale[j], 0, 255)
        # Remember to update trans
        # 3. crop patch from img and perform data augmentation (flip, scale, rot, color scale)
        content_patch, trans = generate_patch_image(content_img, bbox, do_flip, scale, rot)
        for i in range(img_channels):
            content_patch[:, :, i] = np.clip(content_patch[:, :, i] * color_scale[i], 0, 255)


        # 4. generate patch joint ground truth
        # flip joints and apply Affine Transform on joints
        if do_flip:
            joint_img[:, 0] = img_width - joint_img[:, 0] - 1
            for pair in self.flip_pairs:
                joint_img[pair[0], :], joint_img[pair[1], :] = joint_img[pair[1], :], joint_img[pair[0], :].copy()
                joint_vis[pair[0], :], joint_vis[pair[1], :] = joint_vis[pair[1], :], joint_vis[pair[0], :].copy()

        for i in range(len(joint_img)):
            joint_img[i, 0:2] = trans_point2d(joint_img[i, 0:2], trans)
            joint_img[i, 2] /= (bbox_3d_shape[0] / 2. * scale) # expect depth lies in -bbox_3d_shape[0]/2 ~ bbox_3d_shape[0]/2 -> -1.0 ~ 1.0
            joint_img[i, 2] = (joint_img[i, 2] + 1.0) / 2. # 0~1 normalize
            
            joint_vis[i] *= (
                            (joint_img[i,0] >= 0) & \
                            (joint_img[i,0] < img_size) & \
                            (joint_img[i,1] >= 0) & \
                            (joint_img[i,1] < img_size) & \
                            (joint_img[i,2] >= 0) & \
                            (joint_img[i,2] < 1)
                            )
        
        vis = False
        if vis:
            filename = str(random.randrange(1,500))
            tmpimg = img_patch.copy().astype(np.uint8)
            tmpkps = np.zeros((3,joint_num))
            tmpkps[:2,:] = joint_img[:,:2].transpose(1,0)
            tmpkps[2,:] = joint_vis[:,0]
            tmpimg = vis_keypoints(tmpimg, tmpkps, skeleton)
            cv2.imwrite(osp.join(vis_dir, filename + '_gt.jpg'), tmpimg)
        
        vis = False
        if vis:
            vis_3d_skeleton(joint_img, joint_vis, skeleton, filename)

        # 5. change coordinates to output space
        joint_img[:, 0] = joint_img[:, 0] / img_size * output_shape
        joint_img[:, 1] = joint_img[:, 1] / img_size * output_shape
        joint_img[:, 2] = joint_img[:, 2] * depth_dim
        
        # 6. change joint coord, vis to reference dataset. 0-th db is the reference dataset
        if self.multiple_db:
            joint_img = transform_joint_to_other_db(joint_img, joints_name, ref_joints_name)        
            joint_vis = transform_joint_to_other_db(joint_vis, joints_name, ref_joints_name)        
            #joint_depth = transform_joint_to_other_db(joint_depth, joints_name, ref_joints_name)        
       
        gt_3dske = np.zeros((depth_dim * 3, output_shape, output_shape),dtype=np.float32)
        is_vis_3d_ske = False
        gt_3d_center = data['center_cam']
        gt_2d_kpt = joint_img.copy() 
        gt_2d_kpt[:,0], gt_2d_kpt[:,1], gt_2d_kpt[:,2] = warp_coord_to_original(gt_2d_kpt, bbox, gt_3d_center)
 
        if data_source == HUMAN36M:
            f = data['f']
            c = data['c']
            is_vis_3d_ske = True
            
        elif data_source == MYMPII:
            f = [300.0, -300.0]
            c = [0.5, 0.5]
            is_vis_3d_ske = False
        else:
            f = [300.0, -300.0]
            c = [0.5, 0.5]
            
        gt_3d_kpt = np.zeros((joint_img.shape[0],3))
        gt_3d_kpt[:,0], gt_3d_kpt[:,1], gt_3d_kpt[:,2] = pixel2cam(gt_2d_kpt, f, c)
        is_vis_3d_ske = False
        if is_vis_3d_ske:
            # 7. Generate ground truth 3d ske
            map_size_ = output_shape
            depth_dims_ = depth_dim
            root_x = gt_3d_kpt[0,0]
            root_y = gt_3d_kpt[0,1]
            root_z = gt_3d_kpt[0,2]
            
            if data_source == HUMAN36M:
                x_lb_ = -921.5909885
                x_ub_ = 921.5909885
                y_lb_ = -921.5909885
                y_ub_ = 921.5909885
                z_lb_ = -950.637313 
                z_ub_ = 892.544664
                endpoint_dist_threshold_ = 60.0
                line_width_ = 30.0
            ske3d_toshow = []
            for l in range(len(skeleton)):
                i1 = skeleton[l][0]
                i2 = skeleton[l][1]
                if joint_vis[i1] == 0 or joint_vis[i2] == 0:
                    continue
                
                x1 = gt_3d_kpt[i1, 0] - root_x
                x2 = gt_3d_kpt[i2, 0] - root_x
                y1 = gt_3d_kpt[i1, 1] - root_y
                y2 = gt_3d_kpt[i2, 1] - root_y
                z1 = gt_3d_kpt[i1, 2] - root_z
                z2 = gt_3d_kpt[i2, 2] - root_z
                cnt_cur_bone = 0
                for k in range(0, depth_dims_):
                    for row in range(0, map_size_):
                        for col in range(0, map_size_):
                            
                            x0 = float(col) / float(map_size_) * (x_ub_ - x_lb_) + x_lb_
                            y0 = float(row) / float(map_size_) * (y_ub_ - y_lb_) + y_lb_
                            z0 = float(k) / float(depth_dims_) * (z_ub_ - z_lb_) + z_lb_
                            fz = (x1 - x0) * (x2 - x1) + (y1 - y0) * (y2 - y1) + (z1 - z0) * (z2 - z1)
                            fm = math.pow(x2 - x1, 2) + math.pow(y2 - y1, 2) + math.pow(z2 - z1, 2) + 1e-8
                            t = -fz / fm
                            d = math.sqrt(math.pow((x1 - x0) + (x2 - x1) * t, 2) + math.pow((y1 - y0) + (y2 - y1) * t, 2) + math.pow((z1 - z0) + (z2 - z1) * t, 2))

                            #//========distance from point X0 to line X1->X2 X0 = [x0, y0, z0]^T, X1 = [x1, y1, z1]^T, X2 = [x2, y2, z2]^T
                            if (x0 - x1 > 1e-6) or (math.fabs(x0 - x1) < 1e-6):
                                flag_x_a = True
                            else:
                                flag_x_a = False
                            if (x2 - x0 > 1e-6) or (math.fabs(x2 - x0) < 1e-6):
                                flag_x_b = True
                            else:
                                flag_x_b = False
                            equal_x = False
                            if (math.fabs(x0 - x1) < endpoint_dist_threshold_):
                                equal_x = True
                            if (math.fabs(x0 - x2) < endpoint_dist_threshold_):
                                equal_x = True

                            if (y0 - y1 > 1e-6) or (math.fabs(y0 - y1) < 1e-6):
                                flag_y_a = True
                            else:
                                flag_y_a = False
                            if (y2 - y0 > 1e-6) or (math.fabs(y2 - y0) < 1e-6):
                                flag_y_b = True
                            else:
                                flag_y_b = False
                            equal_y = False
                            if (math.fabs(y0 - y1) < endpoint_dist_threshold_):
                                equal_y = True
                            if (math.fabs(y0 - y2) < endpoint_dist_threshold_):
                                equal_y = True


                            if (z0 - z1 > 1e-6) or (math.fabs(z0 - z1) < 1e-6):
                                flag_z_a = True
                            else:
                                flag_z_a = False
                            if (z2 - z0 > 1e-6) or (math.fabs(z2 - z0) < 1e-6):
                                flag_z_b = True
                            else:
                                flag_z_b = False
                            equal_z = False
                            if (math.fabs(z0 - z1) < endpoint_dist_threshold_):
                                equal_z = True
                            if (math.fabs(z0 - z2) < endpoint_dist_threshold_):
                                equal_z = True

                            if (((flag_x_a == flag_x_b) or equal_x) and ((flag_y_a == flag_y_b) or equal_y) and ((flag_z_a == flag_z_b) or equal_z)):
                                if (line_width_ - d > 1e-6):
                                    cnt_cur_bone += 1
                                    ske3d_toshow.append([k, row, col])
                                    
                                    for c in range(3):
                                        gt_3dske[c * depth_dims_ + k][row][col] = self.ske_colors[l][c] / 255.0      
            
           
        
        # 7. Generate ground truth skeleton
        gt_ske = np.zeros((output_shape, output_shape, 3),dtype=np.uint8)
        
        for j in range(len(self.skeleton)):
            if self.multiple_db:
                x = self.skeleton[0][j][0]
                y = self.skeleton[0][j][1]
            else:
                x = self.skeleton[j][0]
                y = self.skeleton[j][1]
            if joint_vis[x] == 1 and joint_vis[y] == 1:
                cv2.line(
                gt_ske, (joint_img[x][0].astype(np.int32), joint_img[x][1].astype(np.int32)), (joint_img[y][0].astype(np.int32), joint_img[y][1].astype(np.int32)),
                color=self.ske_colors[j], thickness=3, lineType=cv2.LINE_AA)
                gt_ske_ = cv2.resize(gt_ske, (512, 512))
                
        gt_ske = gt_ske.transpose((2, 0, 1))
        gt_ske_out = np.zeros((3, output_shape, output_shape),dtype=np.float32)
        gt_ske_out[:, :, :] = gt_ske[:, :, :] / 255.0
        # 8. Image transformation preprocessing
        content_patch = self.transforms(content_patch)
        style_patch = self.transforms(style_patch)
        
        if self.is_train:
            joint_img = joint_img.astype(np.float32)
            joint_vis = (joint_vis > 0).astype(np.float32)
            joints_have_depth = np.array([joints_have_depth]).astype(np.float32)
            return style_patch, content_patch, joint_img, joint_vis, joints_have_depth, gt_ske_out, data_source, gt_3dske, joint_bone, neg_thorax
        else:
            return style_patch, content_patch


class MyDataset(Dataset):

    def __init__(self, is_train):
        self.is_train = is_train
        self.content_path_prefix = "../datasets/mpii/"
        self.style_path_prefix = "../datasets/styles/"

        self.joints_name = ('R_Ankle', 'R_Knee', 'R_Hip', 'L_Hip', 'L_Knee', 'L_Ankle', 'Pelvis', 'Thorax', 'Neck', 'Head', 'R_Wrist', 'R_Elbow', 'R_Shoulder', 'L_Shoulder', 'L_Elbow', 'L_Wrist')
        self.flip_pairs = ( (0, 5), (1, 4), (2, 3), (10, 15), (11, 14), (12, 13) )
        self.skeleton = ( (0, 1), (1, 2), (2, 6), (7, 12), (12, 11), (11, 10), (5, 4), (4, 3), (3, 6), (7, 13), (13, 14), (14, 15), (6, 7), (7, 8), (8, 9) )
        self.ske_colors = (( 85, 152, 188 ),                 
                            ( 199, 153, 96 ),                
                            ( 191, 141, 113 ),               
                            ( 119, 165, 232 ),               
                            ( 140, 139, 66 ),                 
                            ( 84, 83, 232 ),                  
                            ( 234, 218, 188 ),                
                            ( 178, 179, 252 ),               
                            ( 234, 235, 84 ),                 
                            ( 84, 100, 130 ),                
                            ( 230, 30, 210 ),                
                            ( 60, 200, 155),                  
                            ( 180, 20, 60 ),                 
                            ( 128, 128, 128 ),   
                            ( 238,190,77))
    
        self.transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=pixel_mean, std=pixel_std)])
        if self.is_train:
            self.do_aug = True
        else:
            self.do_aug = False
        self.do_aug = False
        self.joint_loc_path = "mpii/gt_joint_2d/"
        self.y = []
        self.id_contents = []
        self.id_styles = []
        self.lbd_contents = 0
        self.ubd_contents = 28881
        self.lbd_styles = 0
        self.ubd_styles = 20
        if self.is_train == False:
            self.lbd_contents = 0
            self.ubd_contents = 10
        for i in range(batch_size):
            self.id_contents.append(random.randint(self.lbd_contents, self.ubd_contents))
            self.id_styles.append(random.randint(self.lbd_styles, self.ubd_styles))
        self.joint_num = 16
        for i in range(batch_size):
            file_name = self.joint_loc_path + str(self.id_contents[i]) + ".txt"
            with open(file_name, 'r') as f:
                jdx = 0
                jt = []
                for line in f:
                    num = line.split(' ')                    
                    cur_jt = []
                    for j in range(len(num)):
                        if num[j] != '\n' and len(num[j]) > 0:
                            t_num = float(num[j]) * output_shape
                            cur_jt.append(t_num)
                    if len(cur_jt) > 0:
                        jt.append(cur_jt)
                self.y.append(jt)
    def __len__(self):
        #(style content) pairs
        return batch_size

    def __getitem__(self, idx):
        #train: return 
        # style, content pairs + 2d gt

        style_imgs = []
        content_imgs = []
        # 1. load image
        i = idx
        content_id = self.id_contents[i]
        content_path_prefix = self.content_path_prefix
        style_id = self.id_styles[i]
        style_path_prefix = self.style_path_prefix 
        if self.is_train:
            content_path_prefix += "train_img"
        else:
            content_path_prefix += "test"
        content_path = content_path_prefix + "/" + str(content_id) + ".png"
        style_path = style_path_prefix + str(style_id) + ".png"
        content_img = cv2.imread(content_path, cv2.IMREAD_COLOR | cv2.IMREAD_IGNORE_ORIENTATION)
        style_img = cv2.imread(style_path, cv2.IMREAD_COLOR | cv2.IMREAD_IGNORE_ORIENTATION)
        if not isinstance(content_img, np.ndarray):
            raise IOError("Fail to read %s" % content_path)

        if not isinstance(style_img, np.ndarray):
            raise IOError("Fail to read %s" % style_path)
        style_img = cv2.resize(style_img, (img_size, img_size))
        img_height, img_width, img_channels = content_img.shape
        content_img = content_img / 255.0
        style_img = style_img / 255.0
        
        # 2. get augmentation params
        self.do_aug = False
        if self.do_aug:
            scale, rot, do_flip, color_scale = get_aug_config()
        else:
            scale, rot, do_flip, color_scale = 1.0, 0, False, [1.0, 1.0, 1.0]

        # 3. crop patch from img and perform data augmentation (flip, scale, rot, color scale)
        content_patch, trans = generate_patch_image(content_img, [0, 0, 224, 224], do_flip, scale, rot)
        for j in range(img_channels):
            content_patch[:, :, j] = np.clip(content_patch[:, :, j] * color_scale[j], 0, 255)
            
        style_patch, trans = generate_patch_image(style_img, [0, 0, 224, 224], do_flip, scale, rot)
        for j in range(img_channels):
            style_patch[:, :, j] = np.clip(style_patch[:, :, j] * color_scale[j], 0, 255)
        
        # 4. online read ground truth
        # 4. generate patch joint ground truth
        # flip joints and apply Affine Transform on joints
        joint_img = np.asarray(self.y[i])
        
        if do_flip:
            joint_img[:, 0] = img_size - joint_img[:, 0] - 1
            for pair in self.flip_pairs:
                joint_img[pair[0], :], joint_img[pair[1], :] = joint_img[pair[1], :], joint_img[pair[0], :].copy()
                joint_vis[pair[0], :], joint_vis[pair[1], :] = joint_vis[pair[1], :], joint_vis[pair[0], :].copy()

        joint_vis = np.zeros((joint_img.shape[0], 1))
        for j in range(joint_img.shape[0]):
            joint_img[j, 0:2] = trans_point2d(joint_img[j, 0:2], trans)
            joint_img[j, 2] /= (bbox_3d_shape/2. * scale) # expect depth lies in -bbox_3d_shape[0]/2 ~ bbox_3d_shape[0]/2 -> -1.0 ~ 1.0
            joint_img[j, 2] = (joint_img[j,2] + 1.0)/2. # 0~1 normalize
            
            joint_vis[j] = (
                            (joint_img[j,0] >= 0) & \
                            (joint_img[j,0] < img_size) & \
                            (joint_img[j,1] >= 0) & \
                            (joint_img[j,1] < img_size)& \
                            (joint_img[j,2] >= 0) & \
                            (joint_img[j,2] < 1)
                            )
        joint_img[:, 0] = joint_img[:, 0] / img_size * output_shape
        joint_img[:, 1] = joint_img[:, 1] / img_size * output_shape
        joint_img[:, 2] = joint_img[:, 2] * depth_dim
        
        gt_ske = np.zeros((output_shape, output_shape, 3),dtype=np.uint8)
        
        for j in range(len(self.skeleton)):
            x = self.skeleton[j][0]
            y = self.skeleton[j][1]
            if joint_vis[x] == 1 and joint_vis[y] == 1:
                cv2.line(
                gt_ske, (joint_img[x][0].astype(np.int32), joint_img[x][1].astype(np.int32)), (joint_img[y][0].astype(np.int32), joint_img[y][1].astype(np.int32)),
                color=self.ske_colors[j], thickness=3, lineType=cv2.LINE_AA)
                gt_ske_ = cv2.resize(gt_ske, (512, 512))
                
        gt_ske = gt_ske.transpose((2, 0, 1))
        gt_ske_out = np.zeros((3, output_shape, output_shape),dtype=np.float32)
        gt_ske_out[:, :, :] = gt_ske[:, :, :] / 255.0
        content_patch = self.transforms(content_patch)
        style_patch = self.transforms(style_patch)
            
        if self.is_train:
            joint_img = joint_img.astype(np.float32)
            joint_vis = (joint_vis > 0).astype(np.float32)
            
            return style_patch, content_patch, joint_img, joint_vis, gt_ske_out
        else:
            return style_patch, content_patch, gt_ske_out

# 2. Build dataset
train_list = []
train_list.append(Human36M("train", spl_ratio = 5, subject_list = [1, 5, 6, 7, 8]))
train_list.append(MPII("train"))
train_list.append(MyMPII("train"))
train_list.append(COCOVal2017Dataset("train"))
train_list.append(COCOTrain2017Dataset("train"))

train_dataset = JointDataset(train_list, True)

train_loader = torch.utils.data.DataLoader(
            train_dataset, batch_size=batch_size, shuffle=True)

if isinstance(train_list, list):
    global_joint_num = train_dataset.joint_num[0]
else:
    global_joint_num = train_dataset.joint_num
        
itr_per_epoch = math.floor(train_dataset.__len__() / batch_size)
print(itr_per_epoch)


#3. network module definition
import torch
import torch.nn as nn
from torchvision.models.resnet import BasicBlock, Bottleneck
from torchvision.models.resnet import model_urls


import torch
import torch.nn as nn
from torch.hub import load_state_dict_from_url

__all__ = ['ResNet', 'resnet18', 'resnet34', 'resnet50', 'resnet101',
           'resnet152', 'resnext50_32x4d', 'resnext101_32x8d',
           'wide_resnet50_2', 'wide_resnet101_2',
           
           'resnext101_32x8d', 'resnext101_32x16d', 'resnext101_32x32d', 'resnext101_32x48d']


model_urls_resnext = {
    'resnet18': 'https://download.pytorch.org/models/resnet18-5c106cde.pth',
    'resnet34': 'https://download.pytorch.org/models/resnet34-333f7ec4.pth',
    'resnet50': 'https://download.pytorch.org/models/resnet50-19c8e357.pth',
    'resnet101': 'https://download.pytorch.org/models/resnet101-5d3b4d8f.pth',
    'resnet152': 'https://download.pytorch.org/models/resnet152-b121ed2d.pth',
    'resnext50_32x4d': 'https://download.pytorch.org/models/resnext50_32x4d-7cdf4587.pth',
    'resnext101_32x8d': 'https://download.pytorch.org/models/resnext101_32x8d-8ba56ff5.pth',
    'wide_resnet50_2': 'https://download.pytorch.org/models/wide_resnet50_2-95faca4d.pth',
    'wide_resnet101_2': 'https://download.pytorch.org/models/wide_resnet101_2-32ee1156.pth',


    'resnext101_32x8d': 'https://download.pytorch.org/models/ig_resnext101_32x8-c38310e5.pth',
    'resnext101_32x16d': 'https://download.pytorch.org/models/ig_resnext101_32x16-c6f796b0.pth',
    'resnext101_32x32d': 'https://download.pytorch.org/models/ig_resnext101_32x32-e4b90b00.pth',
    'resnext101_32x48d': 'https://download.pytorch.org/models/ig_resnext101_32x48-3e41cc8a.pth',
}


def conv3x3(in_planes, out_planes, stride=1, groups=1, dilation=1):
    """3x3 convolution with padding"""
    return nn.Conv2d(in_planes, out_planes, kernel_size=3, stride=stride,
                     padding=dilation, groups=groups, bias=False, dilation=dilation)


def conv1x1(in_planes, out_planes, stride=1):
    """1x1 convolution"""
    return nn.Conv2d(in_planes, out_planes, kernel_size=1, stride=stride, bias=False)


class BasicBlock(nn.Module):
    expansion = 1

    def __init__(self, inplanes, planes, stride=1, downsample=None, groups=1,
                 base_width=64, dilation=1, norm_layer=None):
        super(BasicBlock, self).__init__()
        if norm_layer is None:
            norm_layer = nn.BatchNorm2d
        if groups != 1 or base_width != 64:
            raise ValueError('BasicBlock only supports groups=1 and base_width=64')
        if dilation > 1:
            raise NotImplementedError("Dilation > 1 not supported in BasicBlock")
        # Both self.conv1 and self.downsample layers downsample the input when stride != 1
        self.conv1 = conv3x3(inplanes, planes, stride)
        self.bn1 = norm_layer(planes)
        self.relu = nn.ReLU(inplace=True)
        self.conv2 = conv3x3(planes, planes)
        self.bn2 = norm_layer(planes)
        self.downsample = downsample
        self.stride = stride

    def forward(self, x):
        identity = x

        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)

        out = self.conv2(out)
        out = self.bn2(out)

        if self.downsample is not None:
            identity = self.downsample(x)

        out += identity
        out = self.relu(out)

        return out


class Bottleneck(nn.Module):
    # Bottleneck in torchvision places the stride for downsampling at 3x3 convolution(self.conv2)
    # while original implementation places the stride at the first 1x1 convolution(self.conv1)
    # according to "Deep residual learning for image recognition"https://arxiv.org/abs/1512.03385.
    # This variant is also known as ResNet V1.5 and improves accuracy according to
    # https://ngc.nvidia.com/catalog/model-scripts/nvidia:resnet_50_v1_5_for_pytorch.

    expansion = 4

    def __init__(self, inplanes, planes, stride=1, downsample=None, groups=1,
                 base_width=64, dilation=1, norm_layer=None):
        super(Bottleneck, self).__init__()
        if norm_layer is None:
            norm_layer = nn.BatchNorm2d
        width = int(planes * (base_width / 64.)) * groups
        # Both self.conv2 and self.downsample layers downsample the input when stride != 1
        self.conv1 = conv1x1(inplanes, width)
        self.bn1 = norm_layer(width)
        self.conv2 = conv3x3(width, width, stride, groups, dilation)
        self.bn2 = norm_layer(width)
        self.conv3 = conv1x1(width, planes * self.expansion)
        self.bn3 = norm_layer(planes * self.expansion)
        self.relu = nn.ReLU(inplace=True)
        self.downsample = downsample
        self.stride = stride

    def forward(self, x):
        identity = x

        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)

        out = self.conv2(out)
        out = self.bn2(out)
        out = self.relu(out)

        out = self.conv3(out)
        out = self.bn3(out)

        if self.downsample is not None:
            identity = self.downsample(x)

        out += identity
        out = self.relu(out)

        return out


class ResNet(nn.Module):

    def __init__(self, block, layers, num_classes=1000, zero_init_residual=False,
                 groups=1, width_per_group=64, replace_stride_with_dilation=None,
                 norm_layer=None):
        super(ResNet, self).__init__()
        if norm_layer is None:
            norm_layer = nn.BatchNorm2d
        self._norm_layer = norm_layer

        self.inplanes = 64
        self.dilation = 1
        if replace_stride_with_dilation is None:
            # each element in the tuple indicates if we should replace
            # the 2x2 stride with a dilated convolution instead
            replace_stride_with_dilation = [False, False, False]
        if len(replace_stride_with_dilation) != 3:
            raise ValueError("replace_stride_with_dilation should be None "
                             "or a 3-element tuple, got {}".format(replace_stride_with_dilation))
        self.groups = groups
        self.base_width = width_per_group
        self.conv1 = nn.Conv2d(3, self.inplanes, kernel_size=7, stride=2, padding=3,
                               bias=False)
        self.bn1 = norm_layer(self.inplanes)
        self.relu = nn.ReLU(inplace=True)
        self.maxpool = nn.MaxPool2d(kernel_size=3, stride=2, padding=1)
        self.maxpool2 = nn.MaxPool2d(kernel_size=2, stride=2, padding=0)
        
        self.layer1 = self._make_layer(block, 64, layers[0])
        self.layer2 = self._make_layer(block, 128, layers[1], stride=2,
                                       dilate=replace_stride_with_dilation[0])
        self.layer3 = self._make_layer(block, 256, layers[2], stride=2,
                                       dilate=replace_stride_with_dilation[1])
        self.layer4 = self._make_layer(block, 512, layers[3], stride=2,
                                       dilate=replace_stride_with_dilation[2])
        self.avgpool = nn.AdaptiveAvgPool2d((1, 1))
        self.fc = nn.Linear(512 * block.expansion, num_classes)


        self.deconv_layers = self._make_deconv_layer(3)

        #512x32x32->256x32x32
        self.layer2_conv1 = nn.Conv2d(512, 64, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer2_bn1 = nn.BatchNorm2d(64)
        #256x32x32->256x32x32
        self.layer2_conv2 = nn.Conv2d(64, 64, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer2_bn2 = nn.BatchNorm2d(64)
        #256x32x32->3x32x32
        self.layer2_conv3 = nn.Conv2d(64, 3, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer2_bn3 = nn.BatchNorm2d(3)
        
        self.layer3_deconv1 = nn.ConvTranspose2d(
                    in_channels=1024,
                    out_channels=64,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False)
        self.layer3_bn1 = nn.BatchNorm2d(64)

        self.layer3_conv1 = nn.Conv2d(64, 64, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer3_bn2 = nn.BatchNorm2d(64)

        self.layer3_conv2 = nn.Conv2d(64, 3, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer3_bn3 = nn.BatchNorm2d(3)

        self.layer4_deconv1 = nn.ConvTranspose2d(
                    in_channels=2048,
                    out_channels=32,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False)
        self.layer4_bn1 = nn.BatchNorm2d(32)
        self.layer4_deconv2 = nn.ConvTranspose2d(
                    in_channels=32,
                    out_channels=32,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False)
        self.layer4_bn2 = nn.BatchNorm2d(32)



        self.layer4_conv1 = nn.Conv2d(32, 32, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer4_bn3 = nn.BatchNorm2d(32)

        self.layer4_conv2 = nn.Conv2d(32, 3, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer4_bn4 = nn.BatchNorm2d(3)



        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
            elif isinstance(m, (nn.BatchNorm2d, nn.GroupNorm)):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)
            elif isinstance(m, nn.ConvTranspose2d):
                nn.init.normal_(m.weight, std=0.001)

        # Zero-initialize the last BN in each residual branch,
        # so that the residual branch starts with zeros, and each residual block behaves like an identity.
        # This improves the model by 0.2~0.3% according to https://arxiv.org/abs/1706.02677
        if zero_init_residual:
            for m in self.modules():
                if isinstance(m, Bottleneck):
                    nn.init.constant_(m.bn3.weight, 0)
                elif isinstance(m, BasicBlock):
                    nn.init.constant_(m.bn2.weight, 0)

    def _make_layer(self, block, planes, blocks, stride=1, dilate=False):
        norm_layer = self._norm_layer
        downsample = None
        previous_dilation = self.dilation
        if dilate:
            self.dilation *= stride
            stride = 1
        if stride != 1 or self.inplanes != planes * block.expansion:
            downsample = nn.Sequential(
                conv1x1(self.inplanes, planes * block.expansion, stride),
                norm_layer(planes * block.expansion),
            )

        layers = []
        layers.append(block(self.inplanes, planes, stride, downsample, self.groups,
                            self.base_width, previous_dilation, norm_layer))
        self.inplanes = planes * block.expansion
        for _ in range(1, blocks):
            layers.append(block(self.inplanes, planes, groups=self.groups,
                                base_width=self.base_width, dilation=self.dilation,
                                norm_layer=norm_layer))

        return nn.Sequential(*layers)

    def _make_deconv_layer(self, num_layers):
        layers = []
        for i in range(num_layers):
            
            if i == 0:
               layers.append(
                nn.ConvTranspose2d(
                    in_channels=3,
                    out_channels=64,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False))
            elif i < 2:
               layers.append(
                nn.ConvTranspose2d(
                    in_channels=64,
                    out_channels=64,
                    kernel_size=3,
                    stride=3,
                    padding=0,
                    output_padding=0,
                    bias=False))
            else:
                layers.append(
                nn.ConvTranspose2d(
                    in_channels=64,
                    out_channels=64,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False))
            
            layers.append(nn.BatchNorm2d(64))
            layers.append(nn.ReLU(inplace=True))
            #self.inplanes = self.outplanes

        return nn.Sequential(*layers)


    def _forward_impl(self, x):
        # See note [TorchScript super()]
        
        x = self.conv1(x)
        x = self.bn1(x)
        x = self.relu(x)
        x = self.maxpool(x)

        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.layer4(x)
        return x

    def forward(self, x):
        return self._forward_impl(x)

def load_my_state_dict(model, state_dict):
 
        own_state = model.state_dict()
        for name, param in state_dict.items():
            if name not in own_state:
                 continue
            #if isinstance(param, Parameter):
                # backwards compatibility for serialized parameters
            #    param = param.data
            own_state[name].copy_(param)

def _resnet(arch, block, layers, pretrained, progress, **kwargs):
    model = ResNet(block, layers, num_classes = 43, **kwargs)
    if pretrained:
        org_resnet = torch.utils.model_zoo.load_url(model_urls[arch])
        org_resnet.pop('fc.weight', None)
        org_resnet.pop('fc.bias', None)
        org_resnet.pop('conv1.weight', None)
        org_resnet.pop('conv1.bias', None)
        
        load_my_state_dict(model, org_resnet)
    return model


def _resnext(arch, block, layers, pretrained, progress, **kwargs):
    model = ResNet(block, layers, num_classes = 43, **kwargs)
    if pretrained:
        org_resnet = torch.utils.model_zoo.load_url(model_urls_resnext[arch])
        org_resnet.pop('fc.weight', None)
        org_resnet.pop('fc.bias', None)
        org_resnet.pop('conv1.weight', None)
        org_resnet.pop('conv1.bias', None)
        
        load_my_state_dict(model, org_resnet)
        
  
    return model
    
def resnet18(pretrained=False, progress=True, **kwargs):
    r"""ResNet-18 model from
    `"Deep Residual Learning for Image Recognition" <https://arxiv.org/pdf/1512.03385.pdf>`_
    Args:
        pretrained (bool): If True, returns a model pre-trained on ImageNet
        progress (bool): If True, displays a progress bar of the download to stderr
    """
    return _resnet('resnet18', BasicBlock, [2, 2, 2, 2], pretrained, progress,
                   **kwargs)


def resnet34(pretrained=False, progress=True, **kwargs):
    r"""ResNet-34 model from
    `"Deep Residual Learning for Image Recognition" <https://arxiv.org/pdf/1512.03385.pdf>`_
    Args:
        pretrained (bool): If True, returns a model pre-trained on ImageNet
        progress (bool): If True, displays a progress bar of the download to stderr
    """
    return _resnet('resnet34', BasicBlock, [3, 4, 6, 3], pretrained, progress,
                   **kwargs)


def resnet50(pretrained=False, progress=True, **kwargs):
    r"""ResNet-50 model from
    `"Deep Residual Learning for Image Recognition" <https://arxiv.org/pdf/1512.03385.pdf>`_
    Args:
        pretrained (bool): If True, returns a model pre-trained on ImageNet
        progress (bool): If True, displays a progress bar of the download to stderr
    """
    return _resnet('resnet50', Bottleneck, [3, 4, 6, 3], pretrained, progress,
                   **kwargs)


def resnet101(pretrained=False, progress=True, **kwargs):
    r"""ResNet-101 model from
    `"Deep Residual Learning for Image Recognition" <https://arxiv.org/pdf/1512.03385.pdf>`_
    Args:
        pretrained (bool): If True, returns a model pre-trained on ImageNet
        progress (bool): If True, displays a progress bar of the download to stderr
    """
    return _resnet('resnet101', Bottleneck, [3, 4, 23, 3], pretrained, progress,
                   **kwargs)


def resnet152(pretrained=False, progress=True, **kwargs):
    r"""ResNet-152 model from
    `"Deep Residual Learning for Image Recognition" <https://arxiv.org/pdf/1512.03385.pdf>`_
    Args:
        pretrained (bool): If True, returns a model pre-trained on ImageNet
        progress (bool): If True, displays a progress bar of the download to stderr
    """
    return _resnet('resnet152', Bottleneck, [3, 8, 36, 3], pretrained, progress,
                   **kwargs)


def resnext50_32x4d(pretrained=False, progress=True, **kwargs):
    r"""ResNeXt-50 32x4d model from
    `"Aggregated Residual Transformation for Deep Neural Networks" <https://arxiv.org/pdf/1611.05431.pdf>`_
    Args:
        pretrained (bool): If True, returns a model pre-trained on ImageNet
        progress (bool): If True, displays a progress bar of the download to stderr
    """
    kwargs['groups'] = 32
    kwargs['width_per_group'] = 4
    return _resnet('resnext50_32x4d', Bottleneck, [3, 4, 6, 3],
                   pretrained, progress, **kwargs)


def resnext101_32x8d(pretrained=False, progress=True, **kwargs):
    r"""ResNeXt-101 32x8d model from
    `"Aggregated Residual Transformation for Deep Neural Networks" <https://arxiv.org/pdf/1611.05431.pdf>`_
    Args:
        pretrained (bool): If True, returns a model pre-trained on ImageNet
        progress (bool): If True, displays a progress bar of the download to stderr
    """
    kwargs['groups'] = 32
    kwargs['width_per_group'] = 8
    return _resnet('resnext101_32x8d', Bottleneck, [3, 4, 23, 3],
                   pretrained, progress, **kwargs)


def resnext101_32x8d_wsl(progress=True, **kwargs):
    """Constructs a ResNeXt-101 32x8 model pre-trained on weakly-supervised data
    and finetuned on ImageNet from Figure 5 in
    `"Exploring the Limits of Weakly Supervised Pretraining" <https://arxiv.org/abs/1805.00932>`_
    Args:
        progress (bool): If True, displays a progress bar of the download to stderr.
    """
    kwargs['groups'] = 32
    kwargs['width_per_group'] = 8
    return _resnext('resnext101_32x8d', Bottleneck, [3, 4, 23, 3], True, progress, **kwargs)


def resnext101_32x16d_wsl(progress=True, **kwargs):
    """Constructs a ResNeXt-101 32x16 model pre-trained on weakly-supervised data
    and finetuned on ImageNet from Figure 5 in
    `"Exploring the Limits of Weakly Supervised Pretraining" <https://arxiv.org/abs/1805.00932>`_
    Args:
        progress (bool): If True, displays a progress bar of the download to stderr.
    """
    kwargs['groups'] = 32
    kwargs['width_per_group'] = 16
    return _resnext('resnext101_32x16d', Bottleneck, [3, 4, 23, 3], True, progress, **kwargs)


def resnext101_32x32d_wsl(progress=True, **kwargs):
    """Constructs a ResNeXt-101 32x32 model pre-trained on weakly-supervised data
    and finetuned on ImageNet from Figure 5 in
    `"Exploring the Limits of Weakly Supervised Pretraining" <https://arxiv.org/abs/1805.00932>`_
    Args:
        progress (bool): If True, displays a progress bar of the download to stderr.
    """
    kwargs['groups'] = 32
    kwargs['width_per_group'] = 32
    return _resnext('resnext101_32x32d', Bottleneck, [3, 4, 23, 3], True, progress, **kwargs)


def resnext101_32x48d_wsl(progress=True, **kwargs):
    """Constructs a ResNeXt-101 32x48 model pre-trained on weakly-supervised data
    and finetuned on ImageNet from Figure 5 in
    `"Exploring the Limits of Weakly Supervised Pretraining" <https://arxiv.org/abs/1805.00932>`_
    Args:
        progress (bool): If True, displays a progress bar of the download to stderr.
    """
    kwargs['groups'] = 32
    kwargs['width_per_group'] = 48
    return _resnext('resnext101_32x48d', Bottleneck, [3, 4, 23, 3], True, progress, **kwargs)

def wide_resnet50_2(pretrained=False, progress=True, **kwargs):
    r"""Wide ResNet-50-2 model from
    `"Wide Residual Networks" <https://arxiv.org/pdf/1605.07146.pdf>`_
    The model is the same as ResNet except for the bottleneck number of channels
    which is twice larger in every block. The number of channels in outer 1x1
    convolutions is the same, e.g. last block in ResNet-50 has 2048-512-2048
    channels, and in Wide ResNet-50-2 has 2048-1024-2048.
    Args:
        pretrained (bool): If True, returns a model pre-trained on ImageNet
        progress (bool): If True, displays a progress bar of the download to stderr
    """
    kwargs['width_per_group'] = 64 * 2
    return _resnet('wide_resnet50_2', Bottleneck, [3, 4, 6, 3],
                   pretrained, progress, **kwargs)


def wide_resnet101_2(pretrained=False, progress=True, **kwargs):
    r"""Wide ResNet-101-2 model from
    `"Wide Residual Networks" <https://arxiv.org/pdf/1605.07146.pdf>`_
    The model is the same as ResNet except for the bottleneck number of channels
    which is twice larger in every block. The number of channels in outer 1x1
    convolutions is the same, e.g. last block in ResNet-50 has 2048-512-2048
    channels, and in Wide ResNet-50-2 has 2048-1024-2048.
    Args:
        pretrained (bool): If True, returns a model pre-trained on ImageNet
        progress (bool): If True, displays a progress bar of the download to stderr
    """
    kwargs['width_per_group'] = 64 * 2
    return _resnet('wide_resnet101_2', Bottleneck, [3, 4, 23, 3],
                   pretrained, progress, **kwargs)


class ResNetBackbone(nn.Module):

    def __init__(self, resnet_type, is_pose_net=True, num_channel_first_layer = 3):
    
        resnet_spec = {18: (BasicBlock, [2, 2, 2, 2], [64, 64, 128, 256, 512], 'resnet18'),
               34: (BasicBlock, [3, 4, 6, 3], [64, 64, 128, 256, 512], 'resnet34'),
               50: (Bottleneck, [3, 4, 6, 3], [64, 256, 512, 1024, 2048], 'resnet50'),
               101: (Bottleneck, [3, 4, 23, 3], [64, 256, 512, 1024, 2048], 'resnet101'),
               152: (Bottleneck, [3, 8, 36, 3], [64, 256, 512, 1024, 2048], 'resnet152')}
        block, layers, channels, name = resnet_spec[resnet_type]
        
        self.name = name
        self.inplanes = 64
        self.outplanes = 3
        self.is_pose_net = is_pose_net
        super(ResNetBackbone, self).__init__()
        if self.is_pose_net:
            self.conv1 = nn.Conv2d(num_channel_first_layer, 64, kernel_size=7, stride=2, padding=3,
                               bias=False)
        else:
            self.conv1 = nn.Conv2d(num_channel_first_layer, 64, kernel_size=7, stride=2, padding=3,
                               bias=False)
        self.bn1 = nn.BatchNorm2d(64)
        self.relu = nn.ReLU(inplace=True)
        self.maxpool = nn.MaxPool2d(kernel_size=3, stride=2, padding=1)
        self.maxpool2 = nn.MaxPool2d(kernel_size=2, stride=2, padding=0)
        self.layer1 = self._make_layer(block, 64, layers[0])
        self.layer2 = self._make_layer(block, 128, layers[1], stride=2)
        self.layer3 = self._make_layer(block, 256, layers[2], stride=2)
        self.layer4 = self._make_layer(block, 512, layers[3], stride=2)
        self.deconv_layers = self._make_deconv_layer(2)

        #512x32x32->256x32x32
        if self.is_pose_net:
            self.layer2_deconv1 = nn.ConvTranspose2d(
                        in_channels=512,
                        out_channels=128,
                        kernel_size=4,
                        stride=2,
                        padding=1,
                        output_padding=0,
                        bias=False)
        else:
            self.layer2_deconv1 = nn.ConvTranspose2d(
                        in_channels=128,
                        out_channels=256,
                        kernel_size=4,
                        stride=2,
                        padding=1,
                        output_padding=0,
                        bias=False)
        if self.is_pose_net:
            self.layer2_conv1 = nn.Conv2d(512, 64, kernel_size=1, stride=1, padding=0,
                               bias=False)
        else:
            self.layer2_conv1 = nn.Conv2d(128, 64, kernel_size=1, stride=1, padding=0,
                               bias=False)
        if self.is_pose_net:
            self.layer2_bn1 = nn.BatchNorm2d(128)
        else:
            self.layer2_bn1 = nn.BatchNorm2d(256)
        #256x32x32->256x32x32
        if self.is_pose_net:
            self.layer2_conv2 = nn.Conv2d(128, 128, kernel_size=1, stride=1, padding=0,
                               bias=False)
        else:
            self.layer2_conv2 = nn.Conv2d(256, 256, kernel_size=3, stride=1, padding=1,
                               bias=False)
        if self.is_pose_net:
            self.layer2_bn2 = nn.BatchNorm2d(128)
        else:
            self.layer2_bn2 = nn.BatchNorm2d(256)
        #256x32x32->3x32x32
        if self.is_pose_net:
            self.layer2_conv3 = nn.Conv2d(128, 3, kernel_size=1, stride=1, padding=0,
                               bias=False)
        else:
            self.layer2_conv3 = nn.Conv2d(256, 256, kernel_size=3, stride=1, padding=1,
                               bias=False)
        if self.is_pose_net:
            self.layer2_bn3 = nn.BatchNorm2d(3)
        else:
            self.layer2_bn3 = nn.BatchNorm2d(256)
        self.layer3_deconv1 = nn.ConvTranspose2d(
                    in_channels=1024,
                    out_channels=64,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False)
        self.layer3_bn1 = nn.BatchNorm2d(64)

        self.layer3_conv1 = nn.Conv2d(64, 64, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer3_bn2 = nn.BatchNorm2d(64)

        self.layer3_conv2 = nn.Conv2d(64, 3, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer3_bn3 = nn.BatchNorm2d(3)

        self.layer4_deconv1 = nn.ConvTranspose2d(
                    in_channels=2048,
                    out_channels=64,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False)
        self.layer4_bn1 = nn.BatchNorm2d(64)

        self.layer4_conv1 = nn.Conv2d(64, 64, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer4_bn2 = nn.BatchNorm2d(64)

        self.layer4_conv2 = nn.Conv2d(64, 3, kernel_size=1, stride=1, padding=0,
                               bias=False)
        self.layer4_bn3 = nn.BatchNorm2d(3)


        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                # nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
                nn.init.normal_(m.weight, mean=0, std=0.01)
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

    def _make_layer(self, block, planes, blocks, stride=1):
        downsample = None
        if stride != 1 or self.inplanes != planes * block.expansion:
            downsample = nn.Sequential(
                nn.Conv2d(self.inplanes, planes * block.expansion,
                          kernel_size=1, stride=stride, bias=False),
                nn.BatchNorm2d(planes * block.expansion),
            )

        layers = []
        layers.append(block(self.inplanes, planes, stride, downsample))
        self.inplanes = planes * block.expansion
        for i in range(1, blocks):
            layers.append(block(self.inplanes, planes))

        return nn.Sequential(*layers)


    def _make_deconv_layer(self, num_layers):
        layers = []
        for i in range(num_layers):
            #print(i, "deconv in_channels", self.inplanes, " out_channels ", self.outplanes)
            #2048 x 256 BN ReLU 32 -> 64
            #256 x 256 BN ReLU 64 -> 128
            #256 x 256 BN ReLU 128 -> 256
            if i == 0:
               layers.append(
                nn.ConvTranspose2d(
                    in_channels=256,
                    out_channels=256,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False))
               layers.append(nn.BatchNorm2d(256))
               layers.append(nn.ReLU(inplace=True))
            
            elif i < 2:
               layers.append(
                nn.ConvTranspose2d(
                    in_channels=256,
                    out_channels=3,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False))
               layers.append(nn.Sigmoid())
            
               layers.append(nn.BatchNorm2d(3))
               #layers.append(nn.ReLU(inplace=True))
            
            else:
                layers.append(
                nn.ConvTranspose2d(
                    in_channels=64,
                    out_channels=3,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False))
            
                layers.append(nn.BatchNorm2d(3))
            
            self.inplanes = self.outplanes

        return nn.Sequential(*layers)


    def forward(self, x):
        
        x = self.conv1(x)

        x = self.bn1(x)
        x = self.relu(x)
        x = self.maxpool(x)

        x = self.layer1(x)
        x = self.layer2(x)
        
        if 1:#self.is_pose_net:
            sign_res3c = x

            sign_res3c_deconv = self.layer2_deconv1(x)
            sign_res3c_deconv = self.layer2_bn1(sign_res3c_deconv)
            sign_res3c_deconv = self.relu(sign_res3c_deconv)
            #256x32x32->256x32x32
            sign_res3c_deconv = self.layer2_conv2(sign_res3c_deconv)
            sign_res3c_deconv = self.layer2_bn2(sign_res3c_deconv)
            sign_res3c_deconv = self.relu(sign_res3c_deconv)
            #256x32x32->3x32x32
            sign_res3c_deconv = self.layer2_conv3(sign_res3c_deconv)
            sign_res3c_deconv = self.layer2_bn3(sign_res3c_deconv)
            #sign_res3c_deconv = self.relu(sign_res3c_deconv)


        x = self.layer3(x)
        if self.is_pose_net:
            sign_res4f = x
        x = self.layer4(x)
        if self.is_pose_net:
            sign_res5c = x
        if self.is_pose_net:
            return x, sign_res3c, sign_res4f, sign_res5c, sign_res3c_deconv
        if self.is_pose_net == False:
            x = self.deconv_layers(sign_res3c_deconv)
        return x
        
    def load_my_state_dict(model, state_dict):
 
        own_state = model.state_dict()
        for name, param in state_dict.items():
            if name not in own_state:
                 continue
            #if isinstance(param, Parameter):
                # backwards compatibility for serialized parameters
            #    param = param.data
            own_state[name].copy_(param)

    def init_weights(self):
        org_resnet = torch.utils.model_zoo.load_url(model_urls[self.name])
        # drop orginal resnet fc layer, add 'None' in case of no fc layer, that will raise error
        org_resnet.pop('fc.weight', None)
        org_resnet.pop('fc.bias', None)
        #if self.is_pose_net:
        org_resnet.pop('conv1.weight', None)
        org_resnet.pop('conv1.bias', None)
        
        #self.load_state_dict(org_resnet)
        self.load_my_state_dict(org_resnet)
        print("Initialize resnet from model zoo")
        for name, m in self.deconv_layers.named_modules():
            if isinstance(m, nn.ConvTranspose2d):
                nn.init.normal_(m.weight, std=0.01)

# Conv Layer
class ConvLayer(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size, stride):
        super(ConvLayer, self).__init__()
        padding = kernel_size // 2
        self.reflection_pad = nn.ReflectionPad2d(padding)
        self.conv2d = nn.Conv2d(in_channels, out_channels, kernel_size, stride) #, padding)

    def forward(self, x):
        out = self.reflection_pad(x)
        out = self.conv2d(out)
        return out

# Upsample Conv Layer
class UpsampleConvLayer(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size, stride, upsample=None):
        super(UpsampleConvLayer, self).__init__()
        self.upsample = upsample
        if upsample:
            self.upsample = nn.Upsample(scale_factor=upsample, mode='nearest')
        reflection_padding = kernel_size // 2
        self.reflection_pad = nn.ReflectionPad2d(reflection_padding)
        self.conv2d = nn.Conv2d(in_channels, out_channels, kernel_size, stride)

    def forward(self, x):
        if self.upsample:
            x = self.upsample(x)
        out = self.reflection_pad(x)
        out = self.conv2d(out)
        return out

# Residual Block
#   adapted from pytorch tutorial
#   https://github.com/yunjey/pytorch-tutorial/blob/master/tutorials/02-
#   intermediate/deep_residual_network/main.py
class ResidualBlock(nn.Module):
    def __init__(self, channels):
        super(ResidualBlock, self).__init__()
        self.conv1 = ConvLayer(channels, channels, kernel_size=3, stride=1)
        self.in1 = nn.InstanceNorm2d(channels, affine=True)
        self.relu = nn.ReLU()
        self.conv2 = ConvLayer(channels, channels, kernel_size=3, stride=1)
        self.in2 = nn.InstanceNorm2d(channels, affine=True)

    def forward(self, x):
        residual = x
        out = self.relu(self.in1(self.conv1(x)))
        out = self.in2(self.conv2(out))
        out = out + residual
        out = self.relu(out)
        return out 


# Image Transform Network
class ImageTransformNet(nn.Module):
    def __init__(self):
        super(ImageTransformNet, self).__init__()
        
        # nonlineraity
        self.relu = nn.ReLU()
        self.tanh = nn.Tanh()

        # encoding layers
        self.conv1 = ConvLayer(6, 32, kernel_size=9, stride=1)
        self.in1_e = nn.InstanceNorm2d(32, affine=True)

        self.conv2 = ConvLayer(32, 64, kernel_size=3, stride=2)
        self.in2_e = nn.InstanceNorm2d(64, affine=True)

        self.conv3 = ConvLayer(64, 128, kernel_size=3, stride=2)
        self.in3_e = nn.InstanceNorm2d(128, affine=True)

        # residual layers
        self.res1 = ResidualBlock(128)
        self.res2 = ResidualBlock(128)
        self.res3 = ResidualBlock(128)
        self.res4 = ResidualBlock(128)
        self.res5 = ResidualBlock(128)

        # decoding layers
        self.deconv3 = UpsampleConvLayer(128, 64, kernel_size=3, stride=1, upsample=2 )
        self.in3_d = nn.InstanceNorm2d(64, affine=True)

        self.deconv2 = UpsampleConvLayer(64, 32, kernel_size=3, stride=1, upsample=2 )
        self.in2_d = nn.InstanceNorm2d(32, affine=True)

        self.deconv1 = UpsampleConvLayer(32, 3, kernel_size=9, stride=1)
        self.in1_d = nn.InstanceNorm2d(3, affine=True)

    def forward(self, x):
        # encode
        y = self.relu(self.in1_e(self.conv1(x)))
        y = self.relu(self.in2_e(self.conv2(y)))
        y = self.relu(self.in3_e(self.conv3(y)))

        # residual layers
        y = self.res1(y)
        y = self.res2(y)
        y = self.res3(y)
        y = self.res4(y)
        y = self.res5(y)

        # decode
        y = self.relu(self.in3_d(self.deconv3(y)))
        y = self.relu(self.in2_d(self.deconv2(y)))
        #y = self.tanh(self.in1_d(self.deconv1(y)))
        y = self.deconv1(y)

        return y

    def init_weights(self):
        a = 1


# image transform net for self supervision from 2d ske and style

# Image Transform Network Self Sup
class ImageTransformSelfSupNet(nn.Module):
    def __init__(self):
        super(ImageTransformSelfSupNet, self).__init__()
        
        # nonlineraity
        self.relu = nn.ReLU()
        self.tanh = nn.Tanh()

        # encoding layers
        self.conv1 = ConvLayer(44, 32, kernel_size=9, stride=1)
        
        #self.conv1 = ConvLayer(22, 32, kernel_size=9, stride=1)
        #self.conv1 = ConvLayer(6, 32, kernel_size=9, stride=1)
        
        self.in1_e = nn.InstanceNorm2d(32, affine=True)

        self.conv2 = ConvLayer(32, 64, kernel_size=3, stride=2)
        self.in2_e = nn.InstanceNorm2d(64, affine=True)

        self.conv3 = ConvLayer(64, 128, kernel_size=3, stride=2)
        self.in3_e = nn.InstanceNorm2d(128, affine=True)

        # residual layers
        self.res1 = ResidualBlock(128)
        self.res2 = ResidualBlock(128)
        self.res3 = ResidualBlock(128)
        self.res4 = ResidualBlock(128)
        self.res5 = ResidualBlock(128)

        # decoding layers
        self.deconv3 = UpsampleConvLayer(128, 64, kernel_size=3, stride=1, upsample=2 )
        self.in3_d = nn.InstanceNorm2d(64, affine=True)

        self.deconv2 = UpsampleConvLayer(64, 32, kernel_size=3, stride=1, upsample=2 )
        self.in2_d = nn.InstanceNorm2d(32, affine=True)

        self.deconv1 = UpsampleConvLayer(32, 3, kernel_size=9, stride=1)
        self.in1_d = nn.InstanceNorm2d(3, affine=True)

    def forward(self, x):
        # encode
        y = self.relu(self.in1_e(self.conv1(x)))
        y = self.relu(self.in2_e(self.conv2(y)))
        y = self.relu(self.in3_e(self.conv3(y)))

        # residual layers
        y = self.res1(y)
        y = self.res2(y)
        y = self.res3(y)
        y = self.res4(y)
        y = self.res5(y)

        # decode
        y = self.relu(self.in3_d(self.deconv3(y)))
        y = self.relu(self.in2_d(self.deconv2(y)))
        #y = self.tanh(self.in1_d(self.deconv1(y)))
        y = self.deconv1(y)

        return y

    def init_weights(self):
        a = 1



class HeadNet_reg(nn.Module):
    def __init__(self):
        super(HeadNet_reg, self).__init__()
        self.fc1 = nn.Linear(2048, 256)
        self.batch_norm1 = nn.BatchNorm1d(256)
        
        self.fc_neg_thorax = nn.Linear(256, 3)
        self.fc_norm_bone = nn.Linear(256, 45)
        
        self.avgpool = nn.AvgPool2d(7)
        self.relu = nn.ReLU(inplace=True)
        
    def forward(self, x):
        x = self.avgpool(x)
        x = x.view(-1, 2048)
        x = self.fc1(x)
        x = self.batch_norm1(x)
        x = self.relu(x)
        
        neg_thorax = self.fc_neg_thorax(x)
        norm_bone = self.fc_norm_bone(x)

        return neg_thorax, norm_bone
    def init_weights(self):
        for m in self.modules():
           if isinstance(m, nn.Linear):
              nn.init.normal_(m.weight, std=0.05)
            
class HeadNet(nn.Module):

    def __init__(self, joint_num=16, ge_res50 = True):
        self.inplanes = 2048
        if ge_res50 == False:
            self.inplanes = 512
        self.outplanes = 256

        super(HeadNet, self).__init__()

        self.deconv_layers = self._make_deconv_layer(3)
        self.final_layer = nn.Conv2d(
            in_channels=self.inplanes,
            out_channels=global_joint_num * depth_dim,
            kernel_size=1,
            stride=1,
            padding=0
        )

        self.mapping_cylinder_layer = nn.Conv2d(
            in_channels=self.inplanes,
            out_channels=1 * depth_dim,
            kernel_size=1,
            stride=1,
            padding=0
        )

        self.ske_3d_layer = nn.Conv2d(
            in_channels=self.inplanes,
            out_channels=3 * depth_dim,
            kernel_size=1,
            stride=1,
            padding=0
        )

    def _make_deconv_layer(self, num_layers):
        layers = []
        for i in range(num_layers):
            layers.append(
                nn.ConvTranspose2d(
                    in_channels=self.inplanes,
                    out_channels=self.outplanes,
                    kernel_size=4,
                    stride=2,
                    padding=1,
                    output_padding=0,
                    bias=False))
            layers.append(nn.BatchNorm2d(self.outplanes))
            layers.append(nn.ReLU(inplace=True))
            self.inplanes = self.outplanes

        return nn.Sequential(*layers)

    def forward(self, x):
        x = self.deconv_layers(x)
        t_x = x
        x = self.final_layer(t_x)

        return x#, y

    def init_weights(self):
        for name, m in self.deconv_layers.named_modules():
            if isinstance(m, nn.ConvTranspose2d):
                nn.init.normal_(m.weight, std=0.001)
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)
        for m in self.final_layer.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.normal_(m.weight, std=0.001)
                nn.init.constant_(m.bias, 0)

class ResDeconvNet(nn.Module):
    def __init__(self, backbone):
        super(ResDeconvNet, self).__init__()
        self.backbone = backbone

    def forward(self, x, y):
        x = torch.cat((x, y), dim = 1)
        x = self.backbone(x)
        return x


class ResDeconvSelfSupNet(nn.Module):
    def __init__(self, backbone):
        super(ResDeconvSelfSupNet, self).__init__()
        self.backbone = backbone

    def forward(self, x):
        num_elem = len(x)
        inp_2_self_sup = x[0]
        for i in range(1, num_elem):
            inp_2_self_sup = torch.cat((inp_2_self_sup, x[i]), dim = 1)
        ret = self.backbone(inp_2_self_sup)

        return ret

class ResPoseNet(nn.Module):
    def __init__(self, backbone, head):
        super(ResPoseNet, self).__init__()
        self.backbone = backbone
        self.head = head

    def forward(self, x):
        
        x, sign_res3c, sign_res4f, sign_res5c, sign_res3c_deconv = self.backbone(x)
        x = self.head(x)
        return x, sign_res3c, sign_res4f, sign_res5c, sign_res3c_deconv


class ResPoseNet2DSke(nn.Module):
    def __init__(self, backbone, head):
        super(ResPoseNet2DSke, self).__init__()
        self.backbone = backbone
        self.head = head

    def forward(self, x):
        
        x, sign_res3c, sign_res4f, sign_res5c, sign_res3c_deconv = self.backbone(x)
        neg_thorax, norm_bone = self.head(x)
        return neg_thorax, norm_bone
# string 'resnet18' 'resnet34' 'resnet50' 'resnet101' 'resnet152'

#vgg definition that conveniently let's you grab the outputs from any layer
class VGG(nn.Module):
    def __init__(self, pool='max'):
        super(VGG, self).__init__()
        #vgg modules
        self.conv1_1 = nn.Conv2d(3, 64, kernel_size=3, padding=1)
        self.conv1_2 = nn.Conv2d(64, 64, kernel_size=3, padding=1)
        self.conv2_1 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
        self.conv2_2 = nn.Conv2d(128, 128, kernel_size=3, padding=1)
        self.conv3_1 = nn.Conv2d(128, 256, kernel_size=3, padding=1)
        self.conv3_2 = nn.Conv2d(256, 256, kernel_size=3, padding=1)
        self.conv3_3 = nn.Conv2d(256, 256, kernel_size=3, padding=1)
        self.conv3_4 = nn.Conv2d(256, 256, kernel_size=3, padding=1)
        self.conv4_1 = nn.Conv2d(256, 512, kernel_size=3, padding=1)
        self.conv4_2 = nn.Conv2d(512, 512, kernel_size=3, padding=1)
        self.conv4_3 = nn.Conv2d(512, 512, kernel_size=3, padding=1)
        self.conv4_4 = nn.Conv2d(512, 512, kernel_size=3, padding=1)
        self.conv5_1 = nn.Conv2d(512, 512, kernel_size=3, padding=1)
        self.conv5_2 = nn.Conv2d(512, 512, kernel_size=3, padding=1)
        self.conv5_3 = nn.Conv2d(512, 512, kernel_size=3, padding=1)
        self.conv5_4 = nn.Conv2d(512, 512, kernel_size=3, padding=1)
        if pool == 'max':
            self.pool1 = nn.MaxPool2d(kernel_size=2, stride=2)
            self.pool2 = nn.MaxPool2d(kernel_size=2, stride=2)
            self.pool3 = nn.MaxPool2d(kernel_size=2, stride=2)
            self.pool4 = nn.MaxPool2d(kernel_size=2, stride=2)
            self.pool5 = nn.MaxPool2d(kernel_size=2, stride=2)
        elif pool == 'avg':
            self.pool1 = nn.AvgPool2d(kernel_size=2, stride=2)
            self.pool2 = nn.AvgPool2d(kernel_size=2, stride=2)
            self.pool3 = nn.AvgPool2d(kernel_size=2, stride=2)
            self.pool4 = nn.AvgPool2d(kernel_size=2, stride=2)
            self.pool5 = nn.AvgPool2d(kernel_size=2, stride=2)
            
    def forward(self, x, out_keys):
        out = {}
        out['r11'] = F.relu(self.conv1_1(x))
        out['r12'] = F.relu(self.conv1_2(out['r11']))
        out['p1'] = self.pool1(out['r12'])
        out['r21'] = F.relu(self.conv2_1(out['p1']))
        out['r22'] = F.relu(self.conv2_2(out['r21']))
        out['p2'] = self.pool2(out['r22'])
        out['r31'] = F.relu(self.conv3_1(out['p2']))
        out['r32'] = F.relu(self.conv3_2(out['r31']))
        out['r33'] = F.relu(self.conv3_3(out['r32']))
        out['r34'] = F.relu(self.conv3_4(out['r33']))
        out['p3'] = self.pool3(out['r34'])
        out['r41'] = F.relu(self.conv4_1(out['p3']))
        out['r42'] = F.relu(self.conv4_2(out['r41']))
        out['r43'] = F.relu(self.conv4_3(out['r42']))
        out['r44'] = F.relu(self.conv4_4(out['r43']))
        out['p4'] = self.pool4(out['r44'])
        out['r51'] = F.relu(self.conv5_1(out['p4']))
        out['r52'] = F.relu(self.conv5_2(out['r51']))
        out['r53'] = F.relu(self.conv5_3(out['r52']))
        out['r54'] = F.relu(self.conv5_4(out['r53']))
        out['p5'] = self.pool5(out['r54'])
        return [out[key] for key in out_keys]


def get_deconv_net(is_train):
    backbone_nst = ImageTransformNet()# ResNetBackbone(18, is_pose_net = False)# ImageTransformNet() #ResNetBackbone(18, is_pose_net = False)
    if is_train:
        backbone_nst.init_weights()
        
    model_deconv = ResDeconvNet(backbone_nst)

    return model_deconv


def get_deconv_self_sup_net(is_train):
    backbone_self_sup_nst = ImageTransformSelfSupNet()# ResNetBackbone(18, is_pose_net = False)# ImageTransformNet() #ResNetBackbone(18, is_pose_net = False)
    if is_train:
        backbone_self_sup_nst.init_weights()
        
    model_self_sup_deconv = ResDeconvSelfSupNet(backbone_self_sup_nst)

    return model_self_sup_deconv


def get_pose_net(is_train):
    backbone = ResNetBackbone(50)
    head_net = HeadNet()
    if is_train:
        backbone.init_weights()
        head_net.init_weights()
        
    model = ResPoseNet(backbone, head_net)

    return model

#2d ske -> pose

def get_pose_net_2dske(is_train):
    backbone = ResNetBackbone(50, is_pose_net = True, num_channel_first_layer = 41)
    head_net = HeadNet_reg()#(ge_res50 = True)
    if is_train:
        backbone.init_weights()
        head_net.init_weights()
        
    model = ResPoseNet2DSke(backbone, head_net)

    return model


model_deconv = get_deconv_net(True)
model_deconv = DataParallelModel(model_deconv).cuda()

# For self sup
model_self_sup_deconv = get_deconv_self_sup_net(True)
model_self_sup_deconv = DataParallelModel(model_self_sup_deconv).cuda()


model = get_pose_net(True)        
model = DataParallelModel(model).cuda()

#2d ske -> pose
model_2dske = get_pose_net_2dske(True)
model_2dske = DataParallelModel(model_2dske).cuda()


model_dir = os.getcwd() + '/Models/'
#get network
vgg = VGG()

vgg.load_state_dict(torch.load(model_dir + 'vgg_conv.pth'))
for param in vgg.parameters():
    param.requires_grad = False
if torch.cuda.is_available():
    vgg = DataParallelModel(vgg).cuda()

#3.    possible loading model load model finetune
#print(model.state_dict())
def load_model():
    integral_model_str = '124_4' #'114_16' #currently the best in terms of 2d pckh
    ckpt = torch.load('resnet50/model256_mpii_ist_' + integral_model_str + '.pth')
    model.load_state_dict(ckpt)

    ckpt_2dske = torch.load('resnet50/model256_mpii_ist_2dske_to_pose_' + model_str + '.pth')
    ckpt_2dske.pop('module.backbone.conv1.weight', None)
    ckpt_2dske.pop('module.backbone.conv1.bias', None)
    
    load_my_state_dict(model_2dske, ckpt_2dske)
    
    deconv_model_str = '124_4' # '114_16'
    ckpt_deconv = torch.load('resnet50/model256_mpii_ist_deconv_'  + deconv_model_str + '.pth')
    
    model_deconv.load_state_dict(ckpt_deconv)

    self_sup_model_str = '124_4'
    ckpt_self_sup_deconv = torch.load('resnet50/model256_mpii_ist_self_sup_deconv_'  + self_sup_model_str + '.pth')
    ckpt_self_sup_deconv.pop('module.backbone.conv1.conv2d.weight', None)
    ckpt_self_sup_deconv.pop('module.backbone.conv1.conv2d.bias', None)
    load_my_state_dict(model_self_sup_deconv, ckpt_self_sup_deconv)
    
load_model()

#4. logging
import logging
import os

OK = '\033[92m'
WARNING = '\033[93m'
FAIL = '\033[91m'
END = '\033[0m'

PINK = '\033[95m'
BLUE = '\033[94m'
GREEN = OK
RED = FAIL
WHITE = END
YELLOW = WARNING

class colorlogger():
    def __init__(self, log_dir, log_name='train_logs.txt'):
        # set log
        self._logger = logging.getLogger(log_name)
        self._logger.setLevel(logging.INFO)
        log_file = os.path.join(log_dir, log_name)
        if not os.path.exists(log_dir):
            os.makedirs(log_dir)
        file_log = logging.FileHandler(log_file, mode='a')
        file_log.setLevel(logging.INFO)
        formatter = logging.Formatter(
            "{}%(asctime)s{} %(message)s".format(GREEN, END),
            "%m-%d %H:%M:%S")
        file_log.setFormatter(formatter)
        #console_log.setFormatter(formatter)
        self._logger.addHandler(file_log)
        #self._logger.addHandler(console_log)

    def debug(self, msg):
        self._logger.debug(str(msg))

    def info(self, msg):
        self._logger.info(str(msg))

    def warning(self, msg):
        self._logger.warning(WARNING + 'WRN: ' + str(msg) + END)

    def critical(self, msg):
        self._logger.critical(RED + 'CRI: ' + str(msg) + END)

    def error(self, msg):
        self._logger.error(RED + 'ERR: ' + str(msg) + END)

logger = colorlogger("logs/", log_name="logs_resnet50_32x8d_mpii.txt")
train_loss_logger = colorlogger("logs/", log_name="train_loss_resnet50_32x8d_mpii.txt")
# val: one element per epoch 
val_loss_logger = colorlogger("logs/", log_name="val_loss_resnet50_32x8d_mpii.txt")

#5. lr
lr = 0.0000006
weight_decay = 1e-5
momentum = 0.9
train_loss_arr = []
val_loss_arr = [] #per epoch one value
train_loss_epoch_arr = [] #per-epoch train error (take the average of each iteration)


for param in model_deconv.parameters():
    param.requires_grad = True
            
for param in model.parameters():
    param.requires_grad = True #False

for param in model_self_sup_deconv.parameters():
    param.requires_grad = True
            

params = list(model.parameters()) + list(model_deconv.parameters()) + list(model_2dske.parameters()) + list(model_self_sup_deconv.parameters())

#optimizer torch.optim.Adam(params, lr=lr)# optim.SGD(model.parameters(), lr=lr, momentum=momentum)
optimizer = torch.optim.RMSprop(params, lr=lr, momentum=momentum, weight_decay=weight_decay) 

#optimizer = torch.optim.SGD(params, lr=lr, momentum=momentum, weight_decay=weight_decay) 

lr_dec_epoch = [25, 55]
lr_dec_factor = 0.1
scheduler = torch.optim.lr_scheduler.MultiStepLR(optimizer, milestones=lr_dec_epoch, gamma=lr_dec_factor)

log_interval = 1
epochs = 100000

#6. training
def train(epoch):
    #print(model)
    
    model.train()
    cur_epoch_avg_loss = 0
    cnt = 0
    scheduler.step()
    #for iter_id in range(per_epoch_iters):
    for iter_id in range(1):
        tot = 0

        for batch_idx, (style, content, joint_img, joint_vis,  joints_have_depth, gt_ske, data_source, gt_3dske, joint_bone, neg_thorax) in enumerate(train_loader):
            style = style.cuda()
            content = content.cuda()
            gt_ske = gt_ske.cuda()
            gt_3dske = gt_3dske.cuda()
            gt_joint_3d = joint_img
            joint_img = joint_img.cuda()
            joint_vis = joint_vis.cuda()
            joints_have_depth = joints_have_depth.cuda()
            data_source = data_source.cuda()
            
            norm_bone = joint_bone.cuda()
            neg_thorax = neg_thorax.cuda()
                    
            # Replace optimization (L-BFGS) with deconv network

            opt = model_deconv(style, content)
            style_ent_layers = ['r11','r12', 'r21'] 
            content_layers = ['r42']#'r52', 'r53', 'r54']
            content_ent_layers = ['r52', 'r53', 'r54']
            
            style_layers = ['r11','r21','r31','r41', 'r51'] 
            loss_layers = style_layers + content_layers
            loss_layers_self_sup = style_layers + content_layers
            
            loss_fns = [GramMSELoss()] * len(style_layers) + [nn.MSELoss()] * len(content_layers)
            loss_fns_self_sup = [GramMSELoss()] * len(style_layers) + [nn.MSELoss()] * len(content_layers) + [nn.CosineSimilarity(dim=1, eps=1e-6)] * len(content_layers)
            
            if torch.cuda.is_available():
                loss_fns = [loss_fn.cuda() for loss_fn in loss_fns]
                loss_fns_self_sup = [loss_fn_self_sup.cuda() for loss_fn_self_sup in loss_fns_self_sup]
    
            #these are good weights settings:
            style_weights = [1e3/n**2 for n in [64,128,256,512,512]] #[1e0, 0.002, 5e-4, 1e-5, 0.01]
            content_weights = [1, 1, 1, 1, 1]# [0.01, 0.06, 0.26, 1]
            weights = style_weights + content_weights
            content_weights_selfsup = [0.01, 0.01, 0.01, 0.01, 0.01]# [0.01, 0.06, 0.26, 1]
            content_weights_selfsup2 = [0.5, 0.5, 0.5, 0.5, 0.5]# [0.01, 0.06, 0.26, 1]


            style_weights_selfsup = [1e3/n**2 for n in [64,128,256,512,512]] #[1e0, 0.002, 5e-4, 1e-5, 0.01]

            weights_self_sup = style_weights + content_weights + content_weights_selfsup
            #compute optimization targets
            
            style_targets = [GramMatrix()(A).detach() for A in vgg(style, style_layers)]
            content_targets = [A.detach() for A in vgg(content, content_layers)]
            targets = style_targets + content_targets
            #print(targets)
            
            # USE Deconv opt -> loss network -> 
            # style & content from deconv opt
            out = vgg(opt, loss_layers)
            style_targets_out = [A.detach() for A in vgg(style, style_ent_layers)]
            content_targets_out = [A.detach() for A in vgg(content, content_ent_layers)]
            
            # Pass the stylized transferred "out" to VGG to get its gram matrix 
            style_targets_opt = [GramMatrix()(A).detach() for A in vgg(opt, style_layers)]
            content_targets_opt = [A.detach() for A in vgg(opt, content_layers)]
            
            style_targets_out_opt = [A.detach() for A in vgg(opt, style_ent_layers)]
            content_targets_out_opt = [A.detach() for A in vgg(opt, content_ent_layers)]
            
            targets_self_sup = style_targets_opt + content_targets_opt + content_targets_opt
            
            opt_style_out = [A.detach() for A in vgg(opt, style_ent_layers)]
            opt_content_out = [A.detach() for A in vgg(opt, content_ent_layers)]
            target_h = 0 #red color
            target_s = 0.5  
            target_v = 0.8

            target_hsv = HSVLoss()
            target_hue, target_saturation, target_value = target_hsv.get_hsv(style)
            target_hue = torch.mean(target_hue)
            target_saturation = torch.mean(target_saturation)
            target_value = torch.mean(target_value)
            
            opt_hsv = HSVLoss()
            opt_hue, opt_saturation, opt_value = opt_hsv.get_hsv(opt)
            loss_hsv = HSVLoss(h=target_hue.item(), s=target_saturation.item(), v=target_value.item(), threshold_h=0.03, threshold_sv=0.1)
            loss_hsv = loss_hsv(opt)

            style_srgb = rgb_to_srgb(style)
            #print(style_srgb)
            opt_srgb = rgb_to_srgb(opt)
            srgb_loss = nn.CosineSimilarity(dim=1, eps=1e-6)
            srgb_loss = srgb_loss(opt, style)
            srgb_loss = srgb_loss.mean()
            
            # match the style (content layer of VGG (opt, style))
            match_style_cos = [nn.CosineSimilarity(dim=1, eps=1e-6)] * len(content_layers)
            match_style_cos = [fn.cuda() for fn in match_style_cos]
            style_targets_cos = [A.detach() for A in vgg(style, content_layers)]
            style_opt_cos = vgg(opt, content_layers)
            #    match content layers
            layer_losses_match_style_cos = [content_weights[a] * match_style_cos[a](A, style_targets_cos[a]) for a,A in enumerate(style_opt_cos)]
            layer_losses_style_cos = [content_weights[a] * match_style_cos[a](A, style_targets_cos[a]) for a,A in enumerate(style_opt_cos)]
            

            # KL JS losses
            style_ent_loss = 0
            content_ent_loss = 0
            # ==== STYLE
            for i in range(len(style_targets_out)):
                b, c, h, w = style_targets_out[i].shape
                #GT style VGG features (style)
                style_targets_out[i] = style_targets_out[i].reshape(b, c, h * w)
                style_targets_out[i] = nn.Softmax(dim=2)(style_targets_out[i])
                style_targets_out[i] = style_targets_out[i].reshape(b, c, h, w)

                #opt image VGG features(style)
                opt_style_out[i] = opt_style_out[i].reshape(b, c, h * w)
                opt_style_out[i] = nn.Softmax(dim=2)(opt_style_out[i])
                opt_style_out[i] = opt_style_out[i].reshape(b, c, h, w)
                cur_loss = js_div_loss_2d(style_targets_out[i], opt_style_out[i])
                style_ent_loss += cur_loss

            # ==== CONTENT
            for i in range(len(content_targets_out)):
                b, c, h, w = content_targets_out[i].shape
                #GT content VGG features (content)
                content_targets_out[i] = content_targets_out[i].reshape(b, c, h * w)
                content_targets_out[i] = nn.Softmax(dim=2)(content_targets_out[i])
                content_targets_out[i] = content_targets_out[i].reshape(b, c, h, w)

                #opt image VGG features(content)
                opt_content_out[i] = opt_content_out[i].reshape(b, c, h * w)
                opt_content_out[i] = nn.Softmax(dim=2)(opt_content_out[i])
                opt_content_out[i] = opt_content_out[i].reshape(b, c, h, w)
                cur_loss = js_div_loss_2d(content_targets_out[i], opt_content_out[i])
                content_ent_loss += cur_loss


            layer_losses = [weights[a] * loss_fns[a](A, targets[a]) for a,A in enumerate(out)]
            ll = layer_losses
            
            
            ref_loss = layer_losses[4].item()
            for j in range(0, len(layer_losses)):
                cur_loss = layer_losses[j].item()
                if cur_loss > 0.00000001:
                    w_ = ref_loss / (cur_loss)
                else:
                    w_ = 1.0
                layer_losses[j] = layer_losses[j] * w_
                        
            # calculate total variation regularization (anisotropic version)
            # https://www.wikiwand.com/en/Total_variation_denoising
            diff_i = torch.sum(torch.abs(opt[:, :, :, 1:] - opt[:, :, :, :-1]))
            diff_j = torch.sum(torch.abs(opt[:, :, 1:, :] - opt[:, :, :-1, :]))
            tv_loss = TV_WEIGHT*(diff_i + diff_j)
            print('layer losses')
            print(layer_losses)
            cos = nn.CosineSimilarity(dim=1, eps=1e-6)
            cos_loss = cos(opt, style)
            cos_loss = cos_loss.mean()
            nst_loss = sum(layer_losses) + tv_loss + 1000 * cos_loss #1000 * cos_loss

            optimizer.zero_grad()
            heatmap_out, res3c, res4f, res5c, res3c_deconv = model(opt)
            
            # DEFINE skeleton used in the merged dataset
            if train_dataset.multiple_db == True:
                myske = train_dataset.skeleton[0]
            else:
                myske = train_dataset.skeleton

            # Generate bone img ground truth
            pixel_thresh = 0.3
            eps = 0.001
            sigma_0 = 0.025
            sigma_1 = 2.35
            max_a = 10.0 / float(img_size)

            render_size = img_size
            bone_img_arr = np.zeros((batch_size, img_size, img_size, 3), dtype=np.float32)
            bone_img_arr_per_bone = np.zeros((batch_size, img_size, img_size, len(myske)), dtype=np.float32)
            

            bone_img_arr_oval = np.zeros((batch_size, img_size, img_size, 3), dtype=np.float32)
            bone_img_arr_per_bone_oval = np.zeros((batch_size, img_size, img_size, len(myske)), dtype=np.float32)
            
            use_bone_img = True
            use_oval = True
            if use_bone_img:
                for i in range(batch_size):
                    coord_out = soft_argmax(heatmap_out[i], global_joint_num).data.cpu().numpy()[0]
                    coord_out = coord_out * 4.0
                  
                    bone_img = np.zeros((img_size, img_size, 3), dtype=np.float32)
                    bone_img_oval = np.zeros((img_size, img_size, 3), dtype=np.float32)
                    for bone in range(len(myske)):
                        i1 = myske[bone][0]
                        i2 = myske[bone][1]
                        b = train_dataset.ske_colors[bone][0]
                        g = train_dataset.ske_colors[bone][1]
                        r = train_dataset.ske_colors[bone][2]
                        x1 = coord_out[i1][0] / float(img_size)
                        y1 = coord_out[i1][1] / float(img_size)
                        x2 = coord_out[i2][0] / float(img_size)
                        y2 = coord_out[i2][1] / float(img_size)
                        minx = min(x1, x2) 
                        maxx = max(x1, x2) 
                        miny = min(y1, y2) 
                        maxy = max(y1, y2) 

                        minxx = min(x1, x2) - 0.02
                        maxxx = max(x1, x2) + 0.02
                        minyy = min(y1, y2) - 0.02
                        maxyy = max(y1, y2) + 0.02

                        blank_image = np.zeros((img_size,img_size,1), np.uint8)
                        blank_image_oval = np.zeros((img_size,img_size,1), np.uint8)
                        
                        max_b = 0.5 * math.sqrt(math.pow(x1 - x2, 2) + math.pow(y1 - y2, 2))
                        
                        if use_oval:
                            for col in range(0, render_size):
                                    x = (col + 0.5) / float(render_size);
                                    if (math.fabs(x - minxx) < eps or math.fabs(x - maxxx) < eps or (x - minxx > eps and maxxx - x > eps)):
                                        for row in range(0, render_size):
                                            y = (row + 0.5) / float(render_size)
                                            if (math.fabs(y - minyy) < eps or math.fabs(y - maxyy) < eps or (y - minyy > eps and maxyy - y > eps)):
                                    
                                                dist_2_line = math.fabs((y2 - y1) * x - (x2 - x1) * y + x2 * y1 - y2 * x1) / math.sqrt(math.pow(y2 - y1, 2) + math.pow(x2 - x1, 2) + 0.0000000000000000001)
                                                dist_minor_axis = math.pow(x - (x1 + x2) / 2.0, 2) + math.pow(y - (y1 + y2) / 2.0, 2) - math.pow(dist_2_line, 2)
                                                dist_minor_axis = max(0.0, dist_minor_axis)
                                                dist_minor_axis = math.sqrt(dist_minor_axis)

                                                dist = math.pow(dist_2_line, 2) / math.pow(max_a, 2) + math.pow(dist_minor_axis, 2) / math.pow(max_b, 2)
                                                if 1.0 - dist > 1e-6:
                                                    blank_image_oval[row][col][0] = 1.0 * 255
                                                    bone_img_oval[row][col][0] += blank_image_oval[row][col][0] / 255.0 * b / 255.0
                                                    bone_img_oval[row][col][1] += blank_image_oval[row][col][0] / 255.0 * g / 255.0
                                                    bone_img_oval[row][col][2] += blank_image_oval[row][col][0] / 255.0 * r / 255.0
                                                    bone_img_arr_oval[i][row][col][0] += bone_img_oval[row][col][0]          
                                                    bone_img_arr_oval[i][row][col][1] += bone_img_oval[row][col][1]          
                                                    bone_img_arr_oval[i][row][col][2] += bone_img_oval[row][col][2] 
                                                
                                                bone_img_arr_per_bone_oval[i][row][col][bone] = (blank_image_oval[row][col][0] + blank_image_oval[row][col][0] + blank_image_oval[row][col][0]) / 3.0 / 255.0 
                        
                        if (math.fabs(x1 - x2) < eps):
                            for col in range(0, render_size):
                                x = (col + 0.5) / float(render_size)
                                for row in range(0, render_size):
                                    y = (row + 0.5) / float(render_size)
                                    if (math.fabs(y - miny) < eps or math.fabs(y - maxy) < eps or (y - miny > eps and maxy - y > eps)):
                                        dsquare1 = -1.0 / (2.0 * sigma_0 * sigma_0) * math.pow(x - x1, 2)
                                        dsquare2 = -1.0 / (2.0 * sigma_1 * sigma_1) * math.pow((y - y1) / (y2 - y1), 2)
                                        blank_image[row][col][0] = math.exp(dsquare1) * math.exp(dsquare2) * 255
                                        bone_img[row][col][0] += blank_image[row][col][0] / 255.0 * b / 255.0
                                        bone_img[row][col][1] += blank_image[row][col][0] / 255.0 * g / 255.0
                                        bone_img[row][col][2] += blank_image[row][col][0] / 255.0 * r / 255.0
                                        bone_img_arr[i][row][col][0] += bone_img[row][col][0]          
                                        bone_img_arr[i][row][col][1] += bone_img[row][col][1]          
                                        bone_img_arr[i][row][col][2] += bone_img[row][col][2] 
                                    bone_img_arr_per_bone[i][row][col][bone] = (blank_image[row][col][0] + blank_image[row][col][0] + blank_image[row][col][0]) / 3.0 / 255.0       
                                  
                        elif (math.fabs(y1 - y2) < eps):
                            for row in range(0, render_size):
                                y = (row + 0.5) / float(render_size)
                                for col in range(0, render_size):
                                    x = (col + 0.5) / float(render_size)
                                    if (math.fabs(x - minx) < eps or math.fabs(x - maxx) < eps or (x - minx > eps and maxx - x > eps)):
                                        dsquare1 = -1.0 / (2.0 * sigma_0 * sigma_0) * math.pow(y - y1, 2)
                                        dsquare2 = -1.0 / (2.0 * sigma_1 * sigma_1) * math.pow((x - x1) / (x2 - x1), 2)
                                        blank_image[row][col][0] = math.exp(dsquare1) * math.exp(dsquare2) * 255
                                        bone_img[row][col][0] += blank_image[row][col][0] / 255.0 * b / 255.0
                                        bone_img[row][col][1] += blank_image[row][col][0] / 255.0 * g / 255.0
                                        bone_img[row][col][2] += blank_image[row][col][0] / 255.0 * r / 255.0
                                        bone_img_arr[i][row][col][0] += bone_img[row][col][0]          
                                        bone_img_arr[i][row][col][1] += bone_img[row][col][1]          
                                        bone_img_arr[i][row][col][2] += bone_img[row][col][2] 
                                    bone_img_arr_per_bone[i][row][col][bone] = (blank_image[row][col][0] + blank_image[row][col][0] + blank_image[row][col][0]) / 3.0 / 255.0       
                        else:
                            A = (y2 - y1) / (x2 - x1)
                            B = -1.0
                            C = y1 - A * x1
                            for col in range(0, render_size):
                                x = (col + 0.5) / float(render_size);
                                if (math.fabs(x - minx) < eps or math.fabs(x - maxx) < eps or (x - minx > eps and maxx - x > eps)):
                                    for row in range(0, render_size):
                                        y = (row + 0.5) / float(render_size)
                                        if (math.fabs(y - miny) < eps or math.fabs(y - maxy) < eps or (y - miny > eps and maxy - y > eps)):
                                            dsquare0 = -1.0 / (2.0 * sigma_0 * sigma_0) * math.pow(A * x + B * y + C, 2) / (math.pow(A, 2) + math.pow(B, 2))
                                            dsquare1 = -1.0 / (2.0 * sigma_1 * sigma_1) * math.pow((x - x1) / (x2 - x1), 2)
                                            dsquare2 = -1.0 / (2.0 * sigma_1 * sigma_1) * math.pow((y - y1) / (y2 - y1), 2)
                                            blank_image[row][col][0] = math.exp(dsquare0) * math.exp(dsquare1) * math.exp(dsquare2) * 255
                                            bone_img[row][col][0] += blank_image[row][col][0] / 255.0 * b / 255.0
                                            bone_img[row][col][1] += blank_image[row][col][0] / 255.0 * g / 255.0
                                            bone_img[row][col][2] += blank_image[row][col][0] / 255.0 * r / 255.0
                        
                                            bone_img_arr[i][row][col][0] += bone_img[row][col][0]          
                                            bone_img_arr[i][row][col][1] += bone_img[row][col][1]          
                                            bone_img_arr[i][row][col][2] += bone_img[row][col][2]        
                                        
                                        bone_img_arr_per_bone[i][row][col][bone] = (blank_image[row][col][0] + blank_image[row][col][0] + blank_image[row][col][0]) / 3.0 / 255.0 
                    cv2.imshow("blank", bone_img_arr[i])
                    cv2.waitKey(1)
                    if use_oval:
                        cv2.imshow("blank_oval", bone_img_arr_oval[i])
                        cv2.waitKey(1)
                        
                # Bone img concatenate style -> deconv a stylized image
                # [0, 1] -> std mean normalize it
                pre_transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=pixel_mean, std=pixel_std)])
                bone_img_arr = bone_img_arr.transpose(0,3,1,2)
                if use_oval:
                    bone_img_arr_oval = bone_img_arr_oval.transpose(0,3,1,2)
                

            # the original gt ske (old-fashioned)
            # 7. Generate ground truth skeleton
            ske_img_arr = np.zeros((batch_size, 3, img_size, img_size), dtype=np.float32)
            
            for i in range(batch_size):
                #print(i)
                coord_out = soft_argmax(heatmap_out[i], global_joint_num).data.cpu().numpy()[0]
                coord_out = coord_out * 4.0
                pred_ske = np.zeros((img_size, img_size, 3),dtype=np.uint8)
            
                for j in range(len(myske)):
                    i1 = myske[j][0]
                    i2 = myske[j][1]
                    
                    x1 = coord_out[i1][0]
                    y1 = coord_out[i1][1]
                    x2 = coord_out[i2][0]
                    y2 = coord_out[i2][1]
                    
                    if joint_vis[i][i1] == 1 and joint_vis[i][i2] == 1:                       
                        cv2.line(
                        pred_ske, (x1.astype(np.int32), y1.astype(np.int32)), (x2.astype(np.int32), y2.astype(np.int32)),
                        color=train_dataset.ske_colors[j], thickness=3, lineType=cv2.LINE_AA)
                        #print('connecting ', j, 'done')
                pred_ske_ = cv2.resize(pred_ske, (512, 512))                  
                pred_ske = pred_ske.transpose((2, 0, 1))
                pred_ske_out = np.zeros((3, img_size, img_size),dtype=np.float32)
                pred_ske_out[:, :, :] = pred_ske[:, :, :] / 255.0
                ske_img_arr[i] = pred_ske_out
                
            #transform
            if use_bone_img:
                for i in range(batch_size):
                    cur_img = bone_img_arr[i].transpose(1,2,0)
                    bone_img_arr[i] = pre_transforms(cur_img)

                    if use_oval:
                        cur_img_oval = bone_img_arr_oval[i].transpose(1,2,0)
                        bone_img_arr_oval[i] = pre_transforms(cur_img_oval)

                bone_img_arr = torch.from_numpy(bone_img_arr).cuda()
                if use_oval:
                    bone_img_arr_oval = torch.from_numpy(bone_img_arr_oval).cuda()

                bone_img_arr_per_bone = bone_img_arr_per_bone.transpose(0,3,1,2)
                bone_img_arr_per_bone = torch.from_numpy(bone_img_arr_per_bone).cuda()

                if use_oval:
                    bone_img_arr_per_bone_oval = bone_img_arr_per_bone_oval.transpose(0,3,1,2)
                    bone_img_arr_per_bone_oval = torch.from_numpy(bone_img_arr_per_bone_oval).cuda()

            #2d ske -> pose
            #inp_2dske_to_pose = torch.cat((bone_img_arr, bone_img_arr_oval), dim = 1)#, bone_img_arr_per_bone, bone_img_arr_per_bone_oval), dim = 1)
            t_ske_img_arr = ske_img_arr
            
            ske_img_arr = torch.from_numpy(ske_img_arr).cuda()            
            
            inp_2dske_to_pose = torch.cat((bone_img_arr_per_bone_oval, bone_img_arr_per_bone, bone_img_arr_oval, bone_img_arr, ske_img_arr),dim=1) #ske_img_arr # bone_img_arr_oval # ske_img_arr# bone_img_arr_oval
            #heatmap_out_2dske_to_pose, res3c_, res4f_, res5c_, res3c_deconv_ = model_2dske(inp_2dske_to_pose)
            neg_thorax_pred, norm_bone_pred = model_2dske(inp_2dske_to_pose)
            
            # -> CPU
            neg_thorax_pred = neg_thorax_pred.data.cpu().numpy()
            neg_thorax = neg_thorax.data.cpu().numpy()
            norm_bone_pred = norm_bone_pred.data.cpu().numpy()
            norm_bone = norm_bone.data.cpu().numpy()
            
            neg_thorax_pred = unnorm_neg_thorax(neg_thorax_pred)
            neg_thorax = unnorm_neg_thorax(neg_thorax) #gt

            norm_bone_pred = unnorm_bone(norm_bone_pred)
            norm_bone = unnorm_bone(norm_bone) #gt           
            # predicted joint reconstruction
            joint_pred_reconstruct = bone2joint(norm_bone_pred)
            pred_thorax = -neg_thorax_pred 
            for j in range(18):
                if j != thorax:
                    joint_pred_reconstruct[:,j * 3:(j + 1) * 3] += pred_thorax[:,:]
                else:
                    joint_pred_reconstruct[:,j * 3:(j + 1) * 3] = pred_thorax[:,:]
            # gt joint 
            joint_gt_reconstruct = bone2joint(norm_bone)
            gt_thorax = -neg_thorax 
            for j in range(18):
                if j != thorax:
                    joint_gt_reconstruct[:,j * 3:(j + 1) * 3] += gt_thorax[:,:]
                else:
                    joint_gt_reconstruct[:,j * 3:(j + 1) * 3] = gt_thorax[:,:]            
            reconstruct_diff = (joint_pred_reconstruct - joint_gt_reconstruct)
            for j in range(reconstruct_diff.shape[0]):
                for k in range(reconstruct_diff.shape[1]):
                    reconstruct_diff[j][k] = math.fabs(reconstruct_diff[j][k])            
            pred_proj = pinhole_proj(joint_pred_reconstruct)
            print('pred projection ', pred_proj)

            gt_proj = pinhole_proj(joint_gt_reconstruct)
            print('gt projection ', gt_proj)

            pred_proj_tsr = torch.tensor(pred_proj, requires_grad = True)
        
            gt_proj_tsr = torch.tensor(gt_proj, requires_grad = True)
                    

            #projection ske map (comp) with integral 
            # from comp bone 3d -> 2d render
            bone_img_arr_comp = np.zeros((batch_size, img_size, img_size, 3), dtype=np.float32)
            bone_img_arr_per_bone_comp = np.zeros((batch_size, img_size, img_size, len(myske)), dtype=np.float32)
            
            bone_img_arr_oval_comp = np.zeros((batch_size, img_size, img_size, 3), dtype=np.float32)
            bone_img_arr_per_bone_oval_comp = np.zeros((batch_size, img_size, img_size, len(myske)), dtype=np.float32)
            
            use_bone_img = True
            use_oval = True
            if use_bone_img:
                for i in range(batch_size):
                    coord_out = pred_proj[i] * float(img_size) #[0, 1] * img_size
                    bone_img_comp = np.zeros((img_size, img_size, 3), dtype=np.float32)
                    bone_img_oval_comp = np.zeros((img_size, img_size, 3), dtype=np.float32)
                    for bone in range(len(myske)):
                        i1 = myske[bone][0]
                        i2 = myske[bone][1]
                        b = train_dataset.ske_colors[bone][0]
                        g = train_dataset.ske_colors[bone][1]
                        r = train_dataset.ske_colors[bone][2]
                        x1 = coord_out[i1 * 2 + 0] / float(img_size)
                        y1 = coord_out[i1 * 2 + 1] / float(img_size)
                        x2 = coord_out[i2 * 2 + 0] / float(img_size)
                        y2 = coord_out[i2 * 2 + 1] / float(img_size)
                        minx = min(x1, x2) 
                        maxx = max(x1, x2) 
                        miny = min(y1, y2) 
                        maxy = max(y1, y2) 

                        minxx = min(x1, x2) - 0.02
                        maxxx = max(x1, x2) + 0.02
                        minyy = min(y1, y2) - 0.02
                        maxyy = max(y1, y2) + 0.02
                        blank_image_comp = np.zeros((img_size,img_size,1), np.uint8)
                        blank_image_oval_comp = np.zeros((img_size,img_size,1), np.uint8)
                        
                        max_b = 0.5 * math.sqrt(math.pow(x1 - x2, 2) + math.pow(y1 - y2, 2))
                        
                        if use_oval:
                            for col in range(0, render_size):
                                    x = (col + 0.5) / float(render_size);
                                    if (math.fabs(x - minxx) < eps or math.fabs(x - maxxx) < eps or (x - minxx > eps and maxxx - x > eps)):
                                        for row in range(0, render_size):
                                            y = (row + 0.5) / float(render_size)
                                            if (math.fabs(y - minyy) < eps or math.fabs(y - maxyy) < eps or (y - minyy > eps and maxyy - y > eps)):                           
                                    
                                                dist_2_line = math.fabs((y2 - y1) * x - (x2 - x1) * y + x2 * y1 - y2 * x1) / math.sqrt(math.pow(y2 - y1, 2) + math.pow(x2 - x1, 2) + 0.0000000000000000001)
                                                dist_minor_axis = math.pow(x - (x1 + x2) / 2.0, 2) + math.pow(y - (y1 + y2) / 2.0, 2) - math.pow(dist_2_line, 2)
                                                dist_minor_axis = max(0.0, dist_minor_axis)
                                                dist_minor_axis = math.sqrt(dist_minor_axis)

                                                dist = math.pow(dist_2_line, 2) / math.pow(max_a, 2) + math.pow(dist_minor_axis, 2) / math.pow(max_b, 2)
                                                if 1.0 - dist > 1e-6:
                                                    blank_image_oval_comp[row][col][0] = 1.0 * 255
                                                    bone_img_oval_comp[row][col][0] += blank_image_oval_comp[row][col][0] / 255.0 * b / 255.0
                                                    bone_img_oval_comp[row][col][1] += blank_image_oval_comp[row][col][0] / 255.0 * g / 255.0
                                                    bone_img_oval_comp[row][col][2] += blank_image_oval_comp[row][col][0] / 255.0 * r / 255.0
                                                    bone_img_arr_oval_comp[i][row][col][0] += bone_img_oval_comp[row][col][0]          
                                                    bone_img_arr_oval_comp[i][row][col][1] += bone_img_oval_comp[row][col][1]          
                                                    bone_img_arr_oval_comp[i][row][col][2] += bone_img_oval_comp[row][col][2] 
                                                
                                                bone_img_arr_per_bone_oval_comp[i][row][col][bone] = (blank_image_oval_comp[row][col][0] + blank_image_oval_comp[row][col][0] + blank_image_oval_comp[row][col][0]) / 3.0 / 255.0 
                        
                        if (math.fabs(x1 - x2) < eps):
                            for col in range(0, render_size):
                                x = (col + 0.5) / float(render_size)
                                for row in range(0, render_size):
                                    y = (row + 0.5) / float(render_size)
                                    if (math.fabs(y - miny) < eps or math.fabs(y - maxy) < eps or (y - miny > eps and maxy - y > eps)):
                                        dsquare1 = -1.0 / (2.0 * sigma_0 * sigma_0) * math.pow(x - x1, 2)
                                        dsquare2 = -1.0 / (2.0 * sigma_1 * sigma_1) * math.pow((y - y1) / (y2 - y1), 2)
                                        blank_image_comp[row][col][0] = math.exp(dsquare1) * math.exp(dsquare2) * 255
                                        bone_img_comp[row][col][0] += blank_image_comp[row][col][0] / 255.0 * b / 255.0
                                        bone_img_comp[row][col][1] += blank_image_comp[row][col][0] / 255.0 * g / 255.0
                                        bone_img_comp[row][col][2] += blank_image_comp[row][col][0] / 255.0 * r / 255.0
                                        bone_img_arr_comp[i][row][col][0] += bone_img_comp[row][col][0]          
                                        bone_img_arr_comp[i][row][col][1] += bone_img_comp[row][col][1]          
                                        bone_img_arr_comp[i][row][col][2] += bone_img_comp[row][col][2] 
                                    bone_img_arr_per_bone_comp[i][row][col][bone] = (blank_image_comp[row][col][0] + blank_image_comp[row][col][0] + blank_image_comp[row][col][0]) / 3.0 / 255.0       
                                  
                        elif (math.fabs(y1 - y2) < eps):
                            for row in range(0, render_size):
                                y = (row + 0.5) / float(render_size)
                                for col in range(0, render_size):
                                    x = (col + 0.5) / float(render_size)
                                    if (math.fabs(x - minx) < eps or math.fabs(x - maxx) < eps or (x - minx > eps and maxx - x > eps)):
                                        dsquare1 = -1.0 / (2.0 * sigma_0 * sigma_0) * math.pow(y - y1, 2)
                                        dsquare2 = -1.0 / (2.0 * sigma_1 * sigma_1) * math.pow((x - x1) / (x2 - x1), 2)
                                        blank_image_comp[row][col][0] = math.exp(dsquare1) * math.exp(dsquare2) * 255
                                        bone_img_comp[row][col][0] += blank_image_comp[row][col][0] / 255.0 * b / 255.0
                                        bone_img_comp[row][col][1] += blank_image_comp[row][col][0] / 255.0 * g / 255.0
                                        bone_img_comp[row][col][2] += blank_image_comp[row][col][0] / 255.0 * r / 255.0
                                        bone_img_arr_comp[i][row][col][0] += bone_img_comp[row][col][0]          
                                        bone_img_arr_comp[i][row][col][1] += bone_img_comp[row][col][1]          
                                        bone_img_arr_comp[i][row][col][2] += bone_img_comp[row][col][2] 
                                    bone_img_arr_per_bone_comp[i][row][col][bone] = (blank_image_comp[row][col][0] + blank_image_comp[row][col][0] + blank_image_comp[row][col][0]) / 3.0 / 255.0       
                        else:
                            A = (y2 - y1) / (x2 - x1)
                            B = -1.0
                            C = y1 - A * x1
                            for col in range(0, render_size):
                                x = (col + 0.5) / float(render_size);
                                if (math.fabs(x - minx) < eps or math.fabs(x - maxx) < eps or (x - minx > eps and maxx - x > eps)):
                                    for row in range(0, render_size):
                                        y = (row + 0.5) / float(render_size)
                                        if (math.fabs(y - miny) < eps or math.fabs(y - maxy) < eps or (y - miny > eps and maxy - y > eps)):
                                            dsquare0 = -1.0 / (2.0 * sigma_0 * sigma_0) * math.pow(A * x + B * y + C, 2) / (math.pow(A, 2) + math.pow(B, 2))
                                            dsquare1 = -1.0 / (2.0 * sigma_1 * sigma_1) * math.pow((x - x1) / (x2 - x1), 2)
                                            dsquare2 = -1.0 / (2.0 * sigma_1 * sigma_1) * math.pow((y - y1) / (y2 - y1), 2)
                                            blank_image_comp[row][col][0] = math.exp(dsquare0) * math.exp(dsquare1) * math.exp(dsquare2) * 255
                                            bone_img_comp[row][col][0] += blank_image_comp[row][col][0] / 255.0 * b / 255.0
                                            bone_img_comp[row][col][1] += blank_image_comp[row][col][0] / 255.0 * g / 255.0
                                            bone_img_comp[row][col][2] += blank_image_comp[row][col][0] / 255.0 * r / 255.0
                        
                                            bone_img_arr_comp[i][row][col][0] += bone_img_comp[row][col][0]          
                                            bone_img_arr_comp[i][row][col][1] += bone_img_comp[row][col][1]          
                                            bone_img_arr_comp[i][row][col][2] += bone_img_comp[row][col][2]        
                                        
                                        bone_img_arr_per_bone_comp[i][row][col][bone] = (blank_image_comp[row][col][0] + blank_image_comp[row][col][0] + blank_image_comp[row][col][0]) / 3.0 / 255.0 
                    cv2.imshow("blank_comp", bone_img_arr_comp[i])
                    cv2.waitKey(1)
                    if use_oval:
                        cv2.imshow("blank_oval_comp", bone_img_arr_oval_comp[i])
                        cv2.waitKey(1)
                        
                # Bone img concatenate style -> deconv a stylized image
                # [0, 1] -> std mean normalize it
                pre_transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=pixel_mean, std=pixel_std)])
                bone_img_arr_comp = bone_img_arr_comp.transpose(0,3,1,2)
                if use_oval:
                    bone_img_arr_oval_comp = bone_img_arr_oval_comp.transpose(0,3,1,2)
                
            # the original gt ske (old-fashioned)
            # 7. Generate ground truth skeleton
            ske_img_arr_comp = np.zeros((batch_size, 3, img_size, img_size), dtype=np.float32)
            
            for i in range(batch_size):
                coord_out = pred_proj[i] * float(img_size) #[0, 1] * img_size
                pred_ske_comp = np.zeros((img_size, img_size, 3),dtype=np.uint8)
            
                for j in range(len(myske)):
                    i1 = myske[j][0]
                    i2 = myske[j][1]
                    
                    x1 = coord_out[i1 * 2 + 0]
                    y1 = coord_out[i1 * 2 + 1]
                    x2 = coord_out[i2 * 2 + 0]
                    y2 = coord_out[i2 * 2 + 1]
                
                    if joint_vis[i][i1] == 1 and joint_vis[i][i2] == 1:                        
                        cv2.line(
                        pred_ske_comp, (x1.astype(np.int32), y1.astype(np.int32)), (x2.astype(np.int32), y2.astype(np.int32)),
                        color=train_dataset.ske_colors[j], thickness=3, lineType=cv2.LINE_AA)                        
                pred_ske_comp_ = cv2.resize(pred_ske_comp, (512, 512))                    
                pred_ske_comp = pred_ske_comp.transpose((2, 0, 1))
                pred_ske_out_comp = np.zeros((3, img_size, img_size),dtype=np.float32)
                pred_ske_out_comp[:, :, :] = pred_ske_comp[:, :, :] / 255.0
                ske_img_arr_comp[i] = pred_ske_out_comp

            #transform
            if use_bone_img:
                for i in range(batch_size):
                    cur_img_comp = bone_img_arr_comp[i].transpose(1,2,0)
                    bone_img_arr_comp[i] = pre_transforms(cur_img_comp)

                    if use_oval:
                        cur_img_oval_comp = bone_img_arr_oval_comp[i].transpose(1,2,0)
                        bone_img_arr_oval_comp[i] = pre_transforms(cur_img_oval_comp)

                bone_img_arr_comp = torch.from_numpy(bone_img_arr_comp).cuda()
                if use_oval:
                    bone_img_arr_oval_comp = torch.from_numpy(bone_img_arr_oval_comp).cuda()

                bone_img_arr_per_bone_comp = bone_img_arr_per_bone_comp.transpose(0,3,1,2)
                bone_img_arr_per_bone_comp = torch.from_numpy(bone_img_arr_per_bone_comp).cuda()

                if use_oval:
                    bone_img_arr_per_bone_oval_comp = bone_img_arr_per_bone_oval_comp.transpose(0,3,1,2)
                    bone_img_arr_per_bone_oval_comp = torch.from_numpy(bone_img_arr_per_bone_oval_comp).cuda()

            ske_img_arr_comp = torch.from_numpy(ske_img_arr_comp).cuda()
            
            cur_ = 0
            for i in range(batch_size):
                if data_source[i] == MYMPII:
                    if cur_ == 0:
                        loss_bone_img_arr_per_bone_oval = nn.SmoothL1Loss()                    
                        loss_bone_img_arr_per_bone_oval = loss_bone_img_arr_per_bone_oval(bone_img_arr_per_bone_oval[i], bone_img_arr_per_bone_oval_comp[i])

                        loss_bone_img_arr_per_bone = nn.SmoothL1Loss()                    
                        loss_bone_img_arr_per_bone = loss_bone_img_arr_per_bone(bone_img_arr_per_bone[i], bone_img_arr_per_bone_comp[i])

                        loss_bone_img_arr_oval = nn.SmoothL1Loss()                    
                        loss_bone_img_arr_oval = loss_bone_img_arr_oval(bone_img_arr_oval[i], bone_img_arr_oval_comp[i])

                        loss_bone_img_arr = nn.SmoothL1Loss()                    
                        loss_bone_img_arr = loss_bone_img_arr(bone_img_arr[i], bone_img_arr_comp[i])

                        loss_ske_img_arr = nn.SmoothL1Loss()
                        loss_ske_img_arr = loss_ske_img_arr(ske_img_arr[i], ske_img_arr_comp[i])

                        ske_map_proj_loss = nn.SmoothL1Loss()
                        ske_map_proj_loss = ske_map_proj_loss(pred_proj_tsr[i], gt_proj_tsr[i])
                        
                    else:
                        cur_loss_bone_img_arr_per_bone_oval = nn.SmoothL1Loss()
                        cur_loss_bone_img_arr_per_bone_oval = cur_loss_bone_img_arr_per_bone_oval(bone_img_arr_per_bone_oval[i], bone_img_arr_per_bone_oval_comp[i])
                        loss_bone_img_arr_per_bone_oval += cur_loss_bone_img_arr_per_bone_oval

                        cur_loss_bone_img_arr_per_bone = nn.SmoothL1Loss()  
                        cur_loss_bone_img_arr_per_bone = cur_loss_bone_img_arr_per_bone(bone_img_arr_per_bone[i], bone_img_arr_per_bone_comp[i])
                        loss_bone_img_arr_per_bone += cur_loss_bone_img_arr_per_bone

                        cur_loss_bone_img_arr_oval = nn.SmoothL1Loss() 
                        cur_loss_bone_img_arr_oval = cur_loss_bone_img_arr_oval(bone_img_arr_oval[i], bone_img_arr_oval_comp[i])
                        loss_bone_img_arr_oval += cur_loss_bone_img_arr_oval

                        cur_loss_bone_img_arr = nn.SmoothL1Loss() 
                        cur_loss_bone_img_arr = cur_loss_bone_img_arr(bone_img_arr[i], bone_img_arr_comp[i])
                        loss_bone_img_arr += cur_loss_bone_img_arr

                        cur_loss_ske_img_arr = nn.SmoothL1Loss()
                        cur_loss_ske_img_arr = cur_loss_ske_img_arr(ske_img_arr[i], ske_img_arr_comp[i])
                        loss_ske_img_arr += cur_loss_ske_img_arr

                        cur_ske_map_proj_loss = nn.SmoothL1Loss()
                        cur_ske_map_proj_loss = cur_ske_map_proj_loss(pred_proj_tsr[i], gt_proj_tsr[i])
                        ske_map_proj_loss += cur_ske_map_proj_loss

                    cur_ += 1 
            
            need_bone_img = True
            if cur_ == 0:
                
                need_bone_img = False
            else:
                
                loss_bone_img_arr_per_bone_oval = loss_bone_img_arr_per_bone_oval.mean()
                loss_bone_img_arr_per_bone = loss_bone_img_arr_per_bone.mean()
                loss_bone_img_arr_oval = loss_bone_img_arr_oval.mean()
                loss_bone_img_arr = loss_bone_img_arr.mean()
            
            #visualize in 2d
            for t in range(batch_size):
                cur_ske_img = t_ske_img_arr[t]# * 255.0
                cur_ske_img = cur_ske_img.transpose((1, 2, 0))            

                opt_img = opt.data[t].cpu().numpy().transpose(1, 2, 0)
                post_transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=(0, 0, 0), std=pixel_std_neg), transforms.Normalize(mean=pixel_mean_neg, std=(1,1,1))])
                opt_img = post_transforms(opt_img)
                opt_img[opt_img > 1] = 1
                opt_img[opt_img < 0] = 0
                postpb = transforms.Compose([transforms.Lambda(lambda x: x[torch.LongTensor([2,1,0])]), transforms.ToPILImage()])
                opt_img = postpb(opt_img)
                open_cv_image = np.array(opt_img) 
                
                for j in range(len(myske)):
                    i1 = myske[j][0]
                    i2 = myske[j][1]
                    
                    x1 = pred_proj[t][i1 * 2 + 0] * img_size
                    y1 = pred_proj[t][i1 * 2 + 1] * img_size
                    x2 = pred_proj[t][i2 * 2 + 0] * img_size
                    y2 = pred_proj[t][i2 * 2 + 1] * img_size
                    
                    if joint_vis[t][i1] == 1 and joint_vis[t][i2] == 1:
                        cv2.line(
                        open_cv_image, (x1.astype(np.int32), y1.astype(np.int32)), (x2.astype(np.int32), y2.astype(np.int32)),
                        color=train_dataset.ske_colors[j], thickness=3, lineType=cv2.LINE_AA)
                        #print('connecting ', j, 'done')
                opt_img_ = cv2.resize(open_cv_image, (224, 224))
                    
                cv2.imshow("comp_proj_2d_on_opt", opt_img_)
                cv2.waitKey(1)

            #neg thorax loss
            cur_ = 0
            neg_thorax_pred = torch.tensor(neg_thorax_pred, requires_grad = True)
            neg_thorax = torch.tensor(neg_thorax, requires_grad = False)
            norm_bone_pred = torch.tensor(norm_bone_pred, requires_grad = True)
            norm_bone = torch.tensor(norm_bone, requires_grad = False)
            for i in range(batch_size):
                if data_source[i] == MYMPII:
                    if cur_ == 0:
                        neg_thorax_reg_loss = nn.MSELoss()
                        print('pred : ', neg_thorax_pred[i])
                        print('gt: ', neg_thorax[i].shape)
                        neg_thorax_reg_loss = neg_thorax_reg_loss(neg_thorax_pred[i], neg_thorax[i])
                    else:
                        cur_neg_thorax_reg_loss = nn.MSELoss()
                        cur_neg_thorax_reg_loss = cur_neg_thorax_reg_loss(neg_thorax_pred[i], neg_thorax[i])
                        neg_thorax_reg_loss += cur_neg_thorax_reg_loss
                    cur_ += 1 
            need_neg_thorax_loss = True
            if cur_ == 0:
                need_neg_thorax_loss = False
            else:
                neg_thorax_reg_loss = neg_thorax_reg_loss.mean()
            
            #norm bone loss
            cur_ = 0
            for i in range(batch_size):
                if data_source[i] == MYMPII:
                    if cur_ == 0:
                        norm_bone_reg_loss = nn.MSELoss()
                        norm_bone_reg_loss = norm_bone_reg_loss(norm_bone_pred[i], norm_bone[i])
                    else:
                        cur_norm_bone_reg_loss = nn.MSELoss()
                        cur_norm_bone_reg_loss = cur_norm_bone_reg_loss(norm_bone_pred[i], norm_bone[i])
                        norm_bone_reg_loss += cur_norm_bone_reg_loss
                    cur_ += 1 
            need_norm_bone_loss = True 
            if cur_ == 0:
                need_norm_bone_loss = False
            else:
                norm_bone_reg_loss = norm_bone_reg_loss.mean()
            
            opt_self_sup = model_self_sup_deconv([style, bone_img_arr_per_bone_oval, bone_img_arr_per_bone, bone_img_arr_oval, bone_img_arr, ske_img_arr])
            
            out_self_sup = vgg(opt_self_sup, loss_layers_self_sup)
            
            # SELF SUP STYLE LOSSES
            layer_losses_self_sup = [weights_self_sup[a] * loss_fns_self_sup[a](A, targets_self_sup[a]) for a,A in enumerate(out_self_sup)]
            
            ll_self_sup_cos = nn.CosineSimilarity(dim=1, eps=1e-6)
            ll_self_sup_cos_loss = ll_self_sup_cos(opt_self_sup, opt)
            ll_self_sup_cos_loss = ll_self_sup_cos_loss.mean()
            
            print('ll_self_sup_cos ', ll_self_sup_cos_loss)
            
            diff_self_sup_i = torch.sum(torch.abs(opt_self_sup[:, :, :, 1:] - opt_self_sup[:, :, :, :-1]))
            diff_self_sup_j = torch.sum(torch.abs(opt_self_sup[:, :, 1:, :] - opt_self_sup[:, :, :-1, :]))
            tv_self_sup_loss = TV_WEIGHT*(diff_self_sup_i + diff_self_sup_j)
            
            nst_self_sup_loss = 1 * (ll_self_sup_cos_loss) + sum(layer_losses_self_sup)

            opt_self_sup_style_out = [A.detach() for A in vgg(opt_self_sup, style_ent_layers)]
            opt_self_sup_content_out = [A.detach() for A in vgg(opt_self_sup, content_ent_layers)]
            
            # ====== Visualize the stylized from self sup (integral 2d ske) and style
            #i = 0
            for i in range(batch_size):
                img = opt_self_sup.data[i].cpu().numpy().transpose(1, 2, 0)
                post_transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=(0, 0, 0), std=pixel_std_neg), transforms.Normalize(mean=pixel_mean_neg, std=(1,1,1))])
                img = post_transforms(img)
                img[img > 1] = 1
                img[img < 0] = 0
                postpb = transforms.Compose([transforms.Lambda(lambda x: x[torch.LongTensor([2,1,0])]), transforms.ToPILImage()])
                img = postpb(img)
                open_cv_image = np.array(img) 
                open_cv_image = open_cv_image[:, :, ::-1].copy() 
                cv2.imshow("opt_self_sup", open_cv_image)
                cv2.waitKey(1)
            
            # Output nst result
            for i in range(batch_size):               
                img = opt.data[i].cpu().numpy().transpose(1, 2, 0)
                post_transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=(0, 0, 0), std=pixel_std_neg), transforms.Normalize(mean=pixel_mean_neg, std=(1,1,1))])
                img = post_transforms(img)               
                img[img > 1] = 1
                img[img < 0] = 0
                postpb = transforms.Compose([transforms.Lambda(lambda x: x[torch.LongTensor([2,1,0])]), transforms.ToPILImage()])
                img = postpb(img)                
                open_cv_image = np.array(img) 
                # Convert RGB to BGR                
                if cnt % show_cnt == 0:
                    cv2.imshow("opt", open_cv_image)
                    cv2.waitKey(1)

                    vis_2d = True
                    #overlay 2d prediction on the optimized neural style image
                    if vis_2d:
                        save_img_on_the_fly = True
                        if save_img_on_the_fly:
                            ###save content

                            img_content_name = "D:/results/" + folder_prefix + "/content/" + str(tot) + ".png"
                            tot += 1
                            #content image
                            content_img = content.data[i].cpu().numpy().transpose(1, 2, 0)
                            post_transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=(0, 0, 0), std=pixel_std_neg), transforms.Normalize(mean=pixel_mean_neg, std=(1,1,1))])
                            content_img = post_transforms(content_img)                            
                            content_img[content_img > 1] = 1
                            content_img[content_img < 0] = 0
                            postpb = transforms.Compose([transforms.Lambda(lambda x: x[torch.LongTensor([2,1,0])]), transforms.ToPILImage()])
                            content_img = postpb(content_img)
                            open_cv_image = np.array(content_img) 
                            # Convert RGB to BGR 
                
                            content_img = open_cv_image.astype(np.uint8)
                            
                            ###save style
                            img_style_name = "D:/results/" + folder_prefix + "/style/" + str(tot - 1) + ".png"
                            #style image
                            style_img = style.data[i].cpu().numpy().transpose(1, 2, 0)
                            post_transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=(0, 0, 0), std=pixel_std_neg), transforms.Normalize(mean=pixel_mean_neg, std=(1,1,1))])
                            style_img = post_transforms(style_img)
                            style_img[style_img > 1] = 1
                            style_img[style_img < 0] = 0
                            postpb = transforms.Compose([transforms.Lambda(lambda x: x[torch.LongTensor([2,1,0])]), transforms.ToPILImage()])
                            style_img = postpb(style_img)
                            open_cv_image = np.array(style_img) 
                            style_img = open_cv_image.astype(np.uint8)
                            
                            ###save opt
                            img_opt_name = "D:/results/" + folder_prefix + "/opt/" + str(tot - 1) + ".png"
                            #opt image
                            opt_img = opt.data[i].cpu().numpy().transpose(1, 2, 0)
                            post_transforms = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean=(0, 0, 0), std=pixel_std_neg), transforms.Normalize(mean=pixel_mean_neg, std=(1,1,1))])
                            opt_img = post_transforms(opt_img)
                            opt_img[opt_img > 1] = 1
                            opt_img[opt_img < 0] = 0
                            postpb = transforms.Compose([transforms.Lambda(lambda x: x[torch.LongTensor([2,1,0])]), transforms.ToPILImage()])
                            opt_img = postpb(opt_img)
                            open_cv_image = np.array(opt_img) 
                            opt_img = open_cv_image.astype(np.uint8)
                            
                            if (tot - 1) % on_the_fly_mod == 0:
                                cv2.imwrite(img_content_name, content_img)
                                cv2.imwrite(img_style_name, style_img)
                                cv2.imwrite(img_opt_name, opt_img)


                        coord_out = soft_argmax(heatmap_out[i], global_joint_num).data.cpu().numpy()[0]
                        coord_out = coord_out * 4.0
                        
                        tmpimg = open_cv_image.copy().astype(np.uint8)
                        tmpkps = np.zeros((global_joint_num,3))
                        print(coord_out.shape)
                        tmpkps[:,0], tmpkps[:,1] = coord_out[:,0], coord_out[:,1]
                        tmpkps[:,2] = 1
                        print(tmpkps.shape)
                        
                        tmpimg = vis_keypoints(tmpimg, tmpkps, myske)
                        cv2.imshow("opt", tmpimg)
                        cv2.waitKey(1)

                        vis_3d = False
                        if vis_3d:

                            # back project to camera coordinate system
                            pre_3d_kpt = np.zeros((global_joint_num,3))
                            gt_vis = np.ones((global_joint_num, 1))
                            pre_3d_kpt[:,:] = coord_out[:,:] 
                            if train_dataset.multiple_db == True:
                                this_ske = train_dataset.skeleton[0]
                            else:
                                this_ske = train_dataset.skeleton
                            filename = 'This is {:4d} data source is {}'.format(i, data_source_str[int(data_source[i])])
                            
                            
                            if data_source[i] == MYMPII:
                                f = 300.0
                                root_z = 2.0#gt_joint_3d[i,8,2] #e.g. 2.5
                                rel_z = (pre_3d_kpt[:,2] / depth_dim) * 2.0 - 1.0
                                pre_3d_kpt[:,2] = root_z + rel_z #e.g. 2.56
                                pre_3d_kpt[:,0:2] = pre_3d_kpt[:,0:2] / float(output_shape) #[0.0, 1.0]
                                pre_3d_kpt[:,0] = (pre_3d_kpt[:,0] - 0.5) * pre_3d_kpt[:,2] / f 
                                pre_3d_kpt[:,1] = -(pre_3d_kpt[:,1] - 0.5) * pre_3d_kpt[:,2] / f 
                                print(pre_3d_kpt)
                                vis_3d_skeleton(pre_3d_kpt, gt_vis, this_ske, filename)
                                
                      
                break

            cnt += 1 #Add one iteration

            ske_loss = nn.BCEWithLogitsLoss()
            ske_loss = DataParallelCriterion(ske_loss)
            ske_loss = ske_loss(res3c_deconv, gt_ske)

            open_cv_image = (np.array(res3c_deconv.data.cpu().numpy()))[0].transpose(1, 2, 0)
            print(res3c_deconv.data.cpu().numpy().shape)
            # Convert RGB to BGR 
            open_cv_image = cv2.resize(open_cv_image, (128, 128))
            cv2.imshow("dev", open_cv_image)
            cv2.waitKey(1)
            # backward
            jnt_loss = JointLocationLoss()
            jnt_loss = jnt_loss(heatmap_out, joint_img, joint_vis, joints_have_depth)
            coord_out = soft_argmax(heatmap_out, global_joint_num)
            
            alpha = 1#1.0000000000# * nst_loss.item() / jnt_loss.item()
            ro = 100.0000000
            zeta = 1000.0000
            beta = 0 #* nst_loss.item() / ske_loss.item()
            gamma = 1 #* jnt_loss.item() / nst_loss.item()
            delta = 1000000000# * nst_loss.item() / style_ent_loss.item()
            delta_selfsup = 1
            mu =   1000000# * nst_loss.item() / content_ent_loss.item()
            mu_selfsup = 0
            theta = 0# * jnt_loss.item() / mapping_cylinder_layer_loss.item()
            sylla = 300 # hsv
            bear = 200 #srgb
            nta = 0.0035 #self sup
            print('cos :                       ', cos_loss)
            print('nst :                       ', nst_loss)
            print('nst self sup:               ', nst_self_sup_loss)
            print('entropy (style):            ', style_ent_loss)
            print('entropy (content):          ', content_ent_loss)
            
            print('hsv loss:                   ', loss_hsv)
            print('srgb loss:                  ', srgb_loss)
            print('jnt loss:                   ', jnt_loss)
            if need_neg_thorax_loss:
                print('neg thorax loss:            ', neg_thorax_reg_loss)
            if need_norm_bone_loss:
                print('norm bone loss:             ', norm_bone_reg_loss)
            loss = bear * srgb_loss + mu * content_ent_loss + nta * nst_self_sup_loss + sylla * loss_hsv + delta * style_ent_loss + gamma * nst_loss + alpha * jnt_loss + beta * ske_loss
            if need_norm_bone_loss:
                loss += zeta * norm_bone_reg_loss
            if need_neg_thorax_loss:
                loss += ro * neg_thorax_reg_loss
            
            Rho = 10000.0
            if need_bone_img:
                loss += Rho * loss_bone_img_arr_per_bone_oval
                loss += Rho * loss_bone_img_arr_per_bone
                loss += Rho * loss_bone_img_arr_oval 
                loss += Rho * loss_bone_img_arr 
                loss += Rho * loss_ske_img_arr
                loss += Rho * ske_map_proj_loss
                # comp bone 3d proj from (integral) ske map
                #    compare with gt 2d
                print('loss_bone_img_arr_per_bone_oval: ', loss_bone_img_arr_per_bone_oval)
                print('loss_bone_img_arr_per_bone       ', loss_bone_img_arr_per_bone)
                print('loss_bone_img_arr_oval           ', loss_bone_img_arr_oval)
                print('loss_bone_img_arr                ', loss_bone_img_arr)
                print('loss_ske_img_arr                 ', loss_ske_img_arr)
                print('loss_ske_map_proj                ', ske_map_proj_loss)

            
            match_style_loss = layer_losses_style_cos[0].mean()
            print("match style losses  {:12.2f}".format(layer_losses_style_cos[0].mean()))
            loss = loss + 10.0 * match_style_loss
            loss.backward(retain_graph=True)

            
            cur_epoch_avg_loss += loss.item()
            optimizer.step()
            print('Optimization done')
            
            ### Bug
            ## Constantly save in case of data loss
            #print('cnt - 1', cnt - 1)
            if ((cnt - 1) * batch_size) % per_iter_save_num == 0:
                model_file = 'resnet50/model256_mpii_ist_' + str(epoch) + '_' + str(((cnt - 1) * batch_size) // per_iter_save_num) + '.pth'
                torch.save(model.state_dict(), model_file)
                model_2dske_file = 'resnet50/model256_mpii_ist_2dske_to_pose_' + str(epoch) + '_' + str(((cnt - 1) * batch_size) // per_iter_save_num) + '.pth'
                torch.save(model_2dske.state_dict(), model_2dske_file)
            
                model_deconv_file = 'resnet50/model256_mpii_ist_deconv_' + str(epoch) + '_' + str(((cnt - 1) * batch_size) // per_iter_save_num) + '.pth'
                torch.save(model_deconv.state_dict(), model_deconv_file)
                model_self_sup_deconv_file = 'resnet50/model256_mpii_ist_self_sup_deconv_' + str(epoch) + '_' + str(((cnt - 1) * batch_size) // per_iter_save_num) + '.pth'
                torch.save(model_self_sup_deconv.state_dict(), model_self_sup_deconv_file)
                
            if batch_idx % log_interval == 0:
                print('Train Epoch: {} [{}/{} ({:.0f}%)]\tJnt Loss: {:.6f} Jnt 2d ske Loss: {:.6f}'.format(
                    epoch, batch_size * cnt, itr_per_epoch * batch_size,
                    100. * batch_idx / itr_per_epoch, jnt_loss.detach(), 0.0))
            logger.info('Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}'.format(
                    epoch, batch_size * cnt, itr_per_epoch * batch_size,
                    100. * batch_idx / itr_per_epoch, jnt_loss.detach()))
            #Add loss of this batch
            train_loss_logger.info('{}\n'.format(loss.item()))
            train_loss_arr.append(loss.item())
    cur_epoch_avg_loss /= float(cnt)
    train_loss_epoch_arr.append(cur_epoch_avg_loss)      

def main():
    for epoch in range(st_epoch, epochs + 1):
        model_file = 'resnet50/model256_mpii_ist_' + str(epoch) + '.pth'
        #print(model)
        train(epoch)
        if epoch % 1 == 0:
            #Also save deconv part
            model_file = 'resnet50/model256_mpii_ist_' + str(epoch) + '_final.pth'
            torch.save(model.state_dict(), model_file)
            model_2dske_file = 'resnet50/model256_mpii_ist_2dske_to_pose_' + str(epoch) + '_final.pth'
            torch.save(model_2dske.state_dict(), model_2dske_file)
            
            model_deconv_file = 'resnet50/model256_mpii_ist_deconv_' + str(epoch) + '_final.pth'
            torch.save(model_deconv.state_dict(), model_deconv_file)
            model_self_sup_deconv_file = 'resnet50/model256_mpii_ist_self_sup_deconv_' + str(epoch) + '_final.pth'
            torch.save(model_self_sup_deconv.state_dict(), model_self_sup_deconv_file)
                    

        print('\nSaved model to ' + model_file + '.')
        logger.info("Write snapshot into {}\n".format(model_file))
    
if __name__ == "__main__":
    main()
