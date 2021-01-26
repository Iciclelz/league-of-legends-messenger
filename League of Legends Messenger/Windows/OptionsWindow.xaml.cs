using League_of_Legends_Messenger.Utilities;
using MahApps.Metro.Controls;
using System;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Media;

namespace League_of_Legends_Messenger.Windows
{
    /// <summary>
    /// Interaction logic for OptionsWindow.xaml
    /// </summary>
    public partial class OptionsWindow : MetroWindow
    {
        private string iniPath;
        private IniReader iniReader;
        private IniWriter iniWriter;
        public OptionsWindow()
        {
            BorderThickness = new Thickness(1);
            GlowBrush = this.FindResource("AccentColorBrush") as SolidColorBrush;

            InitializeComponent();
            iniPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "League of Legends Messenger", "Settings.ini");
            if (!Directory.Exists(Path.GetDirectoryName(iniPath)))
            {
                Directory.CreateDirectory(Path.GetDirectoryName(iniPath));
            }
            iniReader = new IniReader(iniPath);
            iniWriter = new IniWriter(iniPath);

            apiPasswordBox.Password = iniReader.Read("Login", "ApiKey");
            serverComboBox.SelectedIndex = iniReader.ReadInt("Login", "Server");
        }

        private void apiButton_Click(object sender, RoutedEventArgs e)
        {
            if (MessageBox.Show("You will need a Riot Games Developer API Key in order to use LoL Messenger. You may go to: \"https://developer.riotgames.com/\" to get your key for free from Riot Games. Click yes to navigate to the site now.", "League of Legends Messenger: About Riot Games Developer API Key", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
            {
                Process.Start("https://developer.riotgames.com/");
            }
        }

        private void okButton_Click(object sender, RoutedEventArgs e)
        {
            iniWriter.Write("Login", "ApiKey", apiPasswordBox.Password);
            iniWriter.WriteInt("Login", "Server", serverComboBox.SelectedIndex);

            if (!String.IsNullOrEmpty(iniReader.Read("Login", "ApiKey")))
            {
                cancelButton_Click(this, e);
            }           
        }

        private void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}
