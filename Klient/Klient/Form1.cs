using SimpleTCP;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;

namespace Klient
{
    public partial class Form1 : Form
    {
        public int i = 0,tryb=0;
        public int ind = 0;
        CheckBox lastChecked;
        public string nr;
        public bool ok=false,gora=false,dol=false,lewo=false,prawo=false;
        public bool pierwszygora = true, pierwszydol = true, pierwszyprawo = true, pierwszylewo = true;
        public Form1()
        {
            InitializeComponent();
        }
        SimpleTcpClient client;
        private void connect(object sender, EventArgs e)
        {
            try
            {
                if (i == 0) { 
                    client.Connect("192.168.4.1", 80);
                    i++;
                    button1.Text = "rozlacz";
                }
                else if (i == 1)
                {
                    client.Disconnect();
                    i--;
                    button1.Text = "polacz";
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        public void sendMessageToESP(string msg)
        {
            try
            {
                client.WriteLineAndGetReply(msg, TimeSpan.FromMilliseconds(1));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            client = new SimpleTcpClient();
            client.StringEncoder = Encoding.UTF8;
            client.DataReceived += Client_DataReceived;
        }

        private void Client_DataReceived(object sender, SimpleTCP.Message e)
        {
            pos.Invoke((MethodInvoker)delegate ()
            {
                pos.Text += e.MessageString;
            });
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (ok)
            {
                if (Int32.TryParse(pos.Text, out int x))
                {
                   if (x > 180)
                    {
                        pos.Text = "180";
                    }
                    else if (x < 0)
                    {
                        pos.Text = "0";
                    }
                    string s = nr;
                    s += pos.Text;
                    s += "%";
                    sendMessageToESP(s);
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string s = nr;
            s += "0%";
            sendMessageToESP(s);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            string s = nr;
            s += "90%";
            sendMessageToESP(s);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            string s = nr;
            s += "180%";
            sendMessageToESP(s);
        }

        private void c0_Click(object sender, EventArgs e)
        {       ok = true;
            CheckBox activeCheckBox = sender as CheckBox;
            if (activeCheckBox.Checked == false&&activeCheckBox==lastChecked) activeCheckBox.Checked = true;
            if (activeCheckBox != lastChecked && lastChecked != null) lastChecked.Checked = false;
            lastChecked = activeCheckBox.Checked ? activeCheckBox : null;
            nr = "l";
            nr += activeCheckBox.Text;
            nr += "$";
        }

        private void text_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyValue == 13)
            {
                e.SuppressKeyPress = true;
                button3_Click(sender, e);
                
            }

        }

        private void wakeWake(object sender, EventArgs e)
        {
            sendMessageToESP("lw@");
        }

        private void sleepSleep(object sender, EventArgs e)
        {
            sendMessageToESP("ls@");
        }

        private void set0(object sender, EventArgs e)
        {
            for(int i=0;i<18;i+=3)
            {
                string ss ="l" + i.ToString()+"$"+pos.Text+"%";
                sendMessageToESP(ss);
            }
        }
        private void set1(object sender, EventArgs e)
        {
            for (int i = 1; i < 18; i += 3)
            {
                string ss = "l" + i.ToString() + "$" + pos.Text+"%";
                sendMessageToESP(ss);
            }
        }
        private void set2(object sender, EventArgs e)
        {
            for (int i = 2; i < 18; i += 3)
            {
                string ss = "l" + i.ToString() + "$" + pos.Text+"%";
                sendMessageToESP(ss);
            }
        }
        private void balance1(object sender, EventArgs e)
        {
            sendMessageToESP("lb1@");
        }
        private void balance2(object sender, EventArgs e)
        {
            sendMessageToESP("lb2@");
        }
        private void step1(object sender, EventArgs e)
        {
            sendMessageToESP("ls1@");
            Thread.Sleep(10);
            sendMessageToESP("lx@");
        }
        private void step2(object sender, EventArgs e)
        {
            sendMessageToESP("ls2@");
            Thread.Sleep(10);
            sendMessageToESP("lx@");
        }
        private void wave(object sender, EventArgs e)
        {
            sendMessageToESP("lwa@");
        }

        private void kalibracjaL(object sender, EventArgs e)
        {
            sendMessageToESP("lk2@");
            Thread.Sleep(10);
            sendMessageToESP("lx@");
        }

        private void kalibracjaR(object sender, EventArgs e)
        {
            sendMessageToESP("lk1@");
            Thread.Sleep(10);
            sendMessageToESP("lx@");
        }
        private void stepp(object sender, EventArgs e)
        {
            sendMessageToESP("lss@");
            Thread.Sleep(10);
            sendMessageToESP("lx@");
        }
        private void bStepp(object sender, EventArgs e)
        {
            sendMessageToESP("lsb@");
            Thread.Sleep(10);
            sendMessageToESP("lx@");
        }
        private void button16_Click(object sender, EventArgs e)
        {
            if (tryb == 0)
            {
                tryb++;
                sendMessageToESP("l6$30%");
                sendMessageToESP("l12$50%");
                sendMessageToESP("l0$90%");
            }
            else
            {
                tryb--;
                sendMessageToESP("l6$90%");
                sendMessageToESP("l12$150%");
                sendMessageToESP("l0$140%");
            }
        }
        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
        
            if (e.KeyCode == Keys.W)
            {
                gora = true;
            }

            if (e.KeyCode == Keys.S)
            {
                dol = true;
            }


            if (e.KeyCode == Keys.D)
            {
                prawo = true;
            }

            if (e.KeyCode == Keys.A)
            {
                lewo = true;
            }



            if (gora && !dol && !lewo && !prawo && pierwszygora)
            {
                if (wolno.Checked)
                {
                    sendMessageToESP("ls1@");
                }
                else
                {
                    sendMessageToESP("lss@");
                }
                pierwszygora = false;

            }
            if (!gora && dol && !lewo && !prawo && pierwszydol)
            {
                if (wolno.Checked)
                {
                    sendMessageToESP("ls2@");
                }
                else
                {
                    sendMessageToESP("lsb@");
                }
            }
            if (!gora && !dol && lewo && !prawo && pierwszylewo)
            {
                sendMessageToESP("lk2@");
                pierwszylewo = false;
            }
            if (!gora && !dol && !lewo && prawo && pierwszyprawo)
            {
                sendMessageToESP("lk1@");
                pierwszyprawo = false;
            }
            if (!gora && !dol && !lewo && !prawo)
            {
                sendMessageToESP("lx@");
            }

        }


        private void Form1_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.W)
            {
                pierwszygora = true;
                gora = false;
            }
            if (e.KeyCode == Keys.A)
            {
                pierwszylewo = true;
                lewo = false;
            }
            if (e.KeyCode == Keys.S)
            {
                pierwszydol = true;
                dol = false;
            }
            if (e.KeyCode == Keys.D)
            {
                pierwszyprawo = true;
                prawo = false;
            }

            if (gora && !dol && !lewo && !prawo)
            {
                if (wolno.Checked)
                {
                    sendMessageToESP("ls1@");
                }
                else
                {
                    sendMessageToESP("lss@");
                }
            }
            if (!gora && dol && !lewo && !prawo)
            {
                if (wolno.Checked)
                {
                    sendMessageToESP("ls2@");
                }
                else
                {
                    sendMessageToESP("lsb@");
                }
            }
            if (!gora && !dol && lewo && !prawo)
            {
                sendMessageToESP("lk2@");
            }
            if (!gora && !dol && !lewo && prawo)
            {
             sendMessageToESP("lk1@");
            }
            if (!gora && !dol && !lewo && !prawo)
            {
                sendMessageToESP("lx@");
            }
        }


    }
}
