﻿using System;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        private int algorithmCount = 0;
        private int algorithmNumber;
        
        public Form1()
        {
            InitializeComponent();
        }
        
        private void comboBox1_Click(object sender, EventArgs e)
        {
            int num;
            comboBox1.Items.Clear();
            string[] ports = SerialPort.GetPortNames().OrderBy(a => a.Length > 3 && int.TryParse(a.Substring(3), out num) ? num : 0).ToArray();
            comboBox1.Items.AddRange(ports);
        }

        private void buttonOpenPort_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)
                try
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.Open();
                    buttonOpenPort.Text = "Close";
                    comboBox1.Enabled = false;
                    button1.Visible = true;
                    button2.Visible = true;
                }
                catch
                {
                    MessageBox.Show("Port " + comboBox1.Text + " is invalid!", "Error", 
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            else
            {
                serialPort1.Close();
                buttonOpenPort.Text = "Open";
                comboBox1.Enabled = true;
                button1.Visible = false;
                button2.Visible = false;
            }
        }

        private void writeByteToArduino(byte command)
        {
            byte[] b1 = new byte[1];
            b1[0] = command;
            serialPort1.Write(b1, 0, 1);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            writeByteToArduino(0xA1);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            writeByteToArduino(0xA2);
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte commandFromArduino = (byte)serialPort1.ReadByte();
            if (commandFromArduino == 0xB1)
            {
                algorithmNumber = 1;
                algorithmCount = 0;
                this.BeginInvoke(new ThreadStart(startTimer));
            }
            else if (commandFromArduino == 0xB2)
            {
                algorithmNumber = 2;
                algorithmCount = 0;
                this.BeginInvoke(new ThreadStart(startTimer));
            }

        }
        private void clearAllLed()
        {
            panel1.BackColor = Color.SkyBlue;
            panel2.BackColor = Color.SkyBlue;
            panel3.BackColor = Color.SkyBlue;
            panel4.BackColor = Color.SkyBlue;
            panel5.BackColor = Color.SkyBlue;
            panel6.BackColor = Color.SkyBlue;
            panel7.BackColor = Color.SkyBlue;
            panel8.BackColor = Color.SkyBlue;
        }

        private void startTimer()
        {
            timer1.Start();
        }

        private void runAlgorithm1()
        {
            clearAllLed();
            algorithmCount++;
            switch (algorithmCount)
            {
                case 1: panel8.BackColor = Color.Red; break;
                case 2: panel6.BackColor = Color.Red; break;
                case 3: panel4.BackColor = Color.Red; break;
                case 4: panel2.BackColor = Color.Red; break;
                case 5: panel7.BackColor = Color.Red; break;
                case 6: panel5.BackColor = Color.Red; break;
                case 7: panel3.BackColor = Color.Red; break;
                case 8: panel1.BackColor = Color.Red; break;
                case 9: timer1.Stop();
                    algorithmNumber = 0;
                    break;
            }
        }

        private void runAlgorithm2()
        {
            clearAllLed();
            algorithmCount++;
            switch (algorithmCount)
            {
                case 1: panel8.BackColor = Color.Red; break;
                case 2: panel1.BackColor = Color.Red; break;
                case 3: panel7.BackColor = Color.Red; break;
                case 4: panel2.BackColor = Color.Red; break;
                case 5: panel6.BackColor = Color.Red; break;
                case 6: panel3.BackColor = Color.Red; break;
                case 7: panel5.BackColor = Color.Red; break;
                case 8: panel4.BackColor = Color.Red; break;
                case 9:
                    timer1.Stop();
                    algorithmNumber = 0;
                    break;
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (algorithmNumber == 1)
                runAlgorithm1();
            else if (algorithmNumber == 2)
                runAlgorithm2();

        }
    }
}
