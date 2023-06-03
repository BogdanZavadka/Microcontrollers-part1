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
            TextBox[] myArr = { textBox1, textBox3, textBox4, textBox5, textBox6 };
            for (int j = 0; j < 5; j++)
            {
                List<string> nameList = new List<string>();
                List<byte> crc_received_mistake = new List<byte>();
                List<byte> crc_calc_mistake = new List<byte>();
                List<int> positions_mistake = new List<int>();
                byte crc_received;
                byte crc_calc;
                bool has_mistakes = false;
                for (int i = 0; i < 26; i++)
                {
                    byte info = (byte)serialPort1.ReadByte();
                    switch (info)
                    {
                        case 0x01:
                            nameList.Add("Z");
                            break;
                        case 0x02:
                            nameList.Add("A");
                            break;
                        case 0x03:
                            nameList.Add("V");
                            break;
                        case 0x04:
                            nameList.Add("D");
                            break;
                        case 0x05:
                            nameList.Add("K");
                            break;
                        case 0x06:
                            nameList.Add("B");
                            break;
                        case 0x07:
                            nameList.Add("O");
                            break;
                        case 0x08:
                            nameList.Add("H");
                            break;
                        case 0x09:
                            nameList.Add("N");
                            break;
                        case 0x0A:
                            nameList.Add("R");
                            break;
                        case 0x0B:
                            nameList.Add("I");
                            break;
                        case 0x0C:
                            nameList.Add("V");
                            break;
                        case 0x0D:
                            nameList.Add("C");
                            break;
                        case 0x0E:
                            nameList.Add("Y");
                            break;
                        case 0x00:
                            nameList.Add(" ");
                            break;
                        default:
                            nameList.Add("");
                            break;
                    };
                    crc_received = (byte)serialPort1.ReadByte();
                    crc_calc = CalculateCRC(info);
                    Console.WriteLine(crc_received.ToString());
                    Console.WriteLine(crc_calc.ToString());
                    if (crc_received != crc_calc)
                    {
                        has_mistakes = true;
                        crc_received_mistake.Add(crc_received);
                        crc_calc_mistake.Add(crc_calc);
                        positions_mistake.Add(i);
                    };
                }
                StringBuilder builder = new StringBuilder();
                foreach (string name in nameList)
                {
                    builder.Append(name);
                }
                string result = builder.ToString();
                if (!has_mistakes)
                {
                    myArr[j].Text = result.ToString();
                } else
                {
                    for (int k = 0; k < crc_calc_mistake.Count; k++)
                    {
                        myArr[j].Text = "Спотворення даних: " + crc_received_mistake[k].ToString() + " vs " + crc_calc_mistake[k].ToString() + " p:" + positions_mistake[k] + ", ";
                    }
                }
                    
            }
        }

        private void buttonFromSlave2_Click(object sender, EventArgs e)
        {
            byte[] b1 = new byte[3];
            b1[0] = SLAVE2_ADDRESS;
            b1[1] = 0xC1;
            serialPort1.Write(b1, 0, 2);
            TextBox[] myArr = { textBox2, textBox7, textBox8, textBox9, textBox10 };
            for (int j = 0; j < 5; j++)
            {
                List<string> numberList = new List<string>();
                List<byte> crc_received_mistake = new List<byte>();
                List<byte> crc_calc_mistake = new List<byte>();
                List<int> positions_mistake = new List<int>();
                bool has_mistakes = false;
                byte crc_received;
                byte crc_calc;
                for (int i = 0; i < 10; i++)
                {
                    byte info = (byte)serialPort1.ReadByte();
                    switch (info)
                    {
                        case 0x00:
                            numberList.Add("0");
                            break;
                        case 0x01:
                            numberList.Add("2");
                            break;
                        case 0x02:
                            numberList.Add("4");
                            break;
                        case 0x04:
                            numberList.Add("6");
                            break;
                        case 0xFF:
                            numberList.Add(".");
                            break;
                        default:
                            numberList.Add("");
                            break;
                    };
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
                StringBuilder builder = new StringBuilder();
                foreach (string name in numberList)
                {
                    builder.Append(name);
                }
                string result = builder.ToString();
                if (!has_mistakes)
                {
                    myArr[j].Text = result.ToString();
                }
                else
                {
                    for (int k = 0; k < crc_calc_mistake.Count; k++)
                    {
                        myArr[j].Text = "Спотворення даних: " + crc_received_mistake[k].ToString() + " vs " + crc_calc_mistake[k].ToString() + " p:" + positions_mistake[k] + ", ";
                    }
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
