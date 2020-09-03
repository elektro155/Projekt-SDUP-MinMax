using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace WindowsFormsApplication1
{ 
    public partial class Form1 : Form
    {
        private bool received = false;

        private const int DEGREE_OF_A_POLYNOMINAL = 5;

        private int coefficientRequestIndex = 5;

        private bool coefficientsRequestValid = false;

        private bool nextCalculationPossibile = false;

        private String[] coefficients = new String[DEGREE_OF_A_POLYNOMINAL + 1];

        private const int ERR_OK = 7;

        public Form1() 
        {
            InitializeComponent();

            int retVal = getValuesFromGUI();

            if (retVal != ERR_OK)
            {
                textBox1.Text += "wrong value in coeff. field at power " + retVal + "\r\n";
            }
            else
            {
                //textBox1.Text += "Coefficients from fiels have been read\r\n";
            }
        }

        private void Form1_Load(object sender, EventArgs e) 
        {
            timer1.Start();
        }

        private void port_OnReceiveDatazz(object sender, SerialDataReceivedEventArgs e) 
        {
            received = true;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen && received == true)
            {
                System.Threading.Thread.Sleep(100);
                int a = serialPort1.BytesToRead;  //length of frame from uart
                byte[] data = new byte[a+1];      //array for data from uart
                serialPort1.Read(data, 0, a);     //reading to bit array          
                received = false;

                string readData = System.Text.Encoding.Default.GetString(data);
                coeffRequestGetValueAndState(readData);
                nextCalculationPossibility(readData);

                if (coefficientsRequestValid)
                {
                    //textBox1.Text += "Read at " + DateTime.Now.ToString() + " : " + System.Text.Encoding.Default.GetString(data) + "\r\n";
                    serialPort1.WriteLine(coefficients[coefficientRequestIndex]+"\r\n");
                    //textBox1.Text += "Sent at " + DateTime.Now.ToString() + " : " + coefficients[coefficientRequestIndex] + "\r\n";
                }
                else
                {
                    if (readData.Contains("Enter any value to continue"))
                    {
                        readData = readData.Remove(readData.IndexOf("Enter any value to continue")); //remove this text
                    }

                    textBox1.Text += readData + "\r\n";           
                }
            }
        }

        private void coeffRequestGetValueAndState(String s)
        {
            if(s.Contains("enter value for x^"))
            {
                coefficientRequestIndex = s[18]-'0';
                coefficientsRequestValid = true;

                if(3 == coefficientRequestIndex)
                {
                    drawPolynominal();
                }
            }
            else
            {
                coefficientsRequestValid = false;
            }
        }

        private void nextCalculationPossibility(String s)
        {
            if (s.Contains("Enter any value to continue"))
            {
                nextCalculationPossibile = true;
            }
        }

        private bool verifyValueOK(String s)
        {
            int value = int.Parse(s);
            if (value < 11 && value > -11) return true;
            else return false;
        }

        private String getTextBoxContent(int index) //index of power
        {
            String retVal = "";

            switch(index)
            {
                case 0:
                    retVal = textBox7.Text;
                    break;
                case 1:
                    retVal = textBox6.Text;
                    break;
                case 2:
                    retVal = textBox4.Text;
                    break;
                case 3:
                    retVal = textBox3.Text;
                    break;
                case 4:
                    retVal = textBox2.Text;
                    break;
                case 5:
                    retVal = textBox8.Text;
                    break;
                default:
                    break;
            }

            return retVal;
        }
        private int getValuesFromGUI()
        {
            int retVal = ERR_OK;

            for(int a = 0; a <= DEGREE_OF_A_POLYNOMINAL; a++ )
            {
                String value = getTextBoxContent(a);
                if (verifyValueOK(value))
                {
                    coefficients[a] = value;
                }
                else //value out of range
                {
                    retVal = a;
                    break;
                }
            }

            return retVal;
        }

        private void textBox1_TextChanged(object sender, EventArgs e) //auto scrolling to bottom
        {
            //auto scrolling to bottom
            if (textBox1.Visible)
            {
                textBox1.SelectionStart = textBox1.TextLength;
                textBox1.ScrollToCaret();
            }
        }

        private void calculate_Click(object sender, EventArgs e) //perform calculations
        {

            if (nextCalculationPossibile)
            {
                int retVal = getValuesFromGUI();  //update fields in case of changes made by the user
                if (retVal != ERR_OK)
                {
                    textBox1.Text += "wrong value in coeff. field at power " + retVal + "\r\n";
                    return;
                }
                else
                {
                    textBox1.Text += "Application: Sending polynominal to be calculated";
                    nextCalculationPossibile = false;
                    drawPolynominal();
                }
                                
                if (serialPort1.IsOpen)
                {
                    serialPort1.Write("997\r\n"); //trigger next cycle of calculations by sending a random number
                }
                else
                {
                    textBox1.Text += "PORT IS CLOSED, OPEN THE PORT\r\n";
                }
            }
            else
            {
                if (serialPort1.IsOpen)
                {
                    //board has started but request for coeff. for x^5 was missed therefore send it

                    int retVal = getValuesFromGUI();
                    if (retVal != ERR_OK)
                    {
                        textBox1.Text += "wrong value in coeff. field at power " + retVal + "\r\n";
                        return;
                    }
                    else
                    {
                        textBox1.Text += "Application: Sending polynominal to be calculated";
                        nextCalculationPossibile = false;
                        drawPolynominal();
                    }

                    serialPort1.WriteLine(coefficients[5] + "\r\n");
                }
                else
                {
                    textBox1.Text += "PORT IS CLOSED, OPEN THE PORT\r\n";
                }
            }
        }

        private void button1_Click(object sender, EventArgs e) //clear log
        {
            textBox1.Text = "";
        }       

        private void textBox5_TextChanged(object sender, EventArgs e) //port text box change detection
        {
            if (serialPort1.IsOpen) textBox1.Text += "port has been closed\r\n";
            serialPort1.Close();            
            string portNumber = textBox5.Text;
            serialPort1.PortName = "COM"+portNumber;

        }

        private void button2_Click(object sender, EventArgs e)  //open port
        {
            if (!serialPort1.IsOpen)
            {
                serialPort1.DataReceived += port_OnReceiveDatazz; //callback for uart
                try
                {
                    serialPort1.Open();
                }
                catch (Exception ex)
                {
                    textBox1.Text += "PORT COULD NOT BE OPENED\r\n";
                }
                if (serialPort1.IsOpen)
                {
                    textBox1.Text += "port has been opened\r\n";
                }
            }
        }

        private float F(float argument)
        {
            //Horner's method
            float retVal = (float)int.Parse(coefficients[5]);
            for (int a = 1; a <= (DEGREE_OF_A_POLYNOMINAL); a++)
            {
                float b = (float)int.Parse(coefficients[a]);
                retVal = retVal * argument + (float)int.Parse(coefficients[DEGREE_OF_A_POLYNOMINAL - a]);
            }
            return retVal;
        }

        //source: http://csharphelper.com/blog/2016/11/graph-an-equation-in-c/
        private void drawPolynominal()
        {
            label9.Visible = true;

            // The bounds to draw.
            float xmin = -20;
            float xmax = 20;
            float ymin = -20;
            float ymax = 20;

            // Make the Bitmap.
            int wid = pictureBox2.ClientSize.Width;
            int hgt = pictureBox2.ClientSize.Height;
            Bitmap bm = new Bitmap(wid, hgt);
            using (Graphics gr = Graphics.FromImage(bm))
            {
                gr.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

                // Transform to map the graph bounds to the Bitmap.
                RectangleF rect = new RectangleF(
                    xmin, ymin, xmax - xmin, ymax - ymin);
                PointF[] pts =
                {
                    new PointF(0, hgt),
                    new PointF(wid, hgt),
                    new PointF(0, 0),
                };
                gr.Transform = new System.Drawing.Drawing2D.Matrix(rect, pts);

                // Draw the graph.
                using (Pen graph_pen = new Pen(Color.Blue, 0))
                {
                    // Draw the axes.
                    gr.DrawLine(graph_pen, xmin, 0, xmax, 0);
                    gr.DrawLine(graph_pen, 0, ymin, 0, ymax);
                    for (int x = (int)xmin; x <= xmax; x++)
                    {
                        gr.DrawLine(graph_pen, x, -0.1f, x, 0.1f);
                    }
                    for (int y = (int)ymin; y <= ymax; y++)
                    {
                        gr.DrawLine(graph_pen, -0.1f, y, 0.1f, y);
                    }
                    graph_pen.Color = Color.Red;

                    // See how big 1 pixel is horizontally.
                    System.Drawing.Drawing2D.Matrix inverse = gr.Transform;
                    inverse.Invert();
                    PointF[] pixel_pts =
                    {
                        new PointF(0, 0),
                        new PointF(1, 0)
                    };
                    inverse.TransformPoints(pixel_pts);
                    float dx = pixel_pts[1].X - pixel_pts[0].X;
                    dx /= 2;

                    // Loop over x values to generate points.
                    List<PointF> points = new List<PointF>();
                    for (float x = xmin; x <= xmax; x += dx)
                    {
                        bool valid_point = false;
                        try
                        {
                            // Get the next point.
                            float y = F(x);

                            // If the slope is reasonable,
                            // this is a valid point.
                            if (points.Count == 0) valid_point = true;
                            else
                            {
                                float dy = y - points[points.Count - 1].Y;
                                if (Math.Abs(dy / dx) < 1000)
                                    valid_point = true;
                            }
                            if (valid_point) points.Add(new PointF(x, y));
                        }
                        catch
                        {
                        }

                        // If the new point is invalid, draw
                        // the points in the latest batch.
                        if (!valid_point)
                        {
                            if (points.Count > 1)
                                gr.DrawLines(graph_pen, points.ToArray());
                            points.Clear();
                        }

                    }

                    // Draw the last batch of points.
                    if (points.Count > 1)
                        gr.DrawLines(graph_pen, points.ToArray());
                }
            }

            // Display the result.
            pictureBox2.Image = bm;
        }
    }
}
