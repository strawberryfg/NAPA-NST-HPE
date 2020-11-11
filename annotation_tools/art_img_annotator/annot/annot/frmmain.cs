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
namespace annot
{

    public partial class frmannot : Form
    {
        int cur_jt = 0;
        int joint_num = 18;
        int[] l_jt = { 4, 5, 6, 0, 13, 12, 11, 17, 8, 10 };
        bool[] is_l_jt = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
        Image cur_img;
        bool draw_head = false;
        bool draw_head_2 = false;
        int corner_pt_id = 0;
        double h_x = 0.0, h_y = 0.0;
        double head_bbx_x1 = 0, head_bbx_y1 = 0, head_bbx_x2 = 0, head_bbx_y2 = 0;
        int[,] skeleton = {
            { 0, 1 },
            { 1, 2 },
            { 2, 3 },
            { 0, 4 },
            { 4, 5 },
            { 5, 6 },
            { 0, 17 },
            { 17, 14 },
            { 14, 15 },
            { 15, 16 },
            { 17, 11 },
            { 11, 12 },
            { 12, 13 },
            { 17, 8 },
            {8,10 }};
        double[,] jt_loc = new double [,]{{ 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0},
                            { 0.0,0.0} };
        public frmannot()
        {
            InitializeComponent();
        }

        private void pic_Click(object sender, EventArgs e)
        {
            pic.Focus();

        }


        private void load_img(string filepath)
        {
            for (int j = 0; j < joint_num; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    jt_loc[j, k] = 0;
                }
            }
            string annotname = "C:/Users/Alienware 15 R3/Documents/nst/datasets/testset/annot/" + filepath + ".txt";
            if (File.Exists(annotname))
            {
                StreamReader reader = new System.IO.StreamReader(annotname, System.Text.Encoding.Default);
                for (int i = 0; i < joint_num; i++)
                {
                    string t_s = reader.ReadLine();
                    var str = t_s.Split(' ');
                    double[] jt = str.Select(System.Double.Parse).ToArray();
                    jt_loc[i, 0] = jt[0];
                    jt_loc[i, 1] = jt[1];
                    //Console.WriteLine(jt_loc[i, 0].ToString() + " " + jt_loc[i, 1].ToString() + "\n");
                }
                string last_line = reader.ReadLine();
                var str_last_line = last_line.Split(' ');
                double[] head_bbx = str_last_line.Select(System.Double.Parse).ToArray();
                head_bbx_x1 = head_bbx[0];
                head_bbx_y1 = head_bbx[1];
                head_bbx_x2 = head_bbx[2];
                head_bbx_y2 = head_bbx[3];

                reader.Close();
            }

            string filename = "C:/Users/Alienware 15 R3/Documents/nst/datasets/testset/" + filepath + ".png";
            if (File.Exists(filename))
            {
                pic.Load(filename);
                cur_img = pic.Image;
            }

            showAnnot();

            /*for (int i = 0; i < joint_num; i++)
            {
                txttmp.Text = txttmp.Text + jt_loc[i, 0].ToString() + " " + jt_loc[i, 1].ToString() + "\n";
            }*/
        }

        private void btnup_Click(object sender, EventArgs e)
        {
            string tmp_text = (Int32.Parse(txtcur.Text) + 1).ToString();
            txtcur.Text = tmp_text;
            load_img(txtcur.Text);
        }

        private void txtcur_KeyDown(object sender, KeyEventArgs e)
        {
            load_img(txtcur.Text);
        }

        private void frmannot_Load(object sender, EventArgs e)
        {
            for (int i = 0; i < l_jt.Length; i++)
            {
                is_l_jt[l_jt[i]] = true;
            }
            string filename = "C:/Users/Alienware 15 R3/Documents/nst/datasets/testset/" + txtcur.Text + ".png";
            if (File.Exists(filename))
            {
                //pic.Load(filename);
                load_img(txtcur.Text);
                cur_img = pic.Image;
            }
            
        }

        private void btndown_Click(object sender, EventArgs e)
        {
            string tmp_text = (Int32.Parse(txtcur.Text) - 1).ToString();
            txtcur.Text = tmp_text;
            load_img(txtcur.Text);
        }

