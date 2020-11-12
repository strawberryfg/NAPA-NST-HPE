using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace ParameterSetttingUI
{
    public partial class frmmain : Form
    {
        double[] bonelen = { 0.30, 0.35, 0.10, 0.44, 0.07, 0.25, 0.25, 0.25, 0.12 };        
        string[] bonename =
        {
            "bone_knee_connect_ankle",
            "bone_hip_connect_knee",
            "bone_pelvis_connect_hip",
            "bone_thorax_connect_pelvis",
            "bone_thorax_connect_upper_neck",
            "bone_upper_neck_connect_head_top",
            "bone_elbow_connect_wrist",
            "bone_shoulder_connect_elbow",
            "bone_thorax_connect_shoulder"
        };

        string[] dofname =
        {
            "global_trans_x", "global_trans_y", "global_trans_z", "global_rot_x", "global_rot_y", "global_rot_z", "right_knee_rot_x", "right_hip_rot_x", "right_hip_rot_y", "right_hip_rot_z", "left_hip_rot_x", "left_hip_rot_y", "left_hip_rot_z", "left_knee_rot_x",
            "upper_neck_rot_x", "upper_neck_rot_y", "upper_neck_rot_z", "head_top_rot_x", "right_elbow_rot_x", "right_shoulder_rot_x", "right_shoulder_rot_y", "right_shoulder_rot_z", "left_shoulder_rot_x", "left_shoulder_rot_y", "left_shoulder_rot_z", "left_elbow_rot_x",
            "hip_rot_y", "hip_rot_z", "perspective_rot_x", "perspective_rot_y", "perspective_rot_z", "perspective_trans_x", "perspective_trans_y", "perspective_scale"
        };

        double[] angle_lb ={
                              -3600,
                              -3600,
                              -3600,
                              -3600,
                              -3600,
                              -3600,
                              0,
                              -90,
                              -45,
                              -90,
                              -90,
                              -45,
                              0,
                              0,
                              -60,
                              -30,
                              -30,
                              0,
                              -120,
                              -180,
                              -90,
                              -180,
                              -180,
                              -90,
                              0,
                              -120,
                              -90,
                              -30,
                              -90,
                              -90,
                              -90,
                              -10,
                              -10,
                              -100
                          };
        double[] angle_ub ={
                              3600,
                              3600,
                              3600,
                              3600,
                              3600,
                              3600,
                              100,
                              90,
                              45,
                              0,
                              90,
                              45,
                              90,
                              100,
                              90,
                              30,
                              30,
                              30,
                              0,
                              30,
                              90,
                              0,
                              30,
                              90,
                              180,
                              0,
                              90,
                              30,
                              90,
                              90,
                              90,
                              10,
                              10,
                              100
                          };
        string[] angle_lower = new string[34];
        string[] angle_upper = new string[34];

        string[] jointname =
        {
            "right_ankle", "right_knee", "right_hip", "left_hip", "left_knee", "left_ankle", "pelvis", "thorax", "upper_neck", "head_top", 
            "right_wrist", "right_elbow", "right_shoulder", "left_shoulder", "left_elbow", "left_wrist"
        };

        int[] last_backup_id = new int[1111], now_backup_id = new int[1111];
        string[] interpret = new string[1111];
        int last_backup_cnt;

        public frmmain()
        {
            InitializeComponent();
            
        }
        
        
        private void btnsetimageid_Click(object sender, EventArgs e)
        {            
            string filename = txtdirectory.Text + "\\configuration\\current_imageid.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtimageid.Text);
                sw.Close();
            }
            lblnotifications.Text = "Notifications: Press 'l' in the C++ to reload image";
            tmrimageid.Enabled = true;
            lblnotifications.Text = "Set Current Image Index to " + txtimageid.Text;
        }

        private void btnsetgraditer_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\descent_iterations.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtiter.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Gradient Descent Iterations to " + txtiter.Text;
            tmrimageid.Enabled = true;
        }

        private void btnjump_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\memory_id.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtmemoryid.Text);
                sw.Close();
            }
            lblnotifications.Text = "Jumpt to Memory State " + txtmemoryid.Text;
        }

        private void btnsetmousespeed_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\mouse_speed.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtmousespeed.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Mouse Speed to " + txtmousespeed.Text;
            tmrimageid.Enabled = true;
        }

        private void btnsetkeyboardspeed_3d_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\keyboard_speed_3d.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtkeyboardspeed_3d.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Keyboard Speed_3d to " + txtkeyboardspeed_3d.Text;
            tmrimageid.Enabled = true;
        }

        private void btnsetkeyboardspeed_2d_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\keyboard_speed_2d.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtkeyboardspeed_2d.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Keyboard Speed_2d to " + txtkeyboardspeed_2d.Text;
            tmrimageid.Enabled = true;
        }

        private void btnsetcircleradius_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\circle_radius.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtcircleradius.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set circle Radius" + txtcircleradius.Text;
            tmrimageid.Enabled = true;
        }

        private void btnsetsphereradius_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\sphere_radius.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtsphereradius.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Sphere Radius" + txtsphereradius.Text;
            tmrimageid.Enabled = true;
        }

        private void btnsetmulfactor_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\mul_factor.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtmulfactor.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Mul Factor" + txtmulfactor.Text;
            tmrimageid.Enabled = true;
        }

        private void btnsetlinewidth2d_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\line_width_2d.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtlinewidth2d.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Line Width 2d " + txtlinewidth2d.Text;
            tmrimageid.Enabled = true;
        }

        private void btnsetlinewidth3d_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\line_width_3d.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtlinewidth3d.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Line Width 3d " + txtlinewidth3d.Text;
            tmrimageid.Enabled = true;
        }
    
        private void btnsetimagesize_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\show_image_size.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtimagesize.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Image Size " + txtimagesize.Text;
            tmrimageid.Enabled = true;
        }

        private void btnsetshiftratio_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\shift_ratio.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtshiftratio.Text);
                sw.Close();
            }
            lblnotifications.Text = "Set Shift Ratio to " + txtshiftratio.Text;
            tmrimageid.Enabled = true;
        }

        private void tmrimageid_Tick(object sender, EventArgs e)
        {
            try
            {
                //image id
                string filename_imageindex = txtdirectory.Text + "\\configuration\\current_imageid.in";
                System.IO.StreamReader reader = new System.IO.StreamReader(filename_imageindex, System.Text.Encoding.Default);
                string t_id = reader.ReadLine();
                txtimageid.Text = t_id;
                reader.Close();
                //graditer
                string filename_graditer = txtdirectory.Text + "\\configuration\\descent_iterations.in";
                if (File.Exists(filename_graditer))
                {
                    reader = new System.IO.StreamReader(filename_graditer, System.Text.Encoding.Default);
                    string t_iter = reader.ReadLine();
                    txtiter.Text = t_iter;
                    reader.Close();
                }
                //mousespeed
                string filename_mousespeed = txtdirectory.Text + "\\configuration\\mouse_speed.in";
                if (File.Exists(filename_mousespeed))
                {
                    reader = new System.IO.StreamReader(filename_mousespeed, System.Text.Encoding.Default);
                    string t_mousespeed = reader.ReadLine();
                    txtmousespeed.Text = t_mousespeed;
                    reader.Close();
                }
                //keyboardspeed 3d
                string filename_keyboardspeed3d = txtdirectory.Text + "\\configuration\\keyboard_speed_3d.in";
                if (File.Exists(filename_keyboardspeed3d))
                {
                    reader = new System.IO.StreamReader(filename_keyboardspeed3d, System.Text.Encoding.Default);
                    string t_keyboardspeed3d = reader.ReadLine();
                    txtkeyboardspeed_3d.Text = t_keyboardspeed3d;
                    reader.Close();
                }
                //keyboardspeed 2d
                string filename_keyboardspeed2d = txtdirectory.Text + "\\configuration\\keyboard_speed_2d.in";
                if (File.Exists(filename_keyboardspeed2d))
                {
                    reader = new System.IO.StreamReader(filename_keyboardspeed2d, System.Text.Encoding.Default);
                    string t_keyboardspeed2d = reader.ReadLine();
                    txtkeyboardspeed_2d.Text = t_keyboardspeed2d;
                    reader.Close();
                }
                //shift ratio
                string filename_shiftratio = txtdirectory.Text + "\\configuration\\shift_ratio.in";
                if (File.Exists(filename_shiftratio))
                {
                    reader = new System.IO.StreamReader(filename_shiftratio, System.Text.Encoding.Default);
                    string t_shiftratio = reader.ReadLine();
                    txtshiftratio.Text = t_shiftratio;
                    reader.Close();
                }
                //image size
                string filename_imagesize = txtdirectory.Text + "\\configuration\\show_image_size.in";
                if (File.Exists(filename_imagesize))
                {
                    reader = new System.IO.StreamReader(filename_imagesize, System.Text.Encoding.Default);
                    string t_imagesize = reader.ReadLine();
                    txtimagesize.Text = t_imagesize;
                    reader.Close();
                }
                //line width 3d
                string filename_linewidth3d = txtdirectory.Text + "\\configuration\\line_width_3d.in";
                if (File.Exists(filename_linewidth3d))
                {
                    reader = new System.IO.StreamReader(filename_linewidth3d, System.Text.Encoding.Default);
                    string t_linewidth3d = reader.ReadLine();
                    txtlinewidth3d.Text = t_linewidth3d;
                    reader.Close();
                }
                //line width 2d
                string filename_linewidth2d = txtdirectory.Text + "\\configuration\\line_width_2d.in";
                if (File.Exists(filename_linewidth2d))
                {
                    reader = new System.IO.StreamReader(filename_linewidth2d, System.Text.Encoding.Default);
                    string t_linewidth2d = reader.ReadLine();
                    txtlinewidth2d.Text = t_linewidth2d;
                    reader.Close();
                }
                //mul factor
                string filename_mulfactor = txtdirectory.Text + "\\configuration\\mul_factor.in";
                if (File.Exists(filename_mulfactor))
                {
                    reader = new System.IO.StreamReader(filename_mulfactor, System.Text.Encoding.Default);
                    string t_mulfactor = reader.ReadLine();
                    txtmulfactor.Text = t_mulfactor;
                    reader.Close();
                }
                //sphere radius
                string filename_sphereradius = txtdirectory.Text + "\\configuration\\sphere_radius.in";
                if (File.Exists(filename_sphereradius))
                {
                    reader = new System.IO.StreamReader(filename_sphereradius, System.Text.Encoding.Default);
                    string t_sphereradius = reader.ReadLine();
                    txtsphereradius.Text = t_sphereradius;
                    reader.Close();
                }
                //circle radius
                string filename_circleradius = txtdirectory.Text + "\\configuration\\circle_radius.in";
                if (File.Exists(filename_circleradius))
                {
                    reader = new System.IO.StreamReader(filename_circleradius, System.Text.Encoding.Default);
                    string t_circleradius = reader.ReadLine();
                    txtcircleradius.Text = t_circleradius;
                    reader.Close();
                }
                //fig_edges
                string filename_figedges = txtdirectory.Text + "\\configuration\\fig_edges.in";
                if (File.Exists(filename_figedges))
                {
                    reader = new System.IO.StreamReader(filename_figedges, System.Text.Encoding.Default);
                    string t_figedges = reader.ReadLine();
                    if (t_figedges.Equals("1")) chkedges.Checked = true;
                    else chkedges.Checked = false;
                    reader.Close();
                }
            }
            catch (System.Exception err)
            {
                return;
            }
        }

        private void btnsetallbonelen_Click(object sender, EventArgs e)
        {
            TextBox[] txt = { this.txtkneeankle, this.txthipknee, this.txtpelviship, this.txtthoraxpelvis, this.txtthoraxupperneck, this.txtupperneckheadtop, this.txtelbowwrist, this.txtelbowwrist, this.txtshoulderelbow, this.txtthoraxshoulder };
            lblnotifications.Text = "Please make sure that the C++ shell hasn't been launched cause this button will make modifications to the shape parameter(bone length) file!!!";
            string filename = txtdirectory.Text + "\\BoneLength.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < 9; i++)
                {
                    sw.Write(bonename[i]);
                    for (int j = 0; j < 33 - bonename[i].Length; j++) sw.Write(" ");
                    sw.Write(txt[i].Text);
                    sw.Write("\n");
                }
                sw.Close();
            }
            lblnotifications.Text = "Set This Bone Length to All Images";
        }     

        private void txtimageid_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }
      
        private void btnloaddefaultbonelen_Click(object sender, EventArgs e)
        {
            txtkneeankle.Text = Convert.ToString(bonelen[0]);
            txthipknee.Text = Convert.ToString(bonelen[1]);
            txtpelviship.Text = Convert.ToString(bonelen[2]);
            txtthoraxpelvis.Text = Convert.ToString(bonelen[3]);
            txtthoraxupperneck.Text = Convert.ToString(bonelen[4]);
            txtupperneckheadtop.Text = Convert.ToString(bonelen[5]);
            txtelbowwrist.Text = Convert.ToString(bonelen[6]);
            txtshoulderelbow.Text = Convert.ToString(bonelen[7]);
            txtthoraxshoulder.Text = Convert.ToString(bonelen[8]);
            lblnotifications.Text = "Load Default Bone Length";
        }

        private void btnsetcurrentbonelen_Click(object sender, EventArgs e)
        {
            TextBox[] txt = { this.txtkneeankle, this.txthipknee, this.txtpelviship, this.txtthoraxpelvis, this.txtthoraxupperneck, this.txtupperneckheadtop, this.txtelbowwrist, this.txtelbowwrist, this.txtshoulderelbow, this.txtthoraxshoulder };
            string filename = txtdirectory.Text + "\\configuration\\bonelen_" + txtimageid.Text + ".in";;
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < 9; i++)
                {
                    sw.Write(bonename[i]);
                    for (int j = 0; j < 33 - bonename[i].Length; j++) sw.Write(" ");
                    sw.Write(txt[i].Text);
                    sw.Write("\n");
                }
                sw.Close();                
            }
            lblnotifications.Text = "Set Current Bone Length";
        }

        private void btnclearcurrentbonelen_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\bonelen_" + txtimageid.Text + ".in"; ;
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            lblnotifications.Text = "Clear Current Bone Length";
        }

        private void btnclearmodifiedgt_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\gt_2d_modified\\mod_" + txtimageid.Text + ".txt";
            if (File.Exists(filename))
            {
                File.Delete(filename);
                lblnotifications.Text = "Clear Modified Ground Truth";
            }            
        }

        private void chkglobal_CheckedChanged(object sender, EventArgs e)
        {            
            chkglobal_trans_x_limit.Checked = chkglobal_trans_y_limit.Checked = chkglobal_trans_z_limit.Checked = chkglobal_rot_x_limit.Checked = chkglobal_rot_y_limit.Checked = chkglobal_rot_z_limit.Checked = chkglobal.Checked;
            chkperspective_trans_x_limit.Checked = chkperspective_trans_y_limit.Checked = chkperspective_rot_x_limit.Checked = chkperspective_rot_y_limit.Checked = chkperspective_rot_z_limit.Checked = chkperspective_scale_limit.Checked = chkglobal.Checked;            
        }

        private void chkshoulder_CheckedChanged(object sender, EventArgs e)
        {
            chkright_shoulder_rot_x_limit.Checked = chkright_shoulder_rot_y_limit.Checked = chkright_shoulder_rot_z_limit.Checked = chkleft_shoulder_rot_x_limit.Checked = chkleft_shoulder_rot_y_limit.Checked = chkleft_shoulder_rot_z_limit.Checked = chkshoulder.Checked;
        }

        private void chkelbow_CheckedChanged(object sender, EventArgs e)
        {
            chkleft_elbow_rot_x_limit.Checked = chkright_elbow_rot_x_limit.Checked = chkelbow.Checked;
        }

        private void chkpelvis_CheckedChanged(object sender, EventArgs e)
        {
            chkhip_rot_y_limit.Checked = chkhip_rot_z_limit.Checked = chkpelvis.Checked;
        }

        private void chkhip_CheckedChanged(object sender, EventArgs e)
        {
            chkright_hip_rot_x_limit.Checked = chkright_hip_rot_y_limit.Checked = chkright_hip_rot_z_limit.Checked = chkhip.Checked;
            chkleft_hip_rot_x_limit.Checked = chkleft_hip_rot_y_limit.Checked = chkleft_hip_rot_z_limit.Checked = chkhip.Checked;
        }

        private void chkknee_CheckedChanged(object sender, EventArgs e)
        {
            chkleft_knee_rot_x_limit.Checked = chkright_knee_rot_x_limit.Checked = chkknee.Checked;
        }

        private void chkupper_body_CheckedChanged(object sender, EventArgs e)
        {
            chkneckhead.Checked = chkshoulder.Checked = chkelbow.Checked = chkupper_body.Checked;
        }

        private void chklower_body_CheckedChanged(object sender, EventArgs e)
        {
            chkpelvis.Checked = chkhip.Checked = chkknee.Checked = chklower_body.Checked;
        }

        private void chkneckhead_CheckedChanged(object sender, EventArgs e)
        {
            chkupper_neck_rot_x_limit.Checked = chkupper_neck_rot_y_limit.Checked = chkupper_neck_rot_z_limit.Checked = chkhead_top_rot_x_limit.Checked = chkneckhead.Checked;
        }

        private void btnsetcurrentlimitdof_Click(object sender, EventArgs e)
        {
            CheckBox[] chkset =
            {
                this.chkglobal_trans_x_limit, this.chkglobal_trans_y_limit, this.chkglobal_trans_z_limit, this.chkglobal_rot_x_limit, this.chkglobal_rot_y_limit, this.chkglobal_rot_z_limit,
                this.chkright_knee_rot_x_limit, this.chkright_hip_rot_x_limit, this.chkright_hip_rot_y_limit, this.chkright_hip_rot_z_limit, this.chkleft_hip_rot_x_limit, this.chkleft_hip_rot_y_limit, this.chkleft_hip_rot_z_limit, this.chkleft_knee_rot_x_limit,
                this.chkupper_neck_rot_x_limit, this.chkupper_neck_rot_y_limit, this.chkupper_neck_rot_z_limit, this.chkhead_top_rot_x_limit,
                this.chkright_elbow_rot_x_limit, this.chkright_shoulder_rot_x_limit, this.chkright_shoulder_rot_y_limit, this.chkright_shoulder_rot_z_limit, this.chkleft_shoulder_rot_x_limit, this.chkleft_shoulder_rot_y_limit, this.chkleft_shoulder_rot_z_limit, this.chkleft_elbow_rot_x_limit,
                this.chkhip_rot_y_limit, this.chkhip_rot_z_limit,
                this.chkperspective_rot_x_limit, this.chkperspective_rot_y_limit, this.chkperspective_rot_z_limit, this.chkperspective_trans_x_limit, this.chkperspective_trans_y_limit, this.chkperspective_scale_limit
            };
            string filename = txtdirectory.Text + "\\configuration\\islimited_" + txtimageid.Text + ".in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < 34; i++)
                {
                    for (int j = 0; j < 32 - dofname[i].Length; j++) sw.Write(" ");                    
                    sw.Write(dofname[i]);
                    for (int j = 0; j < 3; j++) sw.Write(" ");
                    sw.Write(Convert.ToInt32(chkset[i].Checked));
                    sw.Write("\n");
                }
                sw.Close();
            }
            lblnotifications.Text = "Set Current Limit DoF Succuessfully!";
        }

        private void btnclearcurrentlimitdof_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\islimited_" + txtimageid.Text + ".in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
                lblnotifications.Text = "Clear Current Limit DoF Successfully!";
            }
        }

        private void btnsetdefaultinitparam_Click(object sender, EventArgs e)
        {
            TextBox[] txtinit ={
                                  txtglobal_trans_x_initparam,txtglobal_trans_y_initparam,txtglobal_trans_z_initparam,txtglobal_rot_x_initparam,txtglobal_rot_y_initparam,txtglobal_rot_z_initparam,
                                  txtright_knee_rot_x_initparam,txtright_hip_rot_x_initparam,txtright_hip_rot_y_initparam,txtright_hip_rot_z_initparam,txtleft_hip_rot_x_initparam,txtleft_hip_rot_y_initparam,txtleft_hip_rot_z_initparam,txtleft_knee_rot_x_initparam,
                                  txtupper_neck_rot_x_initparam,txtupper_neck_rot_y_initparam,txtupper_neck_rot_z_initparam,txthead_top_rot_x_initparam,txtright_elbow_rot_x_initparam,
                                  txtright_shoulder_rot_x_initparam,txtright_shoulder_rot_y_initparam,txtright_shoulder_rot_z_initparam,
                                  txtleft_shoulder_rot_x_initparam,txtleft_shoulder_rot_y_initparam,txtleft_shoulder_rot_z_initparam, txtleft_elbow_rot_x_initparam,
                                  txthip_rot_y_initparam,txthip_rot_z_initparam,txtpers_global_rot_x_initparam,txtpers_global_rot_y_initparam,txtpers_global_rot_z_initparam,txtpers_global_trans_x_initparam,txtpers_global_trans_y_initparam,txtpers_scale_initparam
                              };
            for (int i = 0; i < 34; i++) txtinit[i].Text = "0.0";
            txtglobal_trans_z_initparam.Text = "1.0";
            txtpers_scale_initparam.Text = "1.0";
            lblnotifications.Text = "Set Default Init Param to Textboxs";
        }

        private void btnsetallinitparam_Click(object sender, EventArgs e)
        {
            TextBox[] txtinit ={
                                  txtglobal_trans_x_initparam,txtglobal_trans_y_initparam,txtglobal_trans_z_initparam,txtglobal_rot_x_initparam,txtglobal_rot_y_initparam,txtglobal_rot_z_initparam,
                                  txtright_knee_rot_x_initparam,txtright_hip_rot_x_initparam,txtright_hip_rot_y_initparam,txtright_hip_rot_z_initparam,txtleft_hip_rot_x_initparam,txtleft_hip_rot_y_initparam,txtleft_hip_rot_z_initparam,txtleft_knee_rot_x_initparam,
                                  txtupper_neck_rot_x_initparam,txtupper_neck_rot_y_initparam,txtupper_neck_rot_z_initparam,txthead_top_rot_x_initparam,txtright_elbow_rot_x_initparam,
                                  txtright_shoulder_rot_x_initparam,txtright_shoulder_rot_y_initparam,txtright_shoulder_rot_z_initparam,
                                  txtleft_shoulder_rot_x_initparam,txtleft_shoulder_rot_y_initparam,txtleft_shoulder_rot_z_initparam, txtleft_elbow_rot_x_initparam,
                                  txthip_rot_y_initparam,txthip_rot_z_initparam,txtpers_global_rot_x_initparam,txtpers_global_rot_y_initparam,txtpers_global_rot_z_initparam,txtpers_global_trans_x_initparam,txtpers_global_trans_y_initparam,txtpers_scale_initparam
                              };
            string filename = txtdirectory.Text + "\\InitParam.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < 34;i++)
                {
                    for (int j = 0; j < 24 - dofname[i].Length; j++) sw.Write(" ");
                    sw.Write(dofname[i]);
                    sw.Write(" ");
                    sw.Write(txtinit[i].Text);
                    sw.Write("\n");
                }
                sw.Close();
            }
            lblnotifications.Text = "Set All Init Param Done";
        }

        private void btnsetcurrentinitparam_Click(object sender, EventArgs e)
        {
            TextBox[] txtinit ={
                                  txtglobal_trans_x_initparam,txtglobal_trans_y_initparam,txtglobal_trans_z_initparam,txtglobal_rot_x_initparam,txtglobal_rot_y_initparam,txtglobal_rot_z_initparam,
                                  txtright_knee_rot_x_initparam,txtright_hip_rot_x_initparam,txtright_hip_rot_y_initparam,txtright_hip_rot_z_initparam,txtleft_hip_rot_x_initparam,txtleft_hip_rot_y_initparam,txtleft_hip_rot_z_initparam,txtleft_knee_rot_x_initparam,
                                  txtupper_neck_rot_x_initparam,txtupper_neck_rot_y_initparam,txtupper_neck_rot_z_initparam,txthead_top_rot_x_initparam,txtright_elbow_rot_x_initparam,
                                  txtright_shoulder_rot_x_initparam,txtright_shoulder_rot_y_initparam,txtright_shoulder_rot_z_initparam,
                                  txtleft_shoulder_rot_x_initparam,txtleft_shoulder_rot_y_initparam,txtleft_shoulder_rot_z_initparam, txtleft_elbow_rot_x_initparam,
                                  txthip_rot_y_initparam,txthip_rot_z_initparam,txtpers_global_rot_x_initparam,txtpers_global_rot_y_initparam,txtpers_global_rot_z_initparam,txtpers_global_trans_x_initparam,txtpers_global_trans_y_initparam,txtpers_scale_initparam
                              };
            string filename = txtdirectory.Text + "\\configuration\\initparam_" + txtimageid.Text + ".in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < 34; i++)
                {
                    for (int j = 0; j < 24 - dofname[i].Length; j++) sw.Write(" ");
                    sw.Write(dofname[i]);
                    sw.Write(" ");
                    sw.Write(txtinit[i].Text);
                    sw.Write("\n");
                }
                sw.Close();
            }
            lblnotifications.Text = "Set Current Init Param Done";
        }

        private void btnclearcurrentinitparam_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\initparam_" + txtimageid.Text + ".in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            lblnotifications.Text = "Clear Current Init Param Done";
        }

        private void btnreadoptimizedparam_Click(object sender, EventArgs e)
        {
            TextBox[] txtinit ={
                                  txtglobal_trans_x_initparam,txtglobal_trans_y_initparam,txtglobal_trans_z_initparam,txtglobal_rot_x_initparam,txtglobal_rot_y_initparam,txtglobal_rot_z_initparam,
                                  txtright_knee_rot_x_initparam,txtright_hip_rot_x_initparam,txtright_hip_rot_y_initparam,txtright_hip_rot_z_initparam,txtleft_hip_rot_x_initparam,txtleft_hip_rot_y_initparam,txtleft_hip_rot_z_initparam,txtleft_knee_rot_x_initparam,
                                  txtupper_neck_rot_x_initparam,txtupper_neck_rot_y_initparam,txtupper_neck_rot_z_initparam,txthead_top_rot_x_initparam,txtright_elbow_rot_x_initparam,
                                  txtright_shoulder_rot_x_initparam,txtright_shoulder_rot_y_initparam,txtright_shoulder_rot_z_initparam,
                                  txtleft_shoulder_rot_x_initparam,txtleft_shoulder_rot_y_initparam,txtleft_shoulder_rot_z_initparam, txtleft_elbow_rot_x_initparam,
                                  txthip_rot_y_initparam,txthip_rot_z_initparam,txtpers_global_rot_x_initparam,txtpers_global_rot_y_initparam,txtpers_global_rot_z_initparam,txtpers_global_trans_x_initparam,txtpers_global_trans_y_initparam,txtpers_scale_initparam
                              };
            string initfilename = txtdirectory.Text + "\\configuration\\initparam_" + txtimageid.Text + ".in";
            double[] initparam = new double[34];
            if (File.Exists(initfilename))
            {
                System.IO.StreamReader reader = new System.IO.StreamReader(initfilename, System.Text.Encoding.Default);
                int id = 0;
                for (; ; )
                {
                    string t_s = reader.ReadLine();
                    if (t_s == null) break;
                    string s = "";
                    for (int i = 0; i < t_s.Length; i++)
                    {
                        if (t_s[i] == '-' || (t_s[i] >= '0' && t_s[i] <= '9') || t_s[i] == '.')
                        {
                            s += t_s[i];
                        }
                    }
                    initparam[id] = Convert.ToDouble(s);
                    id++;
                }
                reader.Close();
            }
            else
            {
                for (int i = 0; i < 34; i++) initparam[i] = 0.0;
                initparam[2] = 1.0; //global_trans_z
                initparam[33] = 1.0;//scale
            }
            string filename = txtdirectory.Text + "\\result\\param_" + txtimageid.Text + ".txt";
            if (File.Exists(filename))
            {
                System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
                int id = 0;
                for (; ; )
                {
                    string t_s = reader.ReadLine();
                    if (t_s == null) break;
                    string s = "";
                    for (int i = 0; i < t_s.Length; i++)
                    {
                        if (t_s[i] == '-' || (t_s[i]>='0' && t_s[i]<='9') || t_s[i]=='.')
                        {
                            s += t_s[i];
                        }
                    }
                    if (id < 28) txtinit[id].Text = Convert.ToString(Convert.ToDouble(s) + initparam[id]);
                    else txtinit[id].Text = s;
                    id++;
                }
                reader.Close();
                lblnotifications.Text = "Read Current Optimized Param Done";
            }
        }

        private void btnuseoptimizedparam_Click(object sender, EventArgs e)
        {
            TextBox[] txtinit ={
                                  txtglobal_trans_x_initparam,txtglobal_trans_y_initparam,txtglobal_trans_z_initparam,txtglobal_rot_x_initparam,txtglobal_rot_y_initparam,txtglobal_rot_z_initparam,
                                  txtright_knee_rot_x_initparam,txtright_hip_rot_x_initparam,txtright_hip_rot_y_initparam,txtright_hip_rot_z_initparam,txtleft_hip_rot_x_initparam,txtleft_hip_rot_y_initparam,txtleft_hip_rot_z_initparam,txtleft_knee_rot_x_initparam,
                                  txtupper_neck_rot_x_initparam,txtupper_neck_rot_y_initparam,txtupper_neck_rot_z_initparam,txthead_top_rot_x_initparam,txtright_elbow_rot_x_initparam,
                                  txtright_shoulder_rot_x_initparam,txtright_shoulder_rot_y_initparam,txtright_shoulder_rot_z_initparam,
                                  txtleft_shoulder_rot_x_initparam,txtleft_shoulder_rot_y_initparam,txtleft_shoulder_rot_z_initparam, txtleft_elbow_rot_x_initparam,
                                  txthip_rot_y_initparam,txthip_rot_z_initparam,txtpers_global_rot_x_initparam,txtpers_global_rot_y_initparam,txtpers_global_rot_z_initparam,txtpers_global_trans_x_initparam,txtpers_global_trans_y_initparam,txtpers_scale_initparam
                              };
            btnreadoptimizedparam_Click(sender, e);
            string filewrite = txtdirectory.Text + "\\configuration\\initparam_" + txtimageid.Text + ".in";
            StreamWriter sw = File.CreateText(filewrite);
            for (int i = 0; i < 34; i++)
            {
                for (int j = 0; j < 32 - dofname[i].Length; j++)
                {
                    sw.Write(" ");
                }
                sw.Write(dofname[i]);
                sw.Write(" ");
                sw.Write(txtinit[i].Text);
                sw.Write("\n");
            }
            sw.Close(); 
            lblnotifications.Text = "Use Optimized Param Done & Write It To File";            
        }

        private void chkhipinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkright_hip_inobj.Checked = chkleft_hip_inobj.Checked = chkpelvis_inobj.Checked = chkhipinobj.Checked;
        }

        private void chkneckheadinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkupper_neck_inobj.Checked = chkhead_top_inobj.Checked = chkneckheadinobj.Checked;
        }
        private void chkwristinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkright_wrist_inobj.Checked = chkleft_wrist_inobj.Checked = chkwristinobj.Checked;
        }

        private void chkankleinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkright_ankle_inobj.Checked = chkleft_ankle_inobj.Checked = chkankleinobj.Checked;
        }

        private void chkkneeinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkright_knee_inobj.Checked = chkleft_knee_inobj.Checked = chkkneeinobj.Checked;
        }
        private void chkelbowinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkright_elbow_inobj.Checked = chkleft_elbow_inobj.Checked = chkelbowinobj.Checked;
        }

        private void chkshoulderinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkright_shoulder_inobj.Checked = chkleft_shoulder_inobj.Checked = chkthorax_inobj.Checked = chkshoulderinobj.Checked;
        }

        private void chktorsoinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkright_shoulder_inobj.Checked = chkleft_shoulder_inobj.Checked = chkthorax_inobj.Checked = chkright_hip_inobj.Checked = chkleft_hip_inobj.Checked = chkpelvis_inobj.Checked = chktorso.Checked;
        }


        private void chkupper_bodyinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkright_shoulder_inobj.Checked = chkleft_shoulder_inobj.Checked = chkright_elbow_inobj.Checked = chkleft_elbow_inobj.Checked = chkright_wrist_inobj.Checked = chkleft_wrist_inobj.Checked = chkthorax_inobj.Checked = chkupper_neck_inobj.Checked = chkhead_top_inobj.Checked = chkupper_bodyinobj.Checked;
        }

        private void chklower_bodyinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkright_hip_inobj.Checked = chkleft_hip_inobj.Checked = chkright_knee_inobj.Checked = chkleft_knee_inobj.Checked = chkright_ankle_inobj.Checked = chkleft_ankle_inobj.Checked = chkpelvis_inobj.Checked = chklower_bodyinobj.Checked;
        }

        private void chkallinobj_CheckedChanged(object sender, EventArgs e)
        {
            chkupper_bodyinobj.Checked = chklower_bodyinobj.Checked = chkallinobj.Checked;
        }

        private void btnsetcurrentinobj_Click(object sender, EventArgs e)
        {
            CheckBox[] chkinobjset =
            {
                chkright_ankle_inobj,chkright_knee_inobj,chkright_hip_inobj,chkleft_hip_inobj,chkleft_knee_inobj,chkleft_ankle_inobj,chkpelvis_inobj,chkthorax_inobj,chkupper_neck_inobj,chkhead_top_inobj,
                chkright_wrist_inobj,chkright_elbow_inobj,chkright_shoulder_inobj,chkleft_shoulder_inobj,chkleft_elbow_inobj,chkleft_wrist_inobj
            };
            string filename = txtdirectory.Text + "\\configuration\\inobj_" + txtimageid.Text + ".in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < 16;i++)
                {
                    for (int j = 0; j < 30 - jointname[i].Length; j++) sw.Write(" ");
                    sw.Write(jointname[i]);
                    sw.Write(" ");
                    sw.Write(Convert.ToInt32(chkinobjset[i].Checked));
                    sw.Write("\n");
                }
                sw.Close();
            }
            lblnotifications.Text = "Set Current In Objective Function Id Done";
        }

        private void btnclearcurrentinobj_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\inobj_" + txtimageid.Text + ".in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
                lblnotifications.Text = "Clear Current Objective Function Id Done";
            }
            
        }

        private void btnsetdefaultangleconstraint_Click(object sender, EventArgs e)
        {
            TextBox[] txtlower_bound =
            {
                txtglobal_trans_x_lb,txtglobal_trans_y_lb,txtglobal_trans_z_lb,txtglobal_rot_x_lb,txtglobal_rot_y_lb,txtglobal_rot_z_lb,
                txtright_knee_rot_x_lb,txtright_hip_rot_x_lb,txtright_hip_rot_y_lb,txtright_hip_rot_z_lb,txtleft_hip_rot_x_lb,txtleft_hip_rot_y_lb,txtleft_hip_rot_z_lb,txtleft_knee_rot_x_lb,
                txtupper_neck_rot_x_lb,txtupper_neck_rot_y_lb,txtupper_neck_rot_z_lb,txthead_top_rot_x_lb,txtright_elbow_rot_x_lb,
                txtright_shoulder_rot_x_lb,txtright_shoulder_rot_y_lb,txtright_shoulder_rot_z_lb,txtleft_shoulder_rot_x_lb,txtleft_shoulder_rot_y_lb,txtleft_shoulder_rot_z_lb,txtleft_elbow_rot_x_lb,
                txthip_rot_y_lb,txthip_rot_z_lb,txtpers_global_rot_x_lb,txtpers_global_rot_y_lb,txtpers_global_rot_z_lb,txtpers_global_trans_x_lb,txtpers_global_trans_y_lb,txtpers_scale_lb
            };
            TextBox[] txtupper_bound =
            {
                txtglobal_trans_x_ub,txtglobal_trans_y_ub,txtglobal_trans_z_ub,txtglobal_rot_x_ub,txtglobal_rot_y_ub,txtglobal_rot_z_ub,
                txtright_knee_rot_x_ub,txtright_hip_rot_x_ub,txtright_hip_rot_y_ub,txtright_hip_rot_z_ub,txtleft_hip_rot_x_ub,txtleft_hip_rot_y_ub,txtleft_hip_rot_z_ub,txtleft_knee_rot_x_ub,
                txtupper_neck_rot_x_ub,txtupper_neck_rot_y_ub,txtupper_neck_rot_z_ub,txthead_top_rot_x_ub,txtright_elbow_rot_x_ub,
                txtright_shoulder_rot_x_ub,txtright_shoulder_rot_y_ub,txtright_shoulder_rot_z_ub,txtleft_shoulder_rot_x_ub,txtleft_shoulder_rot_y_ub,txtleft_shoulder_rot_z_ub,txtleft_elbow_rot_x_ub,
                txthip_rot_y_ub,txthip_rot_z_ub,txtpers_global_rot_x_ub,txtpers_global_rot_y_ub,txtpers_global_rot_z_ub,txtpers_global_trans_x_ub,txtpers_global_trans_y_ub,txtpers_scale_ub
            };
            for (int i = 0; i < 34; i++)
            {
                txtlower_bound[i].Text = Convert.ToString(angle_lb[i]);
                txtupper_bound[i].Text = Convert.ToString(angle_ub[i]);
            }
            lblnotifications.Text = "Set Default Angle Constraint Done";
        }

        private void btnclearcurrentall_Click(object sender, EventArgs e)
        {
            btnclearcurrentbonelen_Click(sender, e);
            btnclearmodifiedgt_Click(sender, e);
            btnclearcurrentlimitdof_Click(sender, e);
            btnclearcurrentinitparam_Click(sender, e);
            btnclearcurrentinobj_Click(sender, e);
            btnclearcurrentangleconstraint_Click(sender, e);
        }

        private void btnsetallangleconstraint_Click(object sender, EventArgs e)
        {
            TextBox[] txtlower_bound =
            {
                txtglobal_trans_x_lb,txtglobal_trans_y_lb,txtglobal_trans_z_lb,txtglobal_rot_x_lb,txtglobal_rot_y_lb,txtglobal_rot_z_lb,
                txtright_knee_rot_x_lb,txtright_hip_rot_x_lb,txtright_hip_rot_y_lb,txtright_hip_rot_z_lb,txtleft_hip_rot_x_lb,txtleft_hip_rot_y_lb,txtleft_hip_rot_z_lb,txtleft_knee_rot_x_lb,
                txtupper_neck_rot_x_lb,txtupper_neck_rot_y_lb,txtupper_neck_rot_z_lb,txthead_top_rot_x_lb,txtright_elbow_rot_x_lb,
                txtright_shoulder_rot_x_lb,txtright_shoulder_rot_y_lb,txtright_shoulder_rot_z_lb,txtleft_shoulder_rot_x_lb,txtleft_shoulder_rot_y_lb,txtleft_shoulder_rot_z_lb,txtleft_elbow_rot_x_lb,
                txthip_rot_y_lb,txthip_rot_z_lb,txtpers_global_rot_x_lb,txtpers_global_rot_y_lb,txtpers_global_rot_z_lb,txtpers_global_trans_x_lb,txtpers_global_trans_y_lb,txtpers_scale_lb
            };
            TextBox[] txtupper_bound =
            {
                txtglobal_trans_x_ub,txtglobal_trans_y_ub,txtglobal_trans_z_ub,txtglobal_rot_x_ub,txtglobal_rot_y_ub,txtglobal_rot_z_ub,
                txtright_knee_rot_x_ub,txtright_hip_rot_x_ub,txtright_hip_rot_y_ub,txtright_hip_rot_z_ub,txtleft_hip_rot_x_ub,txtleft_hip_rot_y_ub,txtleft_hip_rot_z_ub,txtleft_knee_rot_x_ub,
                txtupper_neck_rot_x_ub,txtupper_neck_rot_y_ub,txtupper_neck_rot_z_ub,txthead_top_rot_x_ub,txtright_elbow_rot_x_ub,
                txtright_shoulder_rot_x_ub,txtright_shoulder_rot_y_ub,txtright_shoulder_rot_z_ub,txtleft_shoulder_rot_x_ub,txtleft_shoulder_rot_y_ub,txtleft_shoulder_rot_z_ub,txtleft_elbow_rot_x_ub,
                txthip_rot_y_ub,txthip_rot_z_ub,txtpers_global_rot_x_ub,txtpers_global_rot_y_ub,txtpers_global_rot_z_ub,txtpers_global_trans_x_ub,txtpers_global_trans_y_ub,txtpers_scale_ub
            };
            string filename = txtdirectory.Text + "\\angle_constraint.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < 34;i++)
                {
                    for (int j = 0; j < 34 - dofname[i].Length; j++) sw.Write(" ");
                    sw.Write(dofname[i]);
                    sw.Write(" ");
                    sw.Write(txtlower_bound[i].Text.PadLeft(10) + "     " + txtupper_bound[i].Text.PadLeft(10));
                    sw.Write("\n");
                    
                }
                sw.Close();
            }
            lblnotifications.Text = "Set All Angle Constraint Done";
        }

        private void btnsetcurrentangleconstraint_Click(object sender, EventArgs e)
        {
            TextBox[] txtlower_bound =
            {
                txtglobal_trans_x_lb,txtglobal_trans_y_lb,txtglobal_trans_z_lb,txtglobal_rot_x_lb,txtglobal_rot_y_lb,txtglobal_rot_z_lb,
                txtright_knee_rot_x_lb,txtright_hip_rot_x_lb,txtright_hip_rot_y_lb,txtright_hip_rot_z_lb,txtleft_hip_rot_x_lb,txtleft_hip_rot_y_lb,txtleft_hip_rot_z_lb,txtleft_knee_rot_x_lb,
                txtupper_neck_rot_x_lb,txtupper_neck_rot_y_lb,txtupper_neck_rot_z_lb,txthead_top_rot_x_lb,txtright_elbow_rot_x_lb,
                txtright_shoulder_rot_x_lb,txtright_shoulder_rot_y_lb,txtright_shoulder_rot_z_lb,txtleft_shoulder_rot_x_lb,txtleft_shoulder_rot_y_lb,txtleft_shoulder_rot_z_lb,txtleft_elbow_rot_x_lb,
                txthip_rot_y_lb,txthip_rot_z_lb,txtpers_global_rot_x_lb,txtpers_global_rot_y_lb,txtpers_global_rot_z_lb,txtpers_global_trans_x_lb,txtpers_global_trans_y_lb,txtpers_scale_lb
            };
            TextBox[] txtupper_bound =
            {
                txtglobal_trans_x_ub,txtglobal_trans_y_ub,txtglobal_trans_z_ub,txtglobal_rot_x_ub,txtglobal_rot_y_ub,txtglobal_rot_z_ub,
                txtright_knee_rot_x_ub,txtright_hip_rot_x_ub,txtright_hip_rot_y_ub,txtright_hip_rot_z_ub,txtleft_hip_rot_x_ub,txtleft_hip_rot_y_ub,txtleft_hip_rot_z_ub,txtleft_knee_rot_x_ub,
                txtupper_neck_rot_x_ub,txtupper_neck_rot_y_ub,txtupper_neck_rot_z_ub,txthead_top_rot_x_ub,txtright_elbow_rot_x_ub,
                txtright_shoulder_rot_x_ub,txtright_shoulder_rot_y_ub,txtright_shoulder_rot_z_ub,txtleft_shoulder_rot_x_ub,txtleft_shoulder_rot_y_ub,txtleft_shoulder_rot_z_ub,txtleft_elbow_rot_x_ub,
                txthip_rot_y_ub,txthip_rot_z_ub,txtpers_global_rot_x_ub,txtpers_global_rot_y_ub,txtpers_global_rot_z_ub,txtpers_global_trans_x_ub,txtpers_global_trans_y_ub,txtpers_scale_ub
            };
            string filename = txtdirectory.Text + "\\configuration\\angleconstraint_" + txtimageid.Text + ".in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < 34; i++)
                {
                    for (int j = 0; j < 34 - dofname[i].Length; j++) sw.Write(" ");
                    sw.Write(dofname[i]);
                    sw.Write(" ");
                    sw.Write(txtlower_bound[i].Text.PadLeft(10) + "     " + txtupper_bound[i].Text.PadLeft(10));
                    sw.Write("\n");
                }
                sw.Close();
            }
            lblnotifications.Text = "Set Current Angle Constraint Done";
        }

        private void btnclearcurrentangleconstraint_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\angleconstraint_" + txtimageid.Text + ".in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
                lblnotifications.Text = "Clear Current Angle Constraint Done";
            }
        }

        private void btnsetedges_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\fig_edges.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(Convert.ToInt32(chkedges.Checked));
                sw.Close();
            }
            lblnotifications.Text = "Set Fig Edges Done";
            tmrimageid.Enabled = true;
        }

        private void txtiter_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtkeyboardspeed_3d_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtkeyboardspeed_2d_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtshiftratio_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtimagesize_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtlinewidth3d_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtlinewidth2d_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtmulfactor_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtsphereradius_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtcircleradius_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void chkedges_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void txtmousespeed_Click(object sender, EventArgs e)
        {
            tmrimageid.Enabled = false;
        }

        private void btndefaultgraditer_Click(object sender, EventArgs e)
        {
            txtiter.Text = "10000";
            string filename = txtdirectory.Text + "\\configuration\\descent_iterations.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultmousespeed_Click(object sender, EventArgs e)
        {
            txtmousespeed.Text = "0.5";
            string filename = txtdirectory.Text + "\\configuration\\mouse_speed.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultkeyboard_3d_Click(object sender, EventArgs e)
        {
            txtkeyboardspeed_3d.Text = "1.0";
            string filename = txtdirectory.Text + "\\configuration\\keyboard_speed_3d.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultkeyboard_2d_Click(object sender, EventArgs e)
        {
            txtkeyboardspeed_2d.Text = "1.0";
            string filename = txtdirectory.Text + "\\configuration\\keyboard_speed_2d.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultshiftratio_Click(object sender, EventArgs e)
        {
            txtshiftratio.Text = "0.3";
            string filename = txtdirectory.Text + "\\configuration\\shift_ratio.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultimagesize_Click(object sender, EventArgs e)
        {
            txtimagesize.Text = "500.0";
            string filename = txtdirectory.Text + "\\configuration\\show_image_size.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultlinewidth_3d_Click(object sender, EventArgs e)
        {
            txtlinewidth3d.Text = "10.0";
            string filename = txtdirectory.Text + "\\configuration\\line_width_3d.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultlinewidth_2d_Click(object sender, EventArgs e)
        {
            txtlinewidth2d.Text = "4.0";
            string filename = txtdirectory.Text + "\\configuration\\line_width_2d.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultmulfactor_Click(object sender, EventArgs e)
        {
            txtmulfactor.Text = "7.0";
            string filename = txtdirectory.Text + "\\configuration\\mul_factor.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultsphereradius_Click(object sender, EventArgs e)
        {
            txtsphereradius.Text = "0.1";
            string filename = txtdirectory.Text + "\\configuration\\sphere_radius.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void btndefaultcircleradius_Click(object sender, EventArgs e)
        {
            txtcircleradius.Text = "3.0";
            string filename = txtdirectory.Text + "\\configuration\\circle_radius.in";
            if (File.Exists(filename))
            {
                File.Delete(filename);
            }
            tmrimageid.Enabled = false;
        }

        private void tmrshowimageid_Tick(object sender, EventArgs e)
        {
            try
            {
                string filename_imageindex = txtdirectory.Text + "\\configuration\\current_imageid.in";
                System.IO.StreamReader reader = new System.IO.StreamReader(filename_imageindex, System.Text.Encoding.Default);
                string t_id = reader.ReadLine();
                lblcurrentimageid.Text = "Current Image Id # " + t_id;
                reader.Close();
            }
            catch(System.Exception err)
            {
                return;
            }
            
        }

        private void btnsaveasindex_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\saveindex.in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                sw.WriteLine(txtsaveasindex.Text);
                sw.Close();
            }
            tmrcheckbackupid.Enabled = true;
            lblnotifications.Text = "Modify saveindex.in";
        }

        private void tmrcheckbackupid_Tick(object sender, EventArgs e)
        {
         try
         {
             int max_id = -1, backup_cnt = 0;
             bool different_backup = false;
             for (int i = 0; i < 1000; i++)
             {
                 string filename_backup = txtdirectory.Text + "\\intermediate\\pred_2d_" + txtimageid.Text + "_" + Convert.ToString(i) + ".txt";
                 string filename_interpretation = txtdirectory.Text + "\\intermediate\\interpretation_" + txtimageid.Text + "_" + Convert.ToString(i) + ".txt";
                 if (File.Exists(filename_backup))
                 {
                     System.IO.StreamReader reader = new System.IO.StreamReader(filename_interpretation, System.Text.Encoding.Default);
                     string info = reader.ReadLine();
                     interpret[backup_cnt] = info;
                     reader.Close();
                     max_id = i;
                     if (i != last_backup_id[backup_cnt]) different_backup = true;
                     last_backup_id[backup_cnt] = i;
                     backup_cnt++;
                 }
             }
             if (backup_cnt != last_backup_cnt) different_backup = true;
             last_backup_cnt = backup_cnt;
             if (different_backup)
             {
                 lstbackupid.Items.Clear();
                 for (int i = 0; i < backup_cnt; i++)
                 {
                     ListViewItem lvi = new ListViewItem();
                     lvi.Text = Convert.ToString(last_backup_id[i]);
                     lvi.SubItems.Add(interpret[i]);
                     lstbackupid.Items.Add(lvi);
                 }
                 lblnotifications.Text = "Refresh and Reload Available Backup Indexs";
             }
             string save_id_file = txtdirectory.Text + "\\configuration\\saveindex.in";
             System.IO.StreamReader reader_id = new System.IO.StreamReader(save_id_file, System.Text.Encoding.Default);
             string t_s = reader_id.ReadLine();
             txtsaveasindex.Text = t_s;
             reader_id.Close();            
         }
            catch(System.Exception err)
         {
             return;
         }
         
        }



        private void txtsaveasindex_Click(object sender, EventArgs e)
        {
            tmrcheckbackupid.Enabled = false;
        }

        private void SetAngleConstraint(int id, bool optimal, bool load_default)
        {
            string filename;
            if (!optimal) filename = txtdirectory.Text + "\\intermediate\\angleconstraint_" + txtimageid.Text + "_" + Convert.ToString(id) + ".in";
            else
            {
                filename = txtdirectory.Text + "\\intermediate\\angleconstraint_" + txtimageid.Text + ".in";
                if (!File.Exists(filename)) filename = txtdirectory.Text + "\\angle_constraint.in";
            }
            System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
            TextBox[] txtangle_lb_current =
            {
                txtcurrentglobal_trans_x_lb_constraint,txtcurrentglobal_trans_y_lb_constraint,txtcurrentglobal_trans_z_lb_constraint,txtcurrentglobal_rot_x_lb_constraint,txtcurrentglobal_rot_y_lb_constraint,txtcurrentglobal_rot_z_lb_constraint,
                txtcurrentright_knee_rot_x_lb_constraint,txtcurrentright_hip_rot_x_lb_constraint,txtcurrentright_hip_rot_y_lb_constraint,txtcurrentright_hip_rot_z_lb_constraint,txtcurrentleft_hip_rot_x_lb_constraint,txtcurrentleft_hip_rot_y_lb_constraint,txtcurrentleft_hip_rot_z_lb_constraint,
                txtcurrentleft_knee_rot_x_lb_constraint,txtcurrentupper_neck_rot_x_lb_constraint,txtcurrentupper_neck_rot_y_lb_constraint,txtcurrentupper_neck_rot_z_lb_constraint,txtcurrenthead_top_rot_x_lb_constraint,txtcurrentright_elbow_rot_x_lb_constraint,txtcurrentright_shoulder_rot_x_lb_constraint,txtcurrentright_shoulder_rot_y_lb_constraint,txtcurrentright_shoulder_rot_z_lb_constraint,
                txtcurrentleft_shoulder_rot_x_lb_constraint,txtcurrentleft_shoulder_rot_y_lb_constraint,txtcurrentleft_shoulder_rot_z_lb_constraint,txtcurrentleft_elbow_rot_x_lb_constraint,txtcurrenthip_rot_y_lb_constraint,txtcurrenthip_rot_z_lb_constraint,
                txtcurrentpers_global_rot_x_lb_constraint,txtcurrentpers_global_rot_y_lb_constraint,txtcurrentpers_global_rot_z_lb_constraint,txtcurrentpers_global_trans_x_lb_constraint,txtcurrentpers_global_trans_y_lb_constraint,txtcurrentpers_scale_lb_constraint
            };
            TextBox[] txtangle_lb =
            {
                txtglobal_trans_x_lb,txtglobal_trans_y_lb,txtglobal_trans_z_lb,txtglobal_rot_x_lb,txtglobal_rot_y_lb,txtglobal_rot_z_lb,
                txtright_knee_rot_x_lb,txtright_hip_rot_x_lb,txtright_hip_rot_y_lb,txtright_hip_rot_z_lb,txtleft_hip_rot_x_lb,txtleft_hip_rot_y_lb,txtleft_hip_rot_z_lb,
                txtleft_knee_rot_x_lb,txtupper_neck_rot_x_lb,txtupper_neck_rot_y_lb,txtupper_neck_rot_z_lb,txthead_top_rot_x_lb,txtright_elbow_rot_x_lb,txtright_shoulder_rot_x_lb,txtright_shoulder_rot_y_lb,txtright_shoulder_rot_z_lb,
                txtleft_shoulder_rot_x_lb,txtleft_shoulder_rot_y_lb,txtleft_shoulder_rot_z_lb,txtleft_elbow_rot_x_lb,txthip_rot_y_lb,txthip_rot_z_lb,
                txtpers_global_rot_x_lb,txtpers_global_rot_y_lb,txtpers_global_rot_z_lb,txtpers_global_trans_x_lb,txtpers_global_trans_y_lb,txtpers_scale_lb
            };
            TextBox[] txtangle_ub_current =
            {
                txtcurrentglobal_trans_x_ub_constraint,txtcurrentglobal_trans_y_ub_constraint,txtcurrentglobal_trans_z_ub_constraint,txtcurrentglobal_rot_x_ub_constraint,txtcurrentglobal_rot_y_ub_constraint,txtcurrentglobal_rot_z_ub_constraint,
                txtcurrentright_knee_rot_x_ub_constraint,txtcurrentright_hip_rot_x_ub_constraint,txtcurrentright_hip_rot_y_ub_constraint,txtcurrentright_hip_rot_z_ub_constraint,txtcurrentleft_hip_rot_x_ub_constraint,txtcurrentleft_hip_rot_y_ub_constraint,txtcurrentleft_hip_rot_z_ub_constraint,
                txtcurrentleft_knee_rot_x_ub_constraint,txtcurrentupper_neck_rot_x_ub_constraint,txtcurrentupper_neck_rot_y_ub_constraint,txtcurrentupper_neck_rot_z_ub_constraint,txtcurrenthead_top_rot_x_ub_constraint,txtcurrentright_elbow_rot_x_ub_constraint,txtcurrentright_shoulder_rot_x_ub_constraint,txtcurrentright_shoulder_rot_y_ub_constraint,txtcurrentright_shoulder_rot_z_ub_constraint,
                txtcurrentleft_shoulder_rot_x_ub_constraint,txtcurrentleft_shoulder_rot_y_ub_constraint,txtcurrentleft_shoulder_rot_z_ub_constraint,txtcurrentleft_elbow_rot_x_ub_constraint,txtcurrenthip_rot_y_ub_constraint,txtcurrenthip_rot_z_ub_constraint,
                txtcurrentpers_global_rot_x_ub_constraint,txtcurrentpers_global_rot_y_ub_constraint,txtcurrentpers_global_rot_z_ub_constraint,txtcurrentpers_global_trans_x_ub_constraint,txtcurrentpers_global_trans_y_ub_constraint,txtcurrentpers_scale_ub_constraint
            };
            TextBox[] txtangle_ub =
            {
                txtglobal_trans_x_ub,txtglobal_trans_y_ub,txtglobal_trans_z_ub,txtglobal_rot_x_ub,txtglobal_rot_y_ub,txtglobal_rot_z_ub,
                txtright_knee_rot_x_ub,txtright_hip_rot_x_ub,txtright_hip_rot_y_ub,txtright_hip_rot_z_ub,txtleft_hip_rot_x_ub,txtleft_hip_rot_y_ub,txtleft_hip_rot_z_ub,
                txtleft_knee_rot_x_ub,txtupper_neck_rot_x_ub,txtupper_neck_rot_y_ub,txtupper_neck_rot_z_ub,txthead_top_rot_x_ub,txtright_elbow_rot_x_ub,txtright_shoulder_rot_x_ub,txtright_shoulder_rot_y_ub,txtright_shoulder_rot_z_ub,
                txtleft_shoulder_rot_x_ub,txtleft_shoulder_rot_y_ub,txtleft_shoulder_rot_z_ub,txtleft_elbow_rot_x_ub,txthip_rot_y_ub,txthip_rot_z_ub,
                txtpers_global_rot_x_ub,txtpers_global_rot_y_ub,txtpers_global_rot_z_ub,txtpers_global_trans_x_ub,txtpers_global_trans_y_ub,txtpers_scale_ub
            };
            for (int i = 0; i < 34; i++)
            {
                string t_s = reader.ReadLine();
                if (t_s == null) break;
                string s1 = "", s2 = "";
                int t_id = 0;
                for (int j = 0; j < t_s.Length; j++)
                {
                    if (t_s[j] == '-' || (t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.')
                    {
                        if (t_id == 0) s1 += t_s[j]; else s2 += t_s[j];
                    }
                    else if (s1.Length > 0 && t_s[j] == ' ') t_id = 1;
                }
                if (load_default)
                {
                    txtangle_lb[i].Text = s1;
                    txtangle_ub[i].Text = s2;
                }
                else
                {
                    txtangle_lb_current[i].Text = s1;
                    txtangle_ub_current[i].Text = s2;
                }
            }
            reader.Close();
        }

        private void SetInObj(int id, bool optimal, bool load_default)
        {
            string filename;
            if (!optimal) filename = txtdirectory.Text + "\\intermediate\\inobj_" + txtimageid.Text + "_" + Convert.ToString(id) + ".in";
            else
            {
                filename = txtdirectory.Text + "\\configuration\\inobj_" + txtimageid.Text + ".in";
                if (!File.Exists(filename)) filename = txtdirectory.Text + "\\AllInObj.in";
            }
            System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
            CheckBox[] chkinobj_current =
            {
                chkcurrentright_ankle_inobj,chkcurrentright_knee_inobj,chkcurrentright_hip_inobj,chkcurrentleft_hip_inobj,chkcurrentleft_knee_inobj,chkcurrentleft_ankle_inobj,chkcurrentpelvis_inobj,chkcurrentthorax_inobj,chkcurrentupper_neck_inobj,
                chkcurrenthead_top_inobj,chkcurrentright_wrist_inobj,chkcurrentright_elbow_inobj,chkcurrentright_shoulder_inobj,chkcurrentleft_shoulder_inobj,chkcurrentleft_elbow_inobj,chkcurrentleft_wrist_inobj
            };
            CheckBox[] chkinobj =
            {
                chkright_ankle_inobj,chkright_knee_inobj,chkright_hip_inobj,chkleft_hip_inobj,chkleft_knee_inobj,chkleft_ankle_inobj,chkpelvis_inobj,chkthorax_inobj,chkupper_neck_inobj,
                chkhead_top_inobj,chkright_wrist_inobj,chkright_elbow_inobj,chkright_shoulder_inobj,chkleft_shoulder_inobj,chkleft_elbow_inobj,chkleft_wrist_inobj
            };
            for (int i = 0; i < 16; i++)
            {
                string t_s = reader.ReadLine();
                if (t_s == null) break;
                string s = "";
                for (int j = 0; j < t_s.Length; j++)
                {
                    if ((t_s[j] >= '0' && t_s[j] <= '9'))
                    {
                        s += t_s[j];
                    }
                }
                if (load_default) chkinobj[i].Checked = Convert.ToBoolean(Convert.ToInt32(s));
                else chkinobj_current[i].Checked = Convert.ToBoolean(Convert.ToInt32(s));
            }
            reader.Close();
        }

        private void SetParam(int id, bool optimal)
        {
            string filename;
            if (!optimal) filename = txtdirectory.Text + "\\intermediate\\param_" + txtimageid.Text + "_" + Convert.ToString(id) + ".txt";
            else
            {
                filename = txtdirectory.Text + "\\result\\param_" + txtimageid.Text + ".txt";    
            }
            
            TextBox[] txtparam ={
                                       txtcurrentglobal_trans_x_param,txtcurrentglobal_trans_y_param,txtcurrentglobal_trans_z_param,txtcurrentglobal_rot_x_param,txtcurrentglobal_rot_y_param,txtcurrentglobal_rot_z_param,
                                       txtcurrentright_knee_rot_x_param,txtcurrentright_hip_rot_x_param,txtcurrentright_hip_rot_y_param,txtcurrentright_hip_rot_z_param,txtcurrentleft_hip_rot_x_param,txtcurrentleft_hip_rot_y_param,txtcurrentleft_hip_rot_z_param,
                                       txtcurrentleft_knee_rot_x_param,txtcurrentupper_neck_rot_x_param,txtcurrentupper_neck_rot_y_param,txtcurrentupper_neck_rot_z_param,txtcurrenthead_top_rot_x_param,txtcurrentright_elbow_rot_x_param,txtcurrentright_shoulder_rot_x_param,txtcurrentright_shoulder_rot_y_param,
                                       txtcurrentright_shoulder_rot_z_param,txtcurrentleft_shoulder_rot_x_param,txtcurrentleft_shoulder_rot_y_param,txtcurrentleft_shoulder_rot_z_param,txtcurrentleft_elbow_rot_x_param,txtcurrenthip_rot_y_param,
                                       txtcurrenthip_rot_z_param,txtcurrentpers_global_rot_x_param,txtcurrentpers_global_rot_y_param,txtcurrentpers_global_rot_z_param,txtcurrentpers_global_trans_x_param,txtcurrentpers_global_trans_y_param,txtcurrentpers_scale_param
                                   };
            if (!File.Exists(filename))
            {
                for (int i = 0; i < 34; i++) txtparam[i].Text = "";
                return;
            }
            System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
            for (int i = 0; i < 34; i++)
            {
                string t_s = reader.ReadLine();
                if (t_s == null) break;
                string s = "";
                for (int j = 0; j < t_s.Length; j++)
                {
                    if ((t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.' || t_s[j] == '-')
                    {
                        s += t_s[j];
                    }
                }
                txtparam[i].Text = s;
            }
            reader.Close();
        }

        private void SetInitParam(int id, bool optimal, bool load_default)
        {
            string filename;
            if (!optimal) filename = txtdirectory.Text + "\\intermediate\\initparam_" + txtimageid.Text + "_" + Convert.ToString(id) + ".in";
            else
            {
                filename = txtdirectory.Text + "\\configuration\\initparam_" + txtimageid.Text + ".in";
                if (!File.Exists(filename)) filename = txtdirectory.Text + "\\InitParam.in";
            }
            System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
            TextBox[] txtinitparam_current ={
                                       txtcurrentglobal_trans_x_initparam,txtcurrentglobal_trans_y_initparam,txtcurrentglobal_trans_z_initparam,txtcurrentglobal_rot_x_initparam,txtcurrentglobal_rot_y_initparam,txtcurrentglobal_rot_z_initparam,
                                       txtcurrentright_knee_rot_x_initparam,txtcurrentright_hip_rot_x_initparam,txtcurrentright_hip_rot_y_initparam,txtcurrentright_hip_rot_z_initparam,txtcurrentleft_hip_rot_x_initparam,txtcurrentleft_hip_rot_y_initparam,txtcurrentleft_hip_rot_z_initparam,
                                       txtcurrentleft_knee_rot_x_initparam,txtcurrentupper_neck_rot_x_initparam,txtcurrentupper_neck_rot_y_initparam,txtcurrentupper_neck_rot_z_initparam,txtcurrenthead_top_rot_x_initparam,txtcurrentright_elbow_rot_x_initparam,txtcurrentright_shoulder_rot_x_initparam,txtcurrentright_shoulder_rot_y_initparam,
                                       txtcurrentright_shoulder_rot_z_initparam,txtcurrentleft_shoulder_rot_x_initparam,txtcurrentleft_shoulder_rot_y_initparam,txtcurrentleft_shoulder_rot_z_initparam,txtcurrentleft_elbow_rot_x_initparam,txtcurrenthip_rot_y_initparam,
                                       txtcurrenthip_rot_z_initparam,txtcurrentpers_global_rot_x_initparam,txtcurrentpers_global_rot_y_initparam,txtcurrentpers_global_rot_z_initparam,txtcurrentpers_global_trans_x_initparam,txtcurrentpers_global_trans_y_initparam,txtcurrentpers_scale_initparam
                                   };
            TextBox[] txtinitparam ={
                                       txtglobal_trans_x_initparam,txtglobal_trans_y_initparam,txtglobal_trans_z_initparam,txtglobal_rot_x_initparam,txtglobal_rot_y_initparam,txtglobal_rot_z_initparam,
                                       txtright_knee_rot_x_initparam,txtright_hip_rot_x_initparam,txtright_hip_rot_y_initparam,txtright_hip_rot_z_initparam,txtleft_hip_rot_x_initparam,txtleft_hip_rot_y_initparam,txtleft_hip_rot_z_initparam,
                                       txtleft_knee_rot_x_initparam,txtupper_neck_rot_x_initparam,txtupper_neck_rot_y_initparam,txtupper_neck_rot_z_initparam,txthead_top_rot_x_initparam,txtright_elbow_rot_x_initparam,txtright_shoulder_rot_x_initparam,txtright_shoulder_rot_y_initparam,
                                       txtright_shoulder_rot_z_initparam,txtleft_shoulder_rot_x_initparam,txtleft_shoulder_rot_y_initparam,txtleft_shoulder_rot_z_initparam,txtleft_elbow_rot_x_initparam,txthip_rot_y_initparam,
                                       txthip_rot_z_initparam,txtpers_global_rot_x_initparam,txtpers_global_rot_y_initparam,txtpers_global_rot_z_initparam,txtpers_global_trans_x_initparam,txtpers_global_trans_y_initparam,txtpers_scale_initparam
                                   };
            for (int i = 0; i < 34; i++)
            {
                string t_s = reader.ReadLine();
                if (t_s == null) break;
                string s = "";
                for (int j = 0; j < t_s.Length; j++)
                {
                    if ((t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.' || t_s[j] == '-') 
                    {
                        s += t_s[j];
                    }
                }
                if (load_default) txtinitparam[i].Text = s;
                else txtinitparam_current[i].Text = s;
            }
            reader.Close();
        }

        private void SetLimitDoF(int id, bool optimal, bool load_default)
        {
            string filename;
            if (!optimal) filename = txtdirectory.Text + "\\intermediate\\islimited_" + txtimageid.Text + "_" + Convert.ToString(id) + ".in";
            else
            {
                filename = txtdirectory.Text + "\\configuration\\islimited_" + txtimageid.Text + ".in";
                if (!File.Exists(filename)) filename = txtdirectory.Text + "\\NoLimitDoF.in";
            }
            System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
            CheckBox[] chklimitdofset_current =
            {
                chkcurrentglobal_trans_x_limit,chkcurrentglobal_trans_y_limit,chkcurrentglobal_trans_z_limit,chkcurrentglobal_rot_x_limit,chkcurrentglobal_rot_y_limit,chkcurrentglobal_rot_z_limit,
                chkcurrentright_knee_rot_x_limit,chkcurrentright_hip_rot_x_limit,chkcurrentright_hip_rot_y_limit,chkcurrentright_hip_rot_z_limit,chkcurrentleft_hip_rot_x_limit,chkcurrentleft_hip_rot_y_limit,chkcurrentleft_hip_rot_z_limit,
                chkcurrentleft_knee_rot_x_limit,chkcurrentupper_neck_rot_x_limit,chkcurrentupper_neck_rot_y_limit,chkcurrentupper_neck_rot_z_limit,chkcurrenthead_top_rot_x_limit,chkcurrentright_elbow_rot_x_limit,
                chkcurrentright_shoulder_rot_x_limit,chkcurrentright_shoulder_rot_y_limit,chkcurrentright_shoulder_rot_z_limit,chkcurrentleft_shoulder_rot_x_limit,chkcurrentleft_shoulder_rot_y_limit,chkcurrentleft_shoulder_rot_z_limit,chkcurrentleft_elbow_rot_x_limit,
                chkcurrenthip_rot_y_limit,chkcurrenthip_rot_z_limit,chkcurrentpers_rot_x_limit,chkcurrentpers_rot_y_limit,chkcurrentpers_rot_z_limit,chkcurrentpers_trans_x_limit,chkcurrentpers_trans_y_limit,chkcurrentpers_scale_limit
            };
            CheckBox[] chklimitdofset =
            {
                chkglobal_trans_x_limit,chkglobal_trans_y_limit,chkglobal_trans_z_limit,chkglobal_rot_x_limit,chkglobal_rot_y_limit,chkglobal_rot_z_limit,
                chkright_knee_rot_x_limit,chkright_hip_rot_x_limit,chkright_hip_rot_y_limit,chkright_hip_rot_z_limit,chkleft_hip_rot_x_limit,chkleft_hip_rot_y_limit,chkleft_hip_rot_z_limit,
                chkleft_knee_rot_x_limit,chkupper_neck_rot_x_limit,chkupper_neck_rot_y_limit,chkupper_neck_rot_z_limit,chkhead_top_rot_x_limit,chkright_elbow_rot_x_limit,
                chkright_shoulder_rot_x_limit,chkright_shoulder_rot_y_limit,chkright_shoulder_rot_z_limit,chkleft_shoulder_rot_x_limit,chkleft_shoulder_rot_y_limit,chkleft_shoulder_rot_z_limit,chkleft_elbow_rot_x_limit,
                chkhip_rot_y_limit,chkhip_rot_z_limit,chkperspective_rot_x_limit,chkperspective_rot_y_limit,chkperspective_rot_z_limit,chkperspective_trans_x_limit,chkperspective_trans_y_limit,chkperspective_scale_limit
            };
            for (int i = 0; i < 34; i++)
            {
                string t_s = reader.ReadLine();
                if (t_s == null) break;
                string s = "";
                for (int j = 0; j < t_s.Length; j++)
                {
                    if ((t_s[j] >= '0' && t_s[j] <= '9'))
                    {
                        s += t_s[j];
                    }
                }
                if (load_default) chklimitdofset[i].Checked = Convert.ToBoolean(Convert.ToInt32(s));
                else chklimitdofset_current[i].Checked = Convert.ToBoolean(Convert.ToInt32(s));  
            }
            reader.Close();
        }


        private void SetBoneLength(int id, bool optimal, bool load_default)
        {
            string filename;
            if (!optimal) filename = txtdirectory.Text + "\\intermediate\\bonelen_" + txtimageid.Text + "_" + Convert.ToString(id) + ".in";
            else
            {
                filename = txtdirectory.Text + "\\configuration\\bonelen_" + txtimageid.Text + ".in";
                if (!File.Exists(filename)) filename = txtdirectory.Text + "\\BoneLength.in";
            }
            System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
            TextBox[] txtbone_current = { txtcurrentkneeankle, txtcurrenthipknee, txtcurrentpelviship, txtcurrentthoraxpelvis, txtcurrentthoraxupperneck, txtcurrentupperneckheadtop, txtcurrentelbowwrist, txtcurrentelbowwrist, txtcurrentshoulderelbow, txtcurrentthoraxshoulder };
            TextBox[] txtbone = { txtkneeankle, txthipknee, txtpelviship, txtthoraxpelvis, txtthoraxupperneck, txtupperneckheadtop, txtelbowwrist, txtelbowwrist, txtshoulderelbow, txtthoraxshoulder };            
            for (int i = 0; i < 9; i++)
            {
                string t_s = reader.ReadLine();
                if (t_s == null) break;
                string s = "";
                for (int j = 0; j < t_s.Length; j++)
                {
                    if ((t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.')
                    {
                        s += t_s[j];
                    }
                }
                if (load_default) txtbone[i].Text = s; else txtbone_current[i].Text = s;
            }
            reader.Close();
        }

        private void Set3DLocation(int id, bool optimal)
        {
            //pred_3d
            TextBox[] txtpred_3d_x ={
                                 txtcurrentright_ankle_3d_x, txtcurrentright_knee_3d_x, txtcurrentright_hip_3d_x, txtcurrentleft_hip_3d_x, txtcurrentleft_knee_3d_x,
                                 txtcurrentleft_ankle_3d_x,txtcurrentpelvis_3d_x,   txtcurrentthorax_3d_x,txtcurrentupper_neck_3d_x,txtcurrenthead_top_3d_x,
                                 txtcurrentright_wrist_3d_x,txtcurrentright_elbow_3d_x,txtcurrentright_shoulder_3d_x,txtcurrentleft_shoulder_3d_x,txtcurrentleft_elbow_3d_x,txtcurrentleft_wrist_3d_x
                             };
            TextBox[] txtpred_3d_y ={
                                 txtcurrentright_ankle_3d_y, txtcurrentright_knee_3d_y, txtcurrentright_hip_3d_y, txtcurrentleft_hip_3d_y, txtcurrentleft_knee_3d_y,
                                 txtcurrentleft_ankle_3d_y,txtcurrentpelvis_3d_y,   txtcurrentthorax_3d_y,txtcurrentupper_neck_3d_y,txtcurrenthead_top_3d_y,
                                 txtcurrentright_wrist_3d_y,txtcurrentright_elbow_3d_y,txtcurrentright_shoulder_3d_y,txtcurrentleft_shoulder_3d_y,txtcurrentleft_elbow_3d_y,txtcurrentleft_wrist_3d_y
                             };
            TextBox[] txtpred_3d_z ={
                                 txtcurrentright_ankle_3d_z, txtcurrentright_knee_3d_z, txtcurrentright_hip_3d_z, txtcurrentleft_hip_3d_z, txtcurrentleft_knee_3d_z,
                                 txtcurrentleft_ankle_3d_z,txtcurrentpelvis_3d_z,   txtcurrentthorax_3d_z,txtcurrentupper_neck_3d_z,txtcurrenthead_top_3d_z,
                                 txtcurrentright_wrist_3d_z,txtcurrentright_elbow_3d_z,txtcurrentright_shoulder_3d_z,txtcurrentleft_shoulder_3d_z,txtcurrentleft_elbow_3d_z,txtcurrentleft_wrist_3d_z
                             };
            
            string filename_pred_3d;
            if (!optimal) filename_pred_3d = txtdirectory.Text + "\\intermediate\\pred_3d_" + txtimageid.Text + "_" + Convert.ToString(id) + ".txt";
            else
            {
                filename_pred_3d = txtdirectory.Text + "\\result\\pred_3d_" + txtimageid.Text + ".txt";                
            }
            if (File.Exists(filename_pred_3d))
            {
                System.IO.StreamReader reader = new System.IO.StreamReader(filename_pred_3d, System.Text.Encoding.Default);
                for (int i = 0; i < 16; i++)
                {
                    string t_s = reader.ReadLine();
                    if (t_s == null) break;
                    string s1 = "", s2 = "", s3 = "";
                    int t_id = 0;
                    for (int j = 0; j < t_s.Length; j++)
                    {
                        if (t_s[j] == '-' || (t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.')
                        {
                            if (t_id == 0) s1 += t_s[j];
                            else if (t_id == 1) s2 += t_s[j];
                            else s3 += t_s[j];
                        }
                        else if (s1.Length > 0 && t_s[j] == ' ' && t_s[j - 1] != ' ') 
                        {
                            if (t_id == 0) t_id = 1;
                            else if (t_id == 1) t_id = 2;
                        }
                    }                    
                    txtpred_3d_x[i].Text = s1;
                    txtpred_3d_y[i].Text = s2;
                    txtpred_3d_z[i].Text = s3;
                }
                reader.Close();
            }
            else
            {
                for (int i = 0; i < 16; i++) txtpred_3d_x[i].Text = txtpred_3d_y[i].Text = txtpred_3d_z[i].Text = "";
            }
            //end of pred_3d
        }

        private void Set2DLocation(int id, bool optimal)
        {
            //config
            string filename_config = txtdirectory.Text + "\\origin_data\\config_" + txtimageid.Text + ".txt";
            System.IO.StreamReader reader = new System.IO.StreamReader(filename_config, System.Text.Encoding.Default);
            double[] config = new double[13];
            for (int i = 0; i < 13; i++)
            {
                string t_s = reader.ReadLine();
                if (t_s == null) break;
                string s = "";
                for (int j = 0; j < t_s.Length; j++)
                {
                    if (t_s[j] == '-' || ((t_s[j] >= '0' && t_s[j] <= '9') && t_s[j-1]!='x' && t_s[j-1]!='y') || t_s[j] == '.' || t_s[j] == '-') 
                    {
                        s += t_s[j];
                    }
                }
                config[i]=Convert.ToDouble(s);
            }
            reader.Close();
            double crop_size = config[2], bbx_x1 = config[3], bbx_y1 = config[4];
            //end of config

            //gt_2d
            string filename_gt_2d = txtdirectory.Text + "\\origin_data\\gt_2d_" + txtimageid.Text + ".txt";
            reader = new System.IO.StreamReader(filename_gt_2d, System.Text.Encoding.Default);
            double[,] gt_2d = new double[16,2];
            TextBox[] txtgt_2d_x ={txtcurrentright_ankle_2d_origin_x, txtcurrentright_knee_2d_origin_x, txtcurrentright_hip_2d_origin_x, txtcurrentleft_hip_2d_origin_x,
                                txtcurrentleft_knee_2d_origin_x,txtcurrentleft_ankle_2d_origin_x,txtcurrentpelvis_2d_origin_x,txtcurrentthorax_2d_origin_x,txtcurrentupper_neck_2d_origin_x,
                                txtcurrenthead_top_2d_origin_x,txtcurrentright_wrist_2d_origin_x,txtcurrentright_elbow_2d_origin_x,txtcurrentright_shoulder_2d_origin_x,
                                txtcurrentleft_shoulder_2d_origin_x,txtcurrentleft_elbow_2d_origin_x,txtcurrentleft_wrist_2d_origin_x};
            TextBox[] txtgt_2d_y ={txtcurrentright_ankle_2d_origin_y, txtcurrentright_knee_2d_origin_y, txtcurrentright_hip_2d_origin_y, txtcurrentleft_hip_2d_origin_y,
                                txtcurrentleft_knee_2d_origin_y,txtcurrentleft_ankle_2d_origin_y,txtcurrentpelvis_2d_origin_y,txtcurrentthorax_2d_origin_y,txtcurrentupper_neck_2d_origin_y,
                                txtcurrenthead_top_2d_origin_y,txtcurrentright_wrist_2d_origin_y,txtcurrentright_elbow_2d_origin_y,txtcurrentright_shoulder_2d_origin_y,
                                txtcurrentleft_shoulder_2d_origin_y,txtcurrentleft_elbow_2d_origin_y,txtcurrentleft_wrist_2d_origin_y};
            for (int i = 0; i < 16; i++)
            {
                string t_s = reader.ReadLine();
                if (t_s == null) break;
                string s1 = "", s2 = "";
                int t_id = 0;
                for (int j = 0; j < t_s.Length; j++)
                {
                    if (t_s[j] == '-' || (t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.')
                    {
                        if (t_id == 0) s1 += t_s[j]; else s2 += t_s[j];
                    }
                    else if (s1.Length>0 && t_s[j]==' ' && t_s[j-1]!=' ') t_id = 1;
                }
                gt_2d[i, 0]=Convert.ToDouble(s1);
                gt_2d[i, 1]=Convert.ToDouble(s2);
                gt_2d[i, 0] = (gt_2d[i, 0] * crop_size - bbx_x1) / crop_size * 224.0;
                gt_2d[i, 1] = (gt_2d[i, 1] * crop_size - bbx_y1) / crop_size * 224.0;
                txtgt_2d_x[i].Text = Convert.ToString(gt_2d[i, 0]);
                txtgt_2d_y[i].Text = Convert.ToString(gt_2d[i, 1]);
            }
            reader.Close();
            //end of gt_2d

            //mod_2d
            TextBox[] txtmod_2d_x ={
                                 txtcurrentright_ankle_2d_modified_x, txtcurrentright_knee_2d_modified_x, txtcurrentright_hip_2d_modified_x, txtcurrentleft_hip_2d_modified_x, txtcurrentleft_knee_2d_modified_x,
                                 txtcurrentleft_ankle_2d_modified_x,txtcurrentpelvis_2d_modified_x,   txtcurrentthorax_2d_modified_x,txtcurrentupper_neck_2d_modified_x,txtcurrenthead_top_2d_modified_x,
                                 txtcurrentright_wrist_2d_modified_x,txtcurrentright_elbow_2d_modified_x,txtcurrentright_shoulder_2d_modified_x,txtcurrentleft_shoulder_2d_modified_x,txtcurrentleft_elbow_2d_modified_x,txtcurrentleft_wrist_2d_modified_x
                             };
            TextBox[] txtmod_2d_y ={
                                 txtcurrentright_ankle_2d_modified_y, txtcurrentright_knee_2d_modified_y, txtcurrentright_hip_2d_modified_y, txtcurrentleft_hip_2d_modified_y, txtcurrentleft_knee_2d_modified_y,
                                 txtcurrentleft_ankle_2d_modified_y,txtcurrentpelvis_2d_modified_y,   txtcurrentthorax_2d_modified_y,txtcurrentupper_neck_2d_modified_y,txtcurrenthead_top_2d_modified_y,
                                 txtcurrentright_wrist_2d_modified_y,txtcurrentright_elbow_2d_modified_y,txtcurrentright_shoulder_2d_modified_y,txtcurrentleft_shoulder_2d_modified_y,txtcurrentleft_elbow_2d_modified_y,txtcurrentleft_wrist_2d_modified_y
                             };
            double[,] mod_2d = new double[16, 2];
            string filename_modified_2d;
            if (!optimal) filename_modified_2d = txtdirectory.Text + "\\intermediate\\mod_" + txtimageid.Text + "_" + Convert.ToString(id) + ".txt";
            else
            {
                filename_modified_2d = txtdirectory.Text + "\\gt_2d_modified\\mod_" + txtimageid.Text + ".txt";
            }
            if (File.Exists(filename_modified_2d))
            {
                reader = new System.IO.StreamReader(filename_modified_2d, System.Text.Encoding.Default);
                for (int i = 0; i < 16; i++)
                {
                    string t_s = reader.ReadLine();
                    if (t_s == null) break;
                    string s1 = "", s2 = "";
                    int t_id = 0;
                    for (int j = 0; j < t_s.Length; j++)
                    {
                        if (t_s[j] == '-' || (t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.')
                        {
                            if (t_id == 0) s1 += t_s[j]; else s2 += t_s[j];
                        }
                        else if (s1.Length > 0 && t_s[j] == ' ' && t_s[j-1]!=' ') t_id = 1;
                    }
                    mod_2d[i, 0] = Convert.ToDouble(s1);
                    mod_2d[i, 1] = Convert.ToDouble(s2);
                    mod_2d[i, 0] = (mod_2d[i, 0] * crop_size - bbx_x1) / crop_size * 224.0;
                    mod_2d[i, 1] = (mod_2d[i, 1] * crop_size - bbx_y1) / crop_size * 224.0;
                    txtmod_2d_x[i].Text = Convert.ToString(mod_2d[i, 0]);
                    txtmod_2d_y[i].Text = Convert.ToString(mod_2d[i, 1]);
                }
                reader.Close();
            }
            else
            {
                for (int i = 0; i < 16; i++) txtmod_2d_x[i].Text = txtmod_2d_y[i].Text = "";
            }
            //end of mod_2d

            //pred_2d
            TextBox[] txtpred_2d_x ={
                                 txtcurrentright_ankle_2d_pred_x, txtcurrentright_knee_2d_pred_x, txtcurrentright_hip_2d_pred_x, txtcurrentleft_hip_2d_pred_x, txtcurrentleft_knee_2d_pred_x,
                                 txtcurrentleft_ankle_2d_pred_x,txtcurrentpelvis_2d_pred_x,   txtcurrentthorax_2d_pred_x,txtcurrentupper_neck_2d_pred_x,txtcurrenthead_top_2d_pred_x,
                                 txtcurrentright_wrist_2d_pred_x,txtcurrentright_elbow_2d_pred_x,txtcurrentright_shoulder_2d_pred_x,txtcurrentleft_shoulder_2d_pred_x,txtcurrentleft_elbow_2d_pred_x,txtcurrentleft_wrist_2d_pred_x
                             };
            TextBox[] txtpred_2d_y ={
                                 txtcurrentright_ankle_2d_pred_y, txtcurrentright_knee_2d_pred_y, txtcurrentright_hip_2d_pred_y, txtcurrentleft_hip_2d_pred_y, txtcurrentleft_knee_2d_pred_y,
                                 txtcurrentleft_ankle_2d_pred_y,txtcurrentpelvis_2d_pred_y,   txtcurrentthorax_2d_pred_y,txtcurrentupper_neck_2d_pred_y,txtcurrenthead_top_2d_pred_y,
                                 txtcurrentright_wrist_2d_pred_y,txtcurrentright_elbow_2d_pred_y,txtcurrentright_shoulder_2d_pred_y,txtcurrentleft_shoulder_2d_pred_y,txtcurrentleft_elbow_2d_pred_y,txtcurrentleft_wrist_2d_pred_y
                             };
            double[,] pred_2d = new double[16, 2];
            string filename_pred_2d;
            if (!optimal) filename_pred_2d = txtdirectory.Text + "\\intermediate\\pred_2d_" + txtimageid.Text + "_" + Convert.ToString(id) + ".txt";
            else
            {
                filename_pred_2d = txtdirectory.Text + "\\result\\pred_2d_" + txtimageid.Text + ".txt";                
            }
            if (File.Exists(filename_pred_2d))
            {
                reader = new System.IO.StreamReader(filename_pred_2d, System.Text.Encoding.Default);
                for (int i = 0; i < 16; i++)
                {
                    string t_s = reader.ReadLine();
                    if (t_s == null) break;
                    string s1 = "", s2 = "";
                    int t_id = 0;
                    for (int j = 0; j < t_s.Length; j++)
                    {
                        if (t_s[j] == '-' || (t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.')
                        {
                            if (t_id == 0) s1 += t_s[j]; else s2 += t_s[j];
                        }
                        else if (s1.Length > 0 && t_s[j] == ' ' && t_s[j-1]!=' ') t_id = 1;
                    }
                    pred_2d[i, 0] = Convert.ToDouble(s1);
                    pred_2d[i, 1] = Convert.ToDouble(s2);
                    pred_2d[i, 0] = pred_2d[i, 0] * 224.0;
                    pred_2d[i, 1] = pred_2d[i, 1] * 224.0;
                    txtpred_2d_x[i].Text = Convert.ToString(pred_2d[i, 0]);
                    txtpred_2d_y[i].Text = Convert.ToString(pred_2d[i, 1]);
                }
                reader.Close();
            }
            else
            {
                for (int i = 0; i < 16; i++) txtpred_2d_x[i].Text = txtpred_2d_y[i].Text = "";
            }
            //end of pred_2d
        }

        private void btnnavigate_Click(object sender, EventArgs e)
        {
           
        }

        private void btnreplace_Click(object sender, EventArgs e)
        {
            if (lstbackupid.SelectedItems.Count == 0) return;
            int t_id = Convert.ToInt32(lstbackupid.Items[lstbackupid.SelectedIndices[0]].Text);
            string bonelen_src = txtdirectory.Text + "\\intermediate\\bonelen_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            string bonelen_dst = txtdirectory.Text + "\\configuration\\bonelen_" + txtimageid.Text + ".in";
            if (File.Exists(bonelen_dst)) File.Delete(bonelen_dst);            
            File.Copy(bonelen_src, bonelen_dst);

            string mod_2d_src = txtdirectory.Text + "\\intermediate\\mod_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            string mod_2d_dst = txtdirectory.Text + "\\gt_2d_modified\\mod_" + txtimageid.Text + ".txt";
            if (File.Exists(mod_2d_src))
            {
                if (File.Exists(mod_2d_dst)) File.Delete(mod_2d_dst);
                File.Copy(mod_2d_src, mod_2d_dst);
            }
            string pred_2d_src = txtdirectory.Text + "\\intermediate\\pred_2d_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            string pred_2d_dst = txtdirectory.Text + "\\result\\pred_2d_" + txtimageid.Text + ".txt";
            if (File.Exists(pred_2d_dst)) File.Delete(pred_2d_dst);
            File.Copy(pred_2d_src, pred_2d_dst);

            string pred_3d_src = txtdirectory.Text + "\\intermediate\\pred_3d_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            string pred_3d_dst = txtdirectory.Text + "\\result\\pred_3d_" + txtimageid.Text + ".txt";
            if (File.Exists(pred_3d_dst)) File.Delete(pred_3d_dst);
            File.Copy(pred_3d_src, pred_3d_dst);

            string islimited_src = txtdirectory.Text + "\\intermediate\\islimited_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            string islimited_dst = txtdirectory.Text + "\\configuration\\islimited_" + txtimageid.Text + ".in";
            if (File.Exists(islimited_dst)) File.Delete(islimited_dst);
            File.Copy(islimited_src, islimited_dst);

            string initparam_src = txtdirectory.Text + "\\intermediate\\initparam_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            string initparam_dst = txtdirectory.Text + "\\configuration\\initparam_" + txtimageid.Text + ".in";
            if (File.Exists(initparam_dst)) File.Delete(initparam_dst);
            File.Copy(initparam_src, initparam_dst);

            string param_src = txtdirectory.Text + "\\intermediate\\param_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            string param_dst = txtdirectory.Text + "\\configuration\\param_" + txtimageid.Text + ".txt";
            if (File.Exists(param_dst)) File.Delete(param_dst);
            File.Copy(param_src, param_dst);

            string inobj_src = txtdirectory.Text + "\\intermediate\\inobj_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            string inobj_dst = txtdirectory.Text + "\\configuration\\inobj_" + txtimageid.Text + ".in";
            if (File.Exists(inobj_dst)) File.Delete(inobj_dst);
            File.Copy(inobj_src, inobj_dst);

            string angleconstraint_src = txtdirectory.Text + "\\intermediate\\angleconstraint_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            string angleconstraint_dst = txtdirectory.Text + "\\configuration\\angleconstraint_" + txtimageid.Text + ".in";
            if (File.Exists(angleconstraint_dst)) File.Delete(angleconstraint_dst);
            File.Copy(angleconstraint_src, angleconstraint_dst);
            lblnotifications.Text = "Finish Replacing Current Optimal(result folder)";
        }

        private void btndisplayoptimal_Click(object sender, EventArgs e)
        {            
            SetBoneLength(0, true, false);
            Set2DLocation(Convert.ToInt32(txtimageid.Text), true);
            Set3DLocation(0, true);
            SetLimitDoF(0, true, false);
            SetInitParam(0, true, false);
            SetParam(0, true);
            SetInObj(0, true, false);
            SetAngleConstraint(0, true, false);
            lblnotifications.Text = "Finish Displaying Current Optimal";
        }

        private void lstbackupid_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lstbackupid.SelectedIndices.Count == 0) return;
            ListViewItem lviitem = lstbackupid.SelectedItems[0];
            txtinterpretation.Text = lviitem.SubItems[1].Text;            
            lblnotifications.Text = "Coming to Navigate to " + lstbackupid.Items[lstbackupid.SelectedIndices[0]].Text;
            int t_id = Convert.ToInt32(lstbackupid.Items[lstbackupid.SelectedIndices[0]].Text);
            SetBoneLength(t_id, false, false);
            Set2DLocation(t_id, false);
            Set3DLocation(t_id, false);
            SetLimitDoF(t_id, false, false);
            SetInitParam(t_id, false, false);
            SetParam(t_id, false);
            SetInObj(t_id, false, false);
            SetAngleConstraint(t_id, false, false);
            lblnotifications.Text = "Finish Navigating to " + Convert.ToString(t_id);
        }

        private void txtinterpretation_TextChanged(object sender, EventArgs e)
        {
            int t_id = Convert.ToInt32(lstbackupid.Items[lstbackupid.SelectedIndices[0]].Text);
            string filename_interpretation = txtdirectory.Text + "\\intermediate\\interpretation_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            using (StreamWriter sw = File.CreateText(filename_interpretation))
            {
                sw.WriteLine(txtinterpretation.Text);
                sw.Close();
            }
            ListViewItem lviitem = lstbackupid.SelectedItems[0];
            lviitem.SubItems[1].Text = txtinterpretation.Text;
        }

        private void btnloaddefault_Click(object sender, EventArgs e)
        {
            SetBoneLength(0, true, true);
            SetLimitDoF(0, true, true);
            SetInitParam(0, true, true);
            SetInObj(0, true, true);
            SetAngleConstraint(0, true, true);
            lblnotifications.Text = "Finish Loading Optimal As Base Parameter Settings";
        }

        private void btndeletebackup_Click(object sender, EventArgs e)
        {
            if (lstbackupid.SelectedIndices.Count == 0) return;
            int t_id = Convert.ToInt32(lstbackupid.Items[lstbackupid.SelectedIndices[0]].Text);
            string bonelen_src = txtdirectory.Text + "\\intermediate\\bonelen_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            if (File.Exists(bonelen_src)) File.Delete(bonelen_src);

            string mod_2d_src = txtdirectory.Text + "\\intermediate\\mod_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            if (File.Exists(mod_2d_src)) File.Delete(mod_2d_src);

            string pred_2d_src = txtdirectory.Text + "\\intermediate\\pred_2d_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            if (File.Exists(pred_2d_src)) File.Delete(pred_2d_src);

            string pred_3d_src = txtdirectory.Text + "\\intermediate\\pred_3d_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            if (File.Exists(pred_3d_src)) File.Delete(pred_3d_src);

            string islimited_src = txtdirectory.Text + "\\intermediate\\islimited_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            if (File.Exists(islimited_src)) File.Delete(islimited_src);

            string initparam_src = txtdirectory.Text + "\\intermediate\\initparam_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            if (File.Exists(initparam_src)) File.Delete(initparam_src);

            string param_src = txtdirectory.Text + "\\intermediate\\param_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            if (File.Exists(param_src)) File.Delete(param_src);

            string inobj_src = txtdirectory.Text + "\\intermediate\\inobj_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            if (File.Exists(inobj_src)) File.Delete(inobj_src);

            string angleconstraint_src = txtdirectory.Text + "\\intermediate\\angleconstraint_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".in";
            if (File.Exists(angleconstraint_src)) File.Delete(angleconstraint_src);

            string interpretation_src = txtdirectory.Text + "\\intermediate\\interpretation_" + txtimageid.Text + "_" + Convert.ToString(t_id) + ".txt";
            if (File.Exists(interpretation_src)) File.Delete(interpretation_src);

            lblnotifications.Text = "Finish Removing Selected Backup";
        }

        private void btnloaduncertain_Click(object sender, EventArgs e)
        {
            RadioButton[] radio_uncertain =
            {
                radioleftelbow_uncertain,radiorightelbow_uncertain,                
                radioleftknee_uncertain,radiorightknee_uncertain,                
            };
            for (int i = 0; i < 4; i++) radio_uncertain[i].Checked = true;
            txtleftelbowrotxlb.Text = "-120"; txtleftelbowrotxub.Text = "0";
            txtrightelbowrotxlb.Text = "-120"; txtrightelbowrotxub.Text = "0";
            txtleftkneerotxlb.Text = "0"; txtleftkneerotxub.Text = "100";
            txtrightkneerotxlb.Text = "0"; txtrightkneerotxub.Text = "100";
            txtleftshoulderrotxlb.Text = "-180"; txtleftshoulderrotxub.Text = "30";
            txtrightshoulderrotxlb.Text = "-180"; txtrightshoulderrotxub.Text = "30";
            txtlefthiprotxlb.Text = "-90"; txtlefthiprotxub.Text = "90";
            txtrighthiprotxlb.Text = "-90"; txtrighthiprotxub.Text = "90";
            lblnotifications.Text = "Front & Back relationships are uncertain";
            chkenableleftelbowrotx.Checked = chkenablelefthiprotx.Checked = chkenableleftkneerotx.Checked = chkenableleftshoulderrotx.Checked = chkenablerightelbowrotx.Checked = chkenablerighthiprotx.Checked = chkenablerightkneerotx.Checked = chkenablerightshoulderrotx.Checked = false;
        }

        private void btnsetfrontback_Click(object sender, EventArgs e)
        {
            double[] t_angle_lb = new double[34];
            double[] t_angle_ub = new double[34];
            string filename = txtdirectory.Text + "\\configuration\\angleconstraint_" + txtimageid.Text + ".in";
            if (File.Exists(filename))
            {
                System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
                for (int i = 0; i < 34; i++)
                {
                    string t_s = reader.ReadLine();
                    if (t_s == null) break;
                    string s1 = "", s2 = "";
                    int t_id = 0;
                    for (int j = 0; j < t_s.Length; j++)
                    {
                        if (t_s[j] == '-' || (t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.')
                        {
                            if (t_id == 0) s1 += t_s[j]; else s2 += t_s[j];
                        }
                        else if (s1.Length > 0 && t_s[j] == ' ') t_id = 1;
                    }
                    t_angle_lb[i] = Convert.ToDouble(s1);
                    t_angle_ub[i] = Convert.ToDouble(s2);
                }
                reader.Close();
            }
            
            for (int i = 0; i < 34; i++)
            {
                t_angle_lb[i] = Convert.ToDouble(angle_lower[i]);
                t_angle_ub[i] = Convert.ToDouble(angle_upper[i]);
            }
            //left_shoulder_rot_x
            if (radioleftelbow_front.Checked == true)
            {
                t_angle_lb[22] = -180.0; t_angle_ub[22] = 0.0;
            }
            else if (radioleftelbow_back.Checked == true)
            {
                t_angle_lb[22] = 0.0; t_angle_ub[22] = 30.0;
            }
            //right_shoulder_rot_x
            if (radiorightelbow_front.Checked == true)
            {
                t_angle_lb[19] = -180.0; t_angle_ub[19] = 0.0;
            }
            else if (radiorightelbow_back.Checked == true)
            {
                t_angle_lb[19] = 0.0; t_angle_ub[19] = 30.0;
            }
            //left_hip_rot_x
            if (radioleftknee_front.Checked == true)
            {
                t_angle_lb[10] = -90.0; t_angle_ub[10] = 0.0;
            }
            else if (radioleftknee_back.Checked == true)
            {
                t_angle_lb[10] = 0.0; t_angle_ub[10] = 90.0;
            }
            //right_hip_rot_x
            if (radiorightknee_front.Checked == true)
            {
                t_angle_lb[7] = -90.0; t_angle_ub[7] = 0.0;
            }
            else if (radiorightknee_back.Checked == true)
            {
                t_angle_lb[7] = 0.0; t_angle_ub[7] = 90.0;
            }
            //left_elbow_rot_x
            if (chkenableleftelbowrotx.Checked == true)
            {                
                t_angle_lb[25] = Convert.ToDouble(txtleftelbowrotxlb.Text);
                t_angle_ub[25] = Convert.ToDouble(txtleftelbowrotxub.Text);
            }            
            //right_elbow_rot_x
            if (chkenablerightelbowrotx.Checked == true)
            {
                t_angle_lb[18] = Convert.ToDouble(txtrightelbowrotxlb.Text);
                t_angle_ub[18] = Convert.ToDouble(txtrightelbowrotxub.Text);
            }            
            //left_knee_rot_x
            if (chkenableleftkneerotx.Checked == true)
            {
                t_angle_lb[13] = Convert.ToDouble(txtleftkneerotxlb.Text);
                t_angle_ub[13] = Convert.ToDouble(txtleftkneerotxub.Text);
            }            
            //right_knee_rot_x
            if (chkenablerightkneerotx.Checked == true)
            {
                t_angle_lb[6] = Convert.ToDouble(txtrightkneerotxlb.Text);
                t_angle_ub[6] = Convert.ToDouble(txtrightkneerotxub.Text);
            }            
            //left_shoulder_rot_x
            if (chkenableleftshoulderrotx.Checked == true)
            {
                t_angle_lb[22] = Convert.ToDouble(txtleftshoulderrotxlb.Text);
                t_angle_ub[22] = Convert.ToDouble(txtleftshoulderrotxub.Text);
            }            
            //right_shoulder_rot_x
            if (chkenablerightshoulderrotx.Checked == true)
            {
                t_angle_lb[19] = Convert.ToDouble(txtrightshoulderrotxlb.Text);
                t_angle_ub[19] = Convert.ToDouble(txtrightshoulderrotxub.Text);
            }            
            //left_hip_rot_x
            if (chkenablelefthiprotx.Checked == true)
            {
                t_angle_lb[10] = Convert.ToDouble(txtlefthiprotxlb.Text);
                t_angle_ub[10] = Convert.ToDouble(txtlefthiprotxub.Text);
            }            
            //right_hip_rot_x
            if (chkenablerighthiprotx.Checked == true)
            {
                t_angle_lb[7] = Convert.ToDouble(txtrighthiprotxlb.Text);
                t_angle_ub[7] = Convert.ToDouble(txtrighthiprotxub.Text);
            }            
            filename = txtdirectory.Text + "\\configuration\\angleconstraint_" + txtimageid.Text + ".in";
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < 34; i++)
                {
                    for (int j = 0; j < 34 - dofname[i].Length; j++) sw.Write(" ");
                    sw.Write(dofname[i]);
                    sw.Write(" ");
                    sw.Write(Convert.ToString(t_angle_lb[i]).PadLeft(10) + "     " + Convert.ToString(t_angle_ub[i]).PadLeft(10));
                    sw.Write("\n");
                }
                sw.Close();
            }
            lblnotifications.Text = "Set Angle Constraint to Current Image Done";
        }

        private void btnloadangleconstraint_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\angleconstraint_" + txtimageid.Text + ".in";
            if (File.Exists(filename))
            {
                TextBox[] txtangle_lb =
                {
                    txtglobal_trans_x_lb,txtglobal_trans_y_lb,txtglobal_trans_z_lb,txtglobal_rot_x_lb,txtglobal_rot_y_lb,txtglobal_rot_z_lb,
                    txtright_knee_rot_x_lb,txtright_hip_rot_x_lb,txtright_hip_rot_y_lb,txtright_hip_rot_z_lb,txtleft_hip_rot_x_lb,txtleft_hip_rot_y_lb,txtleft_hip_rot_z_lb,
                    txtleft_knee_rot_x_lb,txtupper_neck_rot_x_lb,txtupper_neck_rot_y_lb,txtupper_neck_rot_z_lb,txthead_top_rot_x_lb,txtright_elbow_rot_x_lb,txtright_shoulder_rot_x_lb,txtright_shoulder_rot_y_lb,txtright_shoulder_rot_z_lb,
                    txtleft_shoulder_rot_x_lb,txtleft_shoulder_rot_y_lb,txtleft_shoulder_rot_z_lb,txtleft_elbow_rot_x_lb,txthip_rot_y_lb,txthip_rot_z_lb,
                    txtpers_global_rot_x_lb,txtpers_global_rot_y_lb,txtpers_global_rot_z_lb,txtpers_global_trans_x_lb,txtpers_global_trans_y_lb,txtpers_scale_lb
                };
                TextBox[] txtangle_ub =
                {
                    txtglobal_trans_x_ub,txtglobal_trans_y_ub,txtglobal_trans_z_ub,txtglobal_rot_x_ub,txtglobal_rot_y_ub,txtglobal_rot_z_ub,
                    txtright_knee_rot_x_ub,txtright_hip_rot_x_ub,txtright_hip_rot_y_ub,txtright_hip_rot_z_ub,txtleft_hip_rot_x_ub,txtleft_hip_rot_y_ub,txtleft_hip_rot_z_ub,
                    txtleft_knee_rot_x_ub,txtupper_neck_rot_x_ub,txtupper_neck_rot_y_ub,txtupper_neck_rot_z_ub,txthead_top_rot_x_ub,txtright_elbow_rot_x_ub,txtright_shoulder_rot_x_ub,txtright_shoulder_rot_y_ub,txtright_shoulder_rot_z_ub,
                    txtleft_shoulder_rot_x_ub,txtleft_shoulder_rot_y_ub,txtleft_shoulder_rot_z_ub,txtleft_elbow_rot_x_ub,txthip_rot_y_ub,txthip_rot_z_ub,
                    txtpers_global_rot_x_ub,txtpers_global_rot_y_ub,txtpers_global_rot_z_ub,txtpers_global_trans_x_ub,txtpers_global_trans_y_ub,txtpers_scale_ub
                };
                System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
                for (int i = 0; i < 34; i++)
                {
                    string t_s = reader.ReadLine();
                    if (t_s == null) break;
                    string s1 = "", s2 = "";
                    int t_id = 0;
                    for (int j = 0; j < t_s.Length; j++)
                    {
                        if (t_s[j] == '-' || (t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.')
                        {
                            if (t_id == 0) s1 += t_s[j]; else s2 += t_s[j];
                        }
                        else if (s1.Length > 0 && t_s[j] == ' ') t_id = 1;
                    }
                    txtangle_lb[i].Text = s1;
                    txtangle_ub[i].Text = s2;
                }
                reader.Close();
                lblnotifications.Text = "Finish Loading Angle Constraint File";
            
            }
        }

        private void btnloadangleconstraintdetail_Click(object sender, EventArgs e)
        {
            string filename = txtdirectory.Text + "\\configuration\\angleconstraint_" + txtimageid.Text + ".in";
            
            if (File.Exists(filename))
            {               
                System.IO.StreamReader reader = new System.IO.StreamReader(filename, System.Text.Encoding.Default);
                for (int i = 0; i < 34; i++)
                {
                    string t_s = reader.ReadLine();
                    if (t_s == null) break;
                    string s1 = "", s2 = "";
                    int t_id = 0;
                    for (int j = 0; j < t_s.Length; j++)
                    {
                        if (t_s[j] == '-' || (t_s[j] >= '0' && t_s[j] <= '9') || t_s[j] == '.')
                        {
                            if (t_id == 0) s1 += t_s[j]; else s2 += t_s[j];
                        }
                        else if (s1.Length > 0 && t_s[j] == ' ') t_id = 1;
                    }
                    angle_lower[i] = s1;
                    angle_upper[i] = s2;
                }
                reader.Close();
                txtleftelbowrotxlb.Text = angle_lower[25]; txtleftelbowrotxub.Text = angle_upper[25];
                txtrightelbowrotxlb.Text = angle_lower[18]; txtrightelbowrotxub.Text = angle_upper[18];
                txtleftkneerotxlb.Text = angle_lower[13]; txtleftkneerotxub.Text = angle_upper[13];
                txtrightkneerotxlb.Text = angle_lower[6]; txtrightkneerotxub.Text = angle_upper[6];
                txtleftshoulderrotxlb.Text = angle_lower[22]; txtleftshoulderrotxub.Text = angle_upper[22];
                txtrightshoulderrotxlb.Text = angle_lower[19]; txtrightshoulderrotxub.Text = angle_upper[19];
                txtlefthiprotxlb.Text = angle_lower[10]; txtlefthiprotxub.Text = angle_upper[10];
                txtrighthiprotxlb.Text = angle_lower[7]; txtrighthiprotxub.Text = angle_upper[7];
            }
        }
    }
}
