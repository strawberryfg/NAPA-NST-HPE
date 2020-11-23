import torch
import torch.nn as nn
import torch.nn.functional as F
import torchvision.models as models
from collections import namedtuple

class ResidualBlock(nn.Module):
  def __init__(self):
    super(ResidualBlock, self).__init__()
    self.conv1 = PaddedConv(128, 128, 3, stride = 1)
    self.in1 = nn.InstanceNorm2d(128, affine=True)
    self.conv2 = PaddedConv(128, 128, 3, stride = 1)
    self.in2 = nn.BatchNorm2d(128, affine=True)
    self.relu = torch.nn.ReLU()
  
  def forward(self, x):
    res = x
    out = self.relu(self.in1(self.conv1(x)))
    out = self.in2(self.conv2(out))
    out = out + res
    return out

class ImageTransformationNetwork(nn.Module):
  def __init__(self):
    super(ImageTransformationNetwork, self).__init__()
    self.conv1 = PaddedConv(3, 32, 9, stride = 1)
    self.bn1 = nn.InstanceNorm2d(32, affine=True)
    self.conv2 = PaddedConv(32, 64, 3, stride = 2)
    self.bn2 = nn.InstanceNorm2d(64, affine=True)
    self.conv3 = PaddedConv(64, 128, 3, stride = 2)
    self.bn3 = nn.InstanceNorm2d(128, affine=True)
    self.Res1 = ResidualBlock()
    self.Res2 = ResidualBlock()
    self.Res3 = ResidualBlock()
    self.Res4 = ResidualBlock()
    self.Res5 = ResidualBlock()
    self.conv4 = UpsamplePaddedConv(128, 64, 3, stride = 1, upsample=2)
    self.bn4 = nn.InstanceNorm2d(64, affine=True)
    self.conv5 = UpsamplePaddedConv(64, 32, 3, stride = 1, upsample=2)
    self.bn5 = nn.InstanceNorm2d(32, affine=True)
    self.conv6 = PaddedConv(32, 3, 9, stride = 1)

  def forward(self, x):
    out = F.relu(self.bn1(self.conv1(x)))
    out = F.relu(self.bn2(self.conv2(out)))
    out = F.relu(self.bn3(self.conv3(out)))
    out = self.Res1(out)
    out = self.Res2(out)
    out = self.Res3(out)
    out = self.Res4(out)
    out = self.Res5(out)
    out = F.relu(self.bn4(self.conv4(out)))
    out = F.relu(self.bn5(self.conv5(out)))
    out = self.conv6(out)
    return out
    
class PaddedConv(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size, stride):
        super(PaddedConv, self).__init__()
        reflection_padding = kernel_size // 2
        self.reflection_pad = nn.ReflectionPad2d(reflection_padding)
        self.conv2d = nn.Conv2d(in_channels, out_channels, kernel_size, stride)

    def forward(self, x):
        out = self.reflection_pad(x)
        out = self.conv2d(out)
        return out
        
class UpsamplePaddedConv(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size, stride, upsample=None):
        super(UpsamplePaddedConv, self).__init__()
        self.upsample = upsample
        reflection_padding = kernel_size // 2
        self.reflection_pad = torch.nn.ReflectionPad2d(reflection_padding)
        self.conv2d = torch.nn.Conv2d(in_channels, out_channels, kernel_size, stride)

    def forward(self, x):
        x_in = x
        x_in = torch.nn.functional.interpolate(x_in, mode='nearest', scale_factor=self.upsample)
        out = self.reflection_pad(x_in)
        out = self.conv2d(out)
        return out

    
class LossNetwork(nn.Module):
    def __init__(self, requires_grad=False):
        super(LossNetwork, self).__init__()
        vgg_pretrained_features = models.vgg16(pretrained=True).features
        self.slice1 = torch.nn.Sequential()
        self.slice2 = torch.nn.Sequential()
        self.slice3 = torch.nn.Sequential()
        self.slice4 = torch.nn.Sequential()
        for x in range(4):
            self.slice1.add_module(str(x), vgg_pretrained_features[x])
        for x in range(4, 9):
            self.slice2.add_module(str(x), vgg_pretrained_features[x])
        for x in range(9, 16):
            self.slice3.add_module(str(x), vgg_pretrained_features[x])
        for x in range(16, 23):
            self.slice4.add_module(str(x), vgg_pretrained_features[x])
        if not requires_grad:
            for param in self.parameters():
                param.requires_grad = False

    def forward(self, X):
        h = self.slice1(X)
        h_relu1_2 = h
        h = self.slice2(h)
        h_relu2_2 = h
        h = self.slice3(h)
        h_relu3_3 = h
        h = self.slice4(h)
        h_relu4_3 = h
        vgg_outputs = namedtuple("VggOutputs", ['relu1_2', 'relu2_2', 'relu3_3', 'relu4_3'])
        out = vgg_outputs(h_relu1_2, h_relu2_2, h_relu3_3, h_relu4_3)
        return out