        private void frmannot_Click(object sender, EventArgs e)
        {
            draw_head = false;
            draw_head_2 = false;
        }

        private void frmannot_KeyPress(object sender, KeyPressEventArgs e)
        {

        }

        private void pic_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {

        }

        private void txt1_TextChanged(object sender, EventArgs e)
        {

        }

        private void txtcur_TextChanged(object sender, EventArgs e)
        {

        }

        private void pic_MouseHover(object sender, EventArgs e)
        {
            //MouseEventArgs me = (MouseEventArgs)e;
            //Point coordinates = me.Location;
            //lblloc.Text = "(" + coordinates.X.ToString() + "," + coordinates.Y.ToString() + ")";
        }

        private void pic_MouseDown(object sender, MouseEventArgs e)
        {
            //this.circle_bounds.Location = e.Location;
            if (draw_head)
            {
                h_x = e.X / (double)pic.Width * 224.0;
                h_y = e.Y / (double)pic.Height * 224.0;

                drawHead(e.X / (double)pic.Width * 224.0, e.Y / (double)pic.Height * 224.0);

                
            }
        }

        private void pic_MouseClick(object sender, MouseEventArgs e)
        {


        }


        //Hovering around mouse move
        private void drawHeadMove(double tx, double ty)
        {
            //Console.WriteLine(jt_loc[cur_jt, 0].ToString() + "\n");
            List<Bitmap> images = new List<Bitmap>();
            Bitmap finalImage = new Bitmap(600, 600);
            Bitmap ori = new Bitmap(cur_img, new Size(600, 600));

            Console.WriteLine(pic.Image.Width + " " + pic.Image.Height);
            images.Add(ori);
            pic.Width = 600;
            pic.Height = 600;

            int nSize = 20;

            //draw circles
            for (int i = 0; i < joint_num; i++)
            {
                Bitmap bmm = new Bitmap(600, 600);
                images.Add(bmm);

                //draw a circle
                using (Graphics gr = Graphics.FromImage(bmm))
                {
                    gr.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality;
                    int n_x = 0, n_y = 0;
                    n_x = Convert.ToInt32(jt_loc[i, 0] / 224.0 * (double)pic.Width);
                    n_y = Convert.ToInt32(jt_loc[i, 1] / 224.0 * (double)pic.Height);
                    if (n_x > 10 && n_y > 10)
                    {
                        if (is_l_jt[i])
                        {
                            gr.FillEllipse(System.Drawing.Brushes.Red, n_x, n_y, nSize, nSize);
                        }
                        else
                        {
                            gr.FillEllipse(System.Drawing.Brushes.Blue, Convert.ToInt32(jt_loc[i, 0] / 224.0 * (double)pic.Width), Convert.ToInt32(jt_loc[i, 1] / 224.0 * (double)pic.Height), nSize, nSize);
                        }
                    }
                }
            }

            //draw skeleton lines
            for (int i = 0; i < skeleton.GetLength(0); i++)
            {
                if (skeleton[i, 0] == 7 || skeleton[i, 1] == 7 || skeleton[i, 0] == 9 || skeleton[i, 1] == 9) continue;
                Bitmap bmm = new Bitmap(600, 600);
                images.Add(bmm);

                //draw a line
                using (Graphics gr = Graphics.FromImage(bmm))
                {
                    gr.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                    Pen cur_pen;
                    if (is_l_jt[skeleton[i, 0]] || is_l_jt[skeleton[i, 1]])
                    {
                        //red
                        cur_pen = new Pen(Color.Red, 10);
                    }
                    else
                    {
                        //blue
                        cur_pen = new Pen(Color.Blue, 10);
                    }

                    int u = skeleton[i, 0], v = skeleton[i, 1];
                    int u_x = Convert.ToInt32(jt_loc[u, 0] / 224.0 * (double)pic.Width);
                    int u_y = Convert.ToInt32(jt_loc[u, 1] / 224.0 * (double)pic.Height);
                    int v_x = Convert.ToInt32(jt_loc[v, 0] / 224.0 * (double)pic.Width);
                    int v_y = Convert.ToInt32(jt_loc[v, 1] / 224.0 * (double)pic.Height);
                    if (u_x > 10 && u_y > 10 && v_x > 10 && v_y > 10)
                    {
                        Console.WriteLine(i.ToString() + " " + u.ToString() + " " + v.ToString() + " " + u_x.ToString() + " " + u_y.ToString() + " " + v_x.ToString() + " " + v_y.ToString());
                        PointF u_pt = new PointF(u_x, u_y);
                        PointF v_pt = new PointF(v_x, v_y);
                        gr.DrawLine(cur_pen, u_pt, v_pt);

                    }

                }

            }

            //draw head 
            Pen selPen = new Pen(Color.Green, 10);
            
            Bitmap bm = new Bitmap(600, 600);
            images.Add(bm);

            //draw a line
            using (Graphics gr = Graphics.FromImage(bm))
            {
                int end_x = Convert.ToInt32(tx / 224.0 * (double)pic.Width);
                int end_y = Convert.ToInt32(ty / 224.0 * (double)pic.Height);

                int start_x = Convert.ToInt32(h_x / 224.0 * (double)pic.Width);
                int start_y = Convert.ToInt32(h_y / 224.0 * (double)pic.Height);


                gr.DrawRectangle(selPen, start_x, start_y, end_x-start_x, end_y-start_y);
                //Console.WriteLine(h_x.ToString() + " " + h_y.ToString() + " " + tx.ToString() + " " + ty.ToString());
                head1.Text = "(" + Convert.ToInt32(h_x).ToString() + "," + Convert.ToInt32(h_y).ToString() + ")";
                head2.Text = "(" + Convert.ToInt32(tx).ToString() + "," + Convert.ToInt32(ty).ToString() + ")";
                head_bbx_x1 = h_x;
                head_bbx_y1 = h_y;
                head_bbx_x2 = tx;
                head_bbx_y2 = ty;
            }

            //draw final
            using (Graphics g = Graphics.FromImage(finalImage))
            {
                int offset = 0;
                foreach (Bitmap image in images)
                {
                    g.DrawImage(image, new Rectangle(offset, 0, image.Width, image.Height));
                }
            }
            pic.Image = finalImage;
            images.Clear();

        }

