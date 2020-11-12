#include "DisplayJointOnImage.h"
namespace HumanModel
{
	Point2d DisplayJoint::GetPoint(double slope, double stx, double sty, double newx)
	{
		return Point2d(newx, (newx - stx) * slope + sty);
	}

	double DisplayJoint::Convert(double origin_value, const double mul1, const double mul2, const double b1, const double b2, const double b3)
	{
		return ((origin_value + b1) * mul1 + b2) * mul2 + b3;
	}

	
	void DisplayJoint::DottedLine(Mat rgb, Point2d st, Point2d en, Scalar color, int thickness)
	{		
		const int delta = 5;
		const int gap = 10;
		if (fabs(st.x - en.x) < 1e-6)
		{
			Point2d start = st;
			int direction = (st.y < en.y ? 1 : -1);
			while ((st.y<en.y && start.y<en.y) || (st.y>en.y && start.y>en.y))
			{
				double newy = (direction == 1 ? min(start.y + delta * direction, en.y) : max(start.y + delta * direction, en.y));
				line(rgb, start, Point2d(start.x, newy), color, thickness);
				start = Point2d(start.x, newy);
			}
		}
		else
		{
			double k = (en.y - st.y) / (en.x - st.x);
			Point2d start = st;
			int direction = (st.x < en.x ? 1 : -1);
			while ((st.x < en.x && start.x<en.x) || (st.x>en.x && start.x>en.x))
			{
				double newx = (direction == 1 ? min(start.x + delta * direction, en.x) : max(start.x + delta * direction, en.x));
				line(rgb, start, GetPoint(k, st.x, st.y, newx), color, thickness);
				newx = (direction == 1 ? min(newx + gap * direction, en.x) : max(newx + gap * direction, en.x));
				start = GetPoint(k, st.x, st.y, newx);
			}
		}
		//imshow("", rgb);
		//waitKey(0);
	}

	void DisplayJoint::DrawRing(Mat rgb, int joint_id, double *pred, double *real, int *vis, int *inobj, const double *mul1, const double *mul2, const double *b1, const double *b2, const double *b3)
	{
		if (!vis[joint_id] || !inobj[joint_id]) return;
		circle(rgb, Point2d(Convert(pred[joint_id * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(pred[joint_id * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), 6, white, 0);		
		circle(rgb, Point2d(Convert(real[joint_id * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(real[joint_id * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), 6, white, 0);
		
	}

	void DisplayJoint::Display(Mat rgb, double *pred, double *real, int waitopt, int showpred, int showgt, int showimg, int *vis, int *inobj, char *outputname, const double *mul1, const double *mul2, const double *b1, const double *b2, const double *b3, char *directory)
	//((x + b1) * mul1 + b2) * mul2 + b3   
	{
		Mat pred_img = Mat::zeros(Size(rgb.cols, rgb.rows), CV_8UC3);
		Mat gt_img = Mat::zeros(Size(rgb.cols, rgb.rows), CV_8UC3);
		rgb.copyTo(pred_img);
		rgb.copyTo(gt_img);
		for (int i = 0; i < JointNum; i++)
		{
			if (vis[i] && inobj[i])
			{
				//pred & gt is in range [-bias, bias]				
				if (showpred)
				{
					circle(rgb, Point2d(Convert(pred[i * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(pred[i * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), circle_radius(directory) , color_joint_pred2[i], -2);
					circle(pred_img, Point2d(Convert(pred[i * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(pred[i * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), circle_radius(directory), color_joint_pred2[i], -2);
				}
				if (showgt)
				{
					circle(rgb, Point2d(Convert(real[i * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(real[i * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), circle_radius(directory), color_joint_gt2[i], -2); //real is the real for XYZ						
					circle(gt_img, Point2d(Convert(real[i * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(real[i * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), circle_radius(directory), color_joint_gt2[i], -2); //real is the real for XYZ						
				}
			}

		}
		for (int i = 0; i < 2 * BoneNum - 4; i++)
		{
			if (vis[edges2[i][0]] && vis[edges2[i][1]] && inobj[edges2[i][0]] && inobj[edges2[i][1]])
			{
				if (showpred)
				{					
					if (fig_edges(directory))
					{
						line(rgb, Point2d(Convert(pred[edges2[i][0] * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(pred[edges2[i][0] * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])),
							Point2d(Convert(pred[edges2[i][1] * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(pred[edges2[i][1] * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), color_bone_pred2[i], line_width_2d(directory));
						line(pred_img, Point2d(Convert(pred[edges2[i][0] * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(pred[edges2[i][0] * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])),
							Point2d(Convert(pred[edges2[i][1] * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(pred[edges2[i][1] * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), color_bone_pred2[i], line_width_2d(directory));
					}					
				}
				
				if (showgt)
				{					
					if (fig_edges(directory))
					{
						DottedLine(rgb, Point2d(Convert(real[edges2[i][0] * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(real[edges2[i][0] * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])),
							Point2d(Convert(real[edges2[i][1] * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(real[edges2[i][1] * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), color_bone_gt2[i], line_width_2d(directory));
						DottedLine(gt_img, Point2d(Convert(real[edges2[i][0] * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(real[edges2[i][0] * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])),
							Point2d(Convert(real[edges2[i][1] * 2], mul1[0], mul2[0], b1[0], b2[0], b3[0]), Convert(real[edges2[i][1] * 2 + 1], mul1[1], mul2[1], b1[1], b2[1], b3[1])), color_bone_gt2[i], line_width_2d(directory));
					}					
				}					
			}
		}
		resize(rgb, rgb, Size(show_image_size(directory),show_image_size(directory)));
		if (showpred) resize(pred_img, pred_img, Size(show_image_size(directory), show_image_size(directory)));
		if (showgt) resize(gt_img, gt_img, Size(show_image_size(directory), show_image_size(directory)));
		if (showimg) 
		{ 
			if (showpred && showgt) imshow("show_image", rgb); 			
			if (showpred) imshow("show_prediction", pred_img);
			if (showgt) imshow("show_ground truth", gt_img);
			waitKey(waitopt);
		}
		
		if (outputname != NULL) imwrite(outputname, rgb);
	}
}