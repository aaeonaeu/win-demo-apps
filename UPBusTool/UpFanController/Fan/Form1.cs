using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using OpenHardwareMonitor.Hardware;
using System.Threading;
using System.Configuration;

namespace fan
{
    public partial class Form1 : Form
    {
        //Temperature by openmonitor 
        public class UpdateVisitor : IVisitor
        {
            public void VisitComputer(IComputer computer)
            {
                computer.Traverse(this);
            }
            public void VisitHardware(IHardware hardware)
            {
                hardware.Update();
                foreach (IHardware subHardware in hardware.SubHardware) subHardware.Accept(this);
            }
            public void VisitSensor(ISensor sensor) { }
            public void VisitParameter(IParameter parameter) { }
        }
        private UpdateVisitor updateVisitor = new UpdateVisitor();
        private Computer computer = new Computer();
        private void GetSystemInfo()
        {
            float avergevalue = 0;
            int cpu_number = 0;
            computer.Open();
            computer.CPUEnabled = true;
            computer.Accept(updateVisitor);
            savelabel2 = "";
            for (int i = 0; i < computer.Hardware.Length; i++)
            {
                if (computer.Hardware[i].HardwareType == HardwareType.CPU)
                {
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Temperature)
                        {
                            savelabel2 += computer.Hardware[i].Sensors[j].Name + ":" + computer.Hardware[i].Sensors[j].Value.ToString() + "," + "\r";
                            avergevalue += (float)computer.Hardware[i].Sensors[j].Value;
                            cpu_number++;
                        }
                    }
                }
            }
            computer.Close();
            double result = avergevalue / cpu_number;
            result = Math.Round(result, 2, MidpointRounding.AwayFromZero);
            tempaverage = (float)result;
        }

        //Variabl
        private Thread timer1_thread;
        private Thread timer2_thread;
        private String savelabel2 = "";
        private float tempaverage=0;



        //smart fan enable function
        private void smartenablefunction(bool ben)
        {
            if(ben)
            {
                //smart enable
                timer1_thread = new Thread(smartfancontroller);
                timer1_thread.IsBackground = true;
                timer1_thread.Start();
                timer1.Enabled = true;
                timer1.Start();
            }
            else
            {
                //smart disable 
                timer1.Enabled = false;
                timer1.Stop();
            }

        }

        //smart fan
        private void smartfancontroller()
        {
            if (tempaverage < 50)
                pwmduty(0x5F);
            else if (tempaverage < 60)
                pwmduty(0xAF);
            else
                pwmduty(0xFF);
        }

        //Read Fan pwm duty by smbus
        static byte pwmduty()
        {
            UpEApis.EApis upb = new UpEApis.EApis();
            byte[] wdata = new byte[1];
            wdata[0] = 0x40;
            byte[] rdata = new byte[1] { 0 };
            upb.I2cWriteReadRaw(5, 0x2E, wdata, 1, rdata, 1, 1);

            return rdata[0];
        }
        //Write Fan pwm duty by smbus
        static void pwmduty(byte write)
        {
            UpEApis.EApis upb = new UpEApis.EApis();
            byte[] wdata = new byte[2];
            wdata[0] = 0x40;
            wdata[1] = write;
            upb.I2cWriteReadRaw(5, 0x2E, wdata, 2, null, 0, 0);
        }


        //When From not handdle,update GUI
        private delegate void updatedatadelegate();
        private void updata()
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new updatedatadelegate(updata));
            }
            else
            {
                listView1.Items.Clear();
                var text = savelabel2.Split(',').ToArray();
                if (text.Length != 0)
                {
                    ListViewItem[] item = new ListViewItem[text.Length];
                    for (int i = 0; i < text.Length; i++)
                    {
                        item[i] = new ListViewItem(text.GetValue(i).ToString());
                        item[i].ImageIndex = 8;
                    }
                    listView1.Items.AddRange(item);
                }

                if (!ManualToolStripMenuItem.Checked)
                {
                    trackBar1.Value = (int)pwmduty();
                    label1.Text = trackBar1.Value.ToString();
                }
            }

        }
        
        //Get CPU Temperature && Fan PWM duty
        public void GetTemp()
        {
            try
            {
                GetSystemInfo();
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            savelabel2 += "CPU Average temp:" + tempaverage.ToString(); 
            updata();
        }
        
        private void saveapplog(String name , int value)
        {
            Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
            config.AppSettings.Settings.Remove(name);
            config.AppSettings.Settings.Add(name, value.ToString());
            config.Save(ConfigurationSaveMode.Modified);
            ConfigurationManager.RefreshSection("appSettings");
        }

        public Form1()
        {
            InitializeComponent();

            //init timer2_thread to get temp
            timer2_thread = new Thread(new ThreadStart(GetTemp));
            timer2_thread.IsBackground = true;

            //timer1::smart fan   , timer2::Get Info
            timer1.Interval = 1000;
            timer2.Start();
            timer2.Interval = 1000;
            track_time.Interval = 100;
            //mode 0 :smart fan
            //mode 1 :slience
            //mode 2 :Maunel
            if (Convert.ToInt32(ConfigurationManager.AppSettings["functoin_mode"])== 0)
            {
                smartToolStripMenuItem.Checked = true;
            }
            else if (Convert.ToInt32(ConfigurationManager.AppSettings["functoin_mode"]) == 1)
            {
                slienceToolStripMenuItem.Checked = true;
            }
            else if (Convert.ToInt32(ConfigurationManager.AppSettings["functoin_mode"]) == 2)
            {
                ManualToolStripMenuItem.Checked = true;
            }
        }

        private void notifyIcon1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            this.Show();
            this.Activate();
        }
        private void ExitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.WindowState = FormWindowState.Minimized;
            Close();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if(!timer1_thread.IsAlive)
            {
                timer1_thread = new Thread(smartfancontroller);
                timer1_thread.IsBackground = true;
                timer1_thread.Start();
            }
        }

        private void timer2_Tick(object sender, EventArgs e)
        {

            if (timer2_thread.ThreadState==ThreadState.Unstarted)
            {
                timer2_thread.Start();
            }
            else
            {
                if(!timer2_thread.IsAlive)
                {
                    timer2_thread = new Thread(new ThreadStart(GetTemp));
                    timer2_thread.IsBackground = true;
                    timer2_thread.Start();
                }
            }

        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            about f2 = new about();
            f2.Show();
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            this.Hide();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this.WindowState != FormWindowState.Minimized)
            {
                e.Cancel = true;
                this.Hide();
            }
        }

        private void FunctionMode(object sender, EventArgs e)
        {
            if(sender==smartToolStripMenuItem)
            {
                smartToolStripMenuItem.Checked = true;
                slienceToolStripMenuItem.Checked = false;
                ManualToolStripMenuItem.Checked = false;
                saveapplog("functoin_mode", 0);
            }
            else if(sender == slienceToolStripMenuItem)
            {
                smartToolStripMenuItem.Checked = false;
                slienceToolStripMenuItem.Checked = true;
                ManualToolStripMenuItem.Checked = false;
                saveapplog("functoin_mode", 1);
            }
            else if (sender==ManualToolStripMenuItem)
            {
                smartToolStripMenuItem.Checked = false;
                slienceToolStripMenuItem.Checked = false;
                ManualToolStripMenuItem.Checked = true;
                saveapplog("functoin_mode", 2);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            label1.Text = trackBar1.Value.ToString();
            saveapplog("Manuel", trackBar1.Value);
        }

        private void smartToolStripMenuItem_CheckStateChanged(object sender, EventArgs e)
        {
            
            if (smartToolStripMenuItem.Checked)
            {
                trackBar1.Enabled = false;
                smartenablefunction(true);
            }
            else
            {
                smartenablefunction(false);
            }
        }

        private void slienceToolStripMenuItem_CheckStateChanged(object sender, EventArgs e)
        {
            if (slienceToolStripMenuItem.Checked)
            {
                trackBar1.Enabled = false;
                pwmduty((byte)0x5F);
            }
        }

        private void ManualToolStripMenuItem_CheckStateChanged(object sender, EventArgs e)
        {
            if(ManualToolStripMenuItem.Checked)
            {
                trackBar1.Value = Convert.ToInt32(ConfigurationManager.AppSettings["Manuel"]);
                label1.Text =ConfigurationManager.AppSettings["Manuel"];
                pwmduty((byte)trackBar1.Value);                
                trackBar1.Enabled = true;
                track_time.Enabled = true;
                track_time.Start();
            }
            else
            {
                track_time.Stop();
                track_time.Enabled = false;
            }

        }

        private void track_time_Tick(object sender, EventArgs e)
        {
            pwmduty((byte)trackBar1.Value);
        }
    }

}