        //MouseDown press (a small rectangle bbx)
        private void drawHead(double tx, double ty)
        {
            //Console.WriteLine(jt_loc[cur_jt, 0].ToString() + "\n");
            List<Bitmap> images = new List<Bitmap>();
            Bitmap finalImage = new Bitmap(600, 600);
            Bitmap ori = new Bitmap(cur_img, new Size(600, 600));

            Console.WriteLine(pic.Image.Width + " " + pic.Image.Height);
            images.Add(ori);
            pic.Width = 600;
            pic.Height = 600;

            int nSize = 20;

            //draw circles
            for (int i = 0; i < joint_num; i++)
            {
                Bitmap bmm = new Bitmap(600, 600);
                images.Add(bmm);

                //draw a circle
                using (Graphics gr = Graphics.FromImage(bmm))
                {
                    gr.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality;
                    int n_x = 0, n_y = 0;
                    n_x = Convert.ToInt32(jt_loc[i, 0] / 224.0 * (double)pic.Width);
                    n_y = Convert.ToInt32(jt_loc[i, 1] / 224.0 * (double)pic.Height);
                    if (n_x > 10 && n_y > 10)
                    {
                        if (is_l_jt[i])
                        {
                            gr.FillEllipse(System.Drawing.Brushes.Red, n_x, n_y, nSize, nSize);
                        }
                        else
                        {
                            gr.FillEllipse(System.Drawing.Brushes.Blue, Convert.ToInt32(jt_loc[i, 0] / 224.0 * (double)pic.Width), Convert.ToInt32(jt_loc[i, 1] / 224.0 * (double)pic.Height), nSize, nSize);
                        }
                    }
                }
            }

            //draw skeleton lines
            for (int i = 0; i < skeleton.GetLength(0); i++)
            {
                if (skeleton[i, 0] == 7 || skeleton[i, 1] == 7 || skeleton[i, 0] == 9 || skeleton[i, 1] == 9) continue;
                Bitmap bmm = new Bitmap(600, 600);
                images.Add(bmm);

                //draw a line
                using (Graphics gr = Graphics.FromImage(bmm))
                {
                    gr.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                    Pen cur_pen;
                    if (is_l_jt[skeleton[i, 0]] || is_l_jt[skeleton[i, 1]])
                    {
                        //red
                        cur_pen = new Pen(Color.Red, 10);
                    }
                    else
                    {
                        //blue
                        cur_pen = new Pen(Color.Blue, 10);
                    }

                    int u = skeleton[i, 0], v = skeleton[i, 1];
                    int u_x = Convert.ToInt32(jt_loc[u, 0] / 224.0 * (double)pic.Width);
                    int u_y = Convert.ToInt32(jt_loc[u, 1] / 224.0 * (double)pic.Height);
                    int v_x = Convert.ToInt32(jt_loc[v, 0] / 224.0 * (double)pic.Width);
                    int v_y = Convert.ToInt32(jt_loc[v, 1] / 224.0 * (double)pic.Height);
                    if (u_x > 10 && u_y > 10 && v_x > 10 && v_y > 10)
                    {
                        Console.WriteLine(i.ToString() + " " + u.ToString() + " " + v.ToString() + " " + u_x.ToString() + " " + u_y.ToString() + " " + v_x.ToString() + " " + v_y.ToString());
                        PointF u_pt = new PointF(u_x, u_y);
                        PointF v_pt = new PointF(v_x, v_y);
                        gr.DrawLine(cur_pen, u_pt, v_pt);

                    }

                }

            }

            //draw head 
            Pen selPen = new Pen(Color.Green, 10);
            Bitmap bm = new Bitmap(600, 600);
            images.Add(bm);

            //draw a line
            using (Graphics gr = Graphics.FromImage(bm))
            {
                int u_x = Convert.ToInt32(tx / 224.0 * (double)pic.Width);
                int u_y = Convert.ToInt32(ty / 224.0 * (double)pic.Height);

                gr.DrawRectangle(selPen, u_x, u_y, 50, 10);

            }
            
            //draw final
            using (Graphics g = Graphics.FromImage(finalImage))
            {
                int offset = 0;
                foreach (Bitmap image in images)
                {
                    g.DrawImage(image, new Rectangle(offset, 0, image.Width, image.Height));
                }
            }
            pic.Image = finalImage;
            images.Clear();
           
        }

