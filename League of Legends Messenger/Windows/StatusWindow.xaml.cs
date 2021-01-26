using League_of_Legends_Messenger.Xmpp;
using MahApps.Metro.Controls;
using System;
using System.Windows;

namespace League_of_Legends_Messenger.Windows
{
    /// <summary>
    /// Interaction logic for StatusWindow.xaml
    /// </summary>
    public partial class StatusWindow : MetroWindow
    {
        private ServerLogin serverLogin;
        public StatusWindow(ServerLogin login)
        {
            InitializeComponent();
            serverLogin = login;
            textBox13.Text = ((int)(DateTime.UtcNow.Subtract(new DateTime(1970, 1, 1))).TotalSeconds).ToString();
        }

        private int IntegerParse(string s)
        {
            int i = 0;
            if (Int32.TryParse(s, out i))
                return i;
            return 0;
        }
        private uint UIntegerParse(string s)
        {
            uint i = 0;
            if (UInt32.TryParse(s, out i))
                return i;
            return 0;
        }

        private void okButton_Click(object sender, RoutedEventArgs e)
        {
            serverLogin.SetPresence(ServerLogin.getFormattedStatus(
                IntegerParse(textBox1.Text),
                textBox2.Text,
                IntegerParse(textBox3.Text),
                IntegerParse(textBox4.Text),
                IntegerParse(textBox5.Text),
                textBox6.Text,
                IntegerParse(textBox7.Text),
                IntegerParse(textBox8.Text),
                IntegerParse(textBox9.Text),
                textBox10.Text,
                textBox11.Text,
                textBox12.Text,
                UIntegerParse(textBox13.Text)
                ));
            this.Close();
        }

        private void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}
