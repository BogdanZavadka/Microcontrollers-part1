using System;
using System.Data;
using System.Linq;
using System.Windows.Forms;
using System.IO.Ports;
using System.Drawing;
using System.Threading;
using System.Collections.Generic;
using System.Text;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        const byte SLAVE1_ADDRESS = 0x17;
        const byte SLAVE2_ADDRESS = 0x36;
        byte[] crc8_darc_table = new byte[256];

        public Form1()
        {
            InitializeComponent();
            for (int i = 0; i < 256; ++i)
            {
                byte crc = (byte)i;
                for (int j = 0; j < 8; ++j)
                {
                    if ((crc & 0x80) != 0)
                    {
                        crc = (byte)((crc << 1) ^ 0x39);
                    }
                    else
                    {
                        crc <<= 1;
                    }
                }
                crc8_darc_table[i] = crc;
            }

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
            {
                try
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.BaudRate = 38400;
                    serialPort1.Open();
                    buttonOpenPort.Text = "Close";
                    comboBox1.Enabled = false;
                    buttonFromSlave1.Visible = true;
                    buttonFromSlave2.Visible = true;
                }
                catch
                {
                    MessageBox.Show("Port " + comboBox1.Text + " is invalid!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                serialPort1.Close();
                buttonOpenPort.Text = "Open";
                comboBox1.Enabled = true;
                buttonFromSlave1.Visible = false;
                buttonFromSlave2.Visible = false;
            }
        }

        private void buttonFromSlave1_Click(object sender, EventArgs e)
        {
            byte[] b1 = new byte[3];
            b1[0] = SLAVE1_ADDRESS;
            b1[1] = 0xC1;
            serialPort1.Write(b1, 0, 2);
            List<int> lis = new List<int>();
            List<byte> crc_received_mistake = new List<byte>();
            List<byte> crc_calc_mistake = new List<byte>();
            List<int> positions_mistake = new List<int>();
            byte crc_received;
            byte crc_calc;
            bool has_mistakes = false;
            for (int i = 0; i<6; i++)
            {
                byte info = (byte)serialPort1.ReadByte();
                lis.Add((info / 16 * 10) + (info % 16));
                crc_received = (byte)serialPort1.ReadByte();
                crc_calc = CalculateCRC(info);
                if (crc_received != crc_calc)
                {
                    has_mistakes = true;
                    crc_received_mistake.Add(crc_received);
                    crc_calc_mistake.Add(crc_calc);
                    positions_mistake.Add(i);
                };
            }
            if (!has_mistakes)
            {
                textBox1.Text = "час " + lis[2].ToString() + ":" + lis[1].ToString() + ":" + lis[0].ToString() + ", дата " + lis[3].ToString() + "." + lis[4].ToString() + ".20" + lis[5].ToString();
            }
            else
            {
                for (int k = 0; k < crc_calc_mistake.Count; k++)
                {
                    textBox1.Text = "Спотворення даних: " + crc_received_mistake[k].ToString() + " vs " + crc_calc_mistake[k].ToString() + " p:" + positions_mistake[k] + ", ";
                }
            }
      ;
        }

        private void buttonFromSlave2_Click(object sender, EventArgs e)
        {
            byte[] b1 = new byte[3];
            b1[0] = SLAVE2_ADDRESS;
            b1[1] = 0xC1;
            serialPort1.Write(b1, 0, 2);
            List<int> lis = new List<int>();
            List<byte> crc_received_mistake = new List<byte>();
            List<byte> crc_calc_mistake = new List<byte>();
            List<int> positions_mistake = new List<int>();
            byte crc_received;
            byte crc_calc;
            bool has_mistakes = false;
            for (int i = 0; i < 6; i++)
            {
                byte info = (byte)serialPort1.ReadByte();
                lis.Add((info / 16 * 10) + (info % 16));
                crc_received = (byte)serialPort1.ReadByte();
                crc_calc = CalculateCRC(info);
                if (crc_received != crc_calc)
                {
                    has_mistakes = true;
                    crc_received_mistake.Add(crc_received);
                    crc_calc_mistake.Add(crc_calc);
                    positions_mistake.Add(i);
                };
            }
            if (!has_mistakes)
            {
                textBox2.Text = "час " + lis[2].ToString() + ":" + lis[1].ToString() + ":" + lis[0].ToString() + ", дата " + lis[3].ToString() + "." + lis[4].ToString() + ".20" + lis[5].ToString();
            }
            else
            {
                for (int k = 0; k < crc_calc_mistake.Count; k++)
                {
                    textBox2.Text = "Спотворення даних: " + crc_received_mistake[k].ToString() + " vs " + crc_calc_mistake[k].ToString() + " p:" + positions_mistake[k] + ", ";
                }
            }
        }

        byte CalculateCRC(byte message)
        {
            byte crc = 0;
            crc = crc8_darc_table[crc ^ message];
            return crc;
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