        private void showAnnot()
        {
            //Console.WriteLine(jt_loc[cur_jt, 0].ToString() + "\n");
            List<Bitmap> images = new List<Bitmap>();
            Bitmap finalImage = new Bitmap(600, 600);
            Bitmap ori = new Bitmap(cur_img, new Size(600, 600));

            Console.WriteLine(pic.Image.Width + " " + pic.Image.Height);
            images.Add(ori);
            pic.Width = 600;
            pic.Height = 600;

            int nSize = 20;

            //draw circles
            for (int i = 0; i < joint_num; i++)
            {
                Bitmap bm = new Bitmap(600, 600);
                images.Add(bm);

                //draw a circle
                using (Graphics gr = Graphics.FromImage(bm))
                {
                    gr.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality;
                    int n_x = 0, n_y = 0;
                    n_x = Convert.ToInt32(jt_loc[i, 0] / 224.0 * (double)pic.Width);
                    n_y = Convert.ToInt32(jt_loc[i, 1] / 224.0 * (double)pic.Height);
                    if (n_x > 10 && n_y > 10)
                    {
                        if (is_l_jt[i])
                        {
                            gr.FillEllipse(System.Drawing.Brushes.Red, n_x, n_y, nSize, nSize);
                        }
                        else
                        {
                            gr.FillEllipse(System.Drawing.Brushes.Blue, Convert.ToInt32(jt_loc[i, 0] / 224.0 * (double)pic.Width), Convert.ToInt32(jt_loc[i, 1] / 224.0 * (double)pic.Height), nSize, nSize);
                        }
                    }
                }
            }

            //draw skeleton lines
            for (int i = 0; i < skeleton.GetLength(0); i++)
            {
                if (skeleton[i, 0] == 7 || skeleton[i, 1] == 7 || skeleton[i, 0] == 9 || skeleton[i, 1] == 9) continue;
                Bitmap bm = new Bitmap(600, 600);
                images.Add(bm);

                //draw a line
                using (Graphics gr = Graphics.FromImage(bm))
                {
                    gr.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                    Pen cur_pen;
                    if (is_l_jt[skeleton[i, 0]] || is_l_jt[skeleton[i, 1]])
                    {
                        //red
                        cur_pen = new Pen(Color.Red, 10);
                    }
                    else
                    {
                        //blue
                        cur_pen = new Pen(Color.Blue, 10);
                    }

                    int u = skeleton[i, 0], v = skeleton[i, 1];
                    int u_x = Convert.ToInt32(jt_loc[u, 0] / 224.0 * (double)pic.Width);
                    int u_y = Convert.ToInt32(jt_loc[u, 1] / 224.0 * (double)pic.Height);
                    int v_x = Convert.ToInt32(jt_loc[v, 0] / 224.0 * (double)pic.Width);
                    int v_y = Convert.ToInt32(jt_loc[v, 1] / 224.0 * (double)pic.Height);
                    if (u_x > 10 && u_y > 10 && v_x > 10 && v_y > 10)
                    {
                        Console.WriteLine(i.ToString() + " " + u.ToString() + " " + v.ToString() + " " + u_x.ToString() + " " + u_y.ToString() + " " + v_x.ToString() + " " + v_y.ToString());
                        PointF u_pt = new PointF(u_x, u_y);
                        PointF v_pt = new PointF(v_x, v_y);
                        gr.DrawLine(cur_pen, u_pt, v_pt);

                    }

                }

            }

            //draw head bbx
            Pen selPen = new Pen(Color.Green, 10);

            Bitmap bmh = new Bitmap(600, 600);
            images.Add(bmh);

            //draw a line
            using (Graphics gr = Graphics.FromImage(bmh))
            {
                int end_x = Convert.ToInt32(head_bbx_x2 / 224.0 * (double)pic.Width);
                int end_y = Convert.ToInt32(head_bbx_y2 / 224.0 * (double)pic.Height);

                int start_x = Convert.ToInt32(head_bbx_x1 / 224.0 * (double)pic.Width);
                int start_y = Convert.ToInt32(head_bbx_y1 / 224.0 * (double)pic.Height);


                gr.DrawRectangle(selPen, start_x, start_y, end_x - start_x, end_y - start_y);
                //Console.WriteLine(h_x.ToString() + " " + h_y.ToString() + " " + tx.ToString() + " " + ty.ToString());
                head1.Text = "(" + Convert.ToInt32(head_bbx_x1).ToString() + "," + Convert.ToInt32(head_bbx_y1).ToString() + ")";
                head2.Text = "(" + Convert.ToInt32(head_bbx_x2).ToString() + "," + Convert.ToInt32(head_bbx_y2).ToString() + ")";
            }

            //draw final
            using (Graphics g = Graphics.FromImage(finalImage))
            {
                //set background color
                //g.Clear(Color.Black);

                //go through each image and draw it on the final image (Notice the offset; since I want to overlay the images i won't have any offset between the images in the finalImage)
                int offset = 0;
                foreach (Bitmap image in images)
                {
                    g.DrawImage(image, new Rectangle(offset, 0, image.Width, image.Height));
                }
            }
            pic.Image = finalImage;
            images.Clear();
            //pic.Image = bm;
        }

