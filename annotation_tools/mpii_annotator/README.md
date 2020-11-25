# MPII Annotator

## Report

A detailed report is in this pdf [2D Human Pose Dataset 3D Joint Location Ground Truth Annotation Tool Guideline](https://github.com/strawberryfg/NAPA-NST-HPE/blob/main/annotation_tools/mpii_annotator/2D%20Human%20Pose%20Dataset%203D%20Joint%20Location%20Ground%20Truth%20Annotation%20Tool%20Guideline.pdf).

## At a glance

Check this video:


<p align="center">  
<img src="../../figs/mpii_annot_opengl.gif">  
</p> 


Go to [this link](https://drive.google.com/file/d/1eOHuXdToHIbm2vnRxY13vhrvOjV4SxIe/view?usp=sharing) for the C# GUI video.

## Screenshots

##### 3D Keypoints Annotation Interface (OpenGL) #####



<p align="center">  
<img src="../../figs/mpii_annot_opengl.png">  
</p> 

##### Parameters Settings Interface (C\#) #####


<p align="center">  
<img src="../../figs/mpii_annot_param0.png">  
</p> 


<p align="center">  
<img src="../../figs/mpii_annot_param1.png">  
</p> 

## Environment

Developed in Visual Studio 2013. Compatible with Visual Studio 2019 (tested).

## Build

Download [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) and place under 

```
  annotation_tools/mpii_annotator/ThreeDGroundTruthLabeler/ThreeDGroundTruthLabeler/eigen/

```

Download tools (*freeglut-2.8.1*, *glew-1.10.0*, *glm-0.9.5.4*, *glm-0.9.5.4*, *opencv-3.0.0*) from [here](https://drive.google.com/drive/folders/15DUHqhOdsQfq6G6zGNMIWkwRGgjRVCNC?usp=sharing) and place under 


```
  annotation_tools/mpii_annotator/ThreeDGroundTruthLabeler/tools/*

```

Download annotation data and images from this [link](https://drive.google.com/drive/folders/1ZNnEt4EQ5SZSTalJdsmUikMOR6ZLCkpS?usp=sharing) and set it as 
*working directory* using the C# interface under *Basic* tab.


```
  Set Working Directory: 

```

Build the c++ project *ThreeDGroundTruthLabeler* using *Release Win32* mode.

Build the C# project *ParameterSetttingUI* using *Release* mode.

## Usage

Essentially it supports 3d annotation with some sort of constraints. There's a C# interface to modify some hyper parameters, and a c++ GUI to do the hefty work.

1. ##### 3d annotation #####

	Used a forward kinematics human model with 2D reprojection error as an intial objective function to obatain a rough estimate of 3D keypoints that match the 2D (given by MPII) ground truth of one frame.
	
	Let the user interact with keyboard and mouse to make minor changes to a specific keypoint, and visualize 3D in OpenGL; 2D in OpenCV. Provided real-time visualization to reflect up-to-date 3D keypoint location changes.

2. ##### Enable constraints to facilitate the annotation process ####

	Offered constraints e.g. *pose-conditioned joint angle limit*, *joint angle lower/upper bound* to ensure the pose validity. 
	
	One might choose the joints to be considered in the objective function.
	
	One might change the initial DoF parameter. 
	
	For some DoF, there is an option to use the initial parameter instead of learnable parameter.
	
	With constraints above, it's convenient to perform a hierarchical optimization, where torso joints are lined up first and then upper/lower body joints are aligned further. Details included in the pdf report.