        private void pic_MouseUp(object sender, MouseEventArgs e)
        {
            if (draw_head)
            {
                draw_head = false;
            }
            else if (draw_head_2)
            {
                if (corner_pt_id == 0)
                {
                    h_x = e.X / (double)pic.Width * 224.0;
                    h_y = e.Y / (double)pic.Height * 224.0;
                }

                drawHead(e.X / (double)pic.Width * 224.0, e.Y / (double)pic.Height * 224.0);

                corner_pt_id += 1;
                if (corner_pt_id == 2)
                {
                    corner_pt_id = 0;
                    drawHeadMove(e.X / (double)pic.Width * 224.0, e.Y / (double)pic.Height * 224.0);

                    draw_head_2 = false;
                }
            }
            else
            {
                //set joint location (2d)
                jt_loc[cur_jt, 0] = e.X / (double)pic.Width * 224.0;
                jt_loc[cur_jt, 1] = e.Y / (double)pic.Height * 224.0;
                lblloc.Text = "(" + ((int)jt_loc[cur_jt, 0]).ToString() + "," + ((int)jt_loc[cur_jt, 1]).ToString() + ")";

                showAnnot();
            }

        }

        private void btn0_Click(object sender, EventArgs e)
        {
            cur_jt = 0;
            txtcurjt.Text = "0";
        }

        private void btn1_Click(object sender, EventArgs e)
        {
            cur_jt = 1;
            txtcurjt.Text = "1";
        }

        private void btn4_Click(object sender, EventArgs e)
        {
            cur_jt = 4;
            txtcurjt.Text = "4";
        }

        private void btn13_Click(object sender, EventArgs e)
        {
            cur_jt = 13;
            txtcurjt.Text = "13";
        }

        private void btn16_Click(object sender, EventArgs e)
        {
            cur_jt = 16;
            txtcurjt.Text = "16";
        }

        private void btn12_Click(object sender, EventArgs e)
        {
            cur_jt = 12;
            txtcurjt.Text = "12";
        }

        private void btn15_Click(object sender, EventArgs e)
        {
            cur_jt = 15;
            txtcurjt.Text = "15";
        }

        private void btn11_Click(object sender, EventArgs e)
        {
            cur_jt = 11;
            txtcurjt.Text = "11";
        }

        private void btn17_Click(object sender, EventArgs e)
        {
            cur_jt = 17;
            txtcurjt.Text = "17";
        }

        private void button3_Click(object sender, EventArgs e)
        {
            cur_jt = 14;
            txtcurjt.Text = "14";
        }

        private void btn8_Click(object sender, EventArgs e)
        {
            cur_jt = 8;
            txtcurjt.Text = "8";
        }

        private void btn10_Click(object sender, EventArgs e)
        {
            cur_jt = 10;
            txtcurjt.Text = "10";
        }

        private void btn2_Click(object sender, EventArgs e)
        {
            cur_jt = 2;
            txtcurjt.Text = "2";
        }

        private void btn3_Click(object sender, EventArgs e)
        {
            cur_jt = 3;
            txtcurjt.Text = "3";
        }

        private void btn6_Click(object sender, EventArgs e)
        {
            cur_jt = 6;
            txtcurjt.Text = "6";
        }

        private void btn5_Click(object sender, EventArgs e)
        {
            cur_jt = 5;
            txtcurjt.Text = "5";
        }

        private void pic_MouseMove(object sender, MouseEventArgs e)
        {
            double jt_loc_x = e.X / (double)pic.Width * 224.0;
            double jt_loc_y = e.Y / (double)pic.Height * 224.0;
            lblloc.Text = "(" + ((int)jt_loc_x).ToString() + "," + ((int)jt_loc_y).ToString() + ")";

            if (draw_head)
            {
                drawHeadMove(e.X / (double)pic.Width * 224.0, e.Y / (double)pic.Height * 224.0);

            }
        }

        private void btndone_Click(object sender, EventArgs e)
        {
            string filename = "C:/Users/Alienware 15 R3/Documents/nst/datasets/testset/annot/" + txtcur.Text + ".txt";
            using (StreamWriter sw = File.CreateText(filename))
            {
                for (int i = 0; i < joint_num; i++)
                {
                    sw.WriteLine(jt_loc[i,0].ToString()+" "+jt_loc[i,1].ToString()); 
                }
                sw.Write(head_bbx_x1.ToString() + " " + head_bbx_y1.ToString() + " " + head_bbx_x2.ToString() + " " + head_bbx_y2.ToString());
                sw.Close();
            }
        }

        private void btndraw2_Click(object sender, EventArgs e)
        {
            draw_head_2 = true;
        }

        private void btndraw_Click(object sender, EventArgs e)
        {
            draw_head = true;
        }
    }
}
