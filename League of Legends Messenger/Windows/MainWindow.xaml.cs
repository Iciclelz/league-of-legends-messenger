using League_of_Legends_Messenger.Riot;
using League_of_Legends_Messenger.Utilities;
using League_of_Legends_Messenger.Xmpp;
using MahApps.Metro;
using MahApps.Metro.Controls;
using MahApps.Metro.Controls.Dialogs;
using Matrix.Xmpp;
using Matrix.Xmpp.Client;
using Matrix.Xmpp.Roster;
using Microsoft.Win32;
using System;
using System.ComponentModel;
using System.IO;
using System.Net;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;

namespace League_of_Legends_Messenger.Windows
{
    //
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        private IniReader iniReader;
        private IniWriter iniWriter;
        private String iniPath;
        public LoginData loginData;
        private ServerLogin serverLogin;
        private XmppClient xmppClient;
        private RiotApi riotApi;
        public MainWindow()
        {
            BorderThickness = new Thickness(0);
            GlowBrush = this.FindResource("BlackBrush") as SolidColorBrush;


            InitializeComponent();

            iniPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "League of Legends Messenger", "Settings.ini");
            if (!Directory.Exists(Path.GetDirectoryName(iniPath)))
            {
                Directory.CreateDirectory(Path.GetDirectoryName(iniPath));
            }

            iniReader = new IniReader(iniPath);
            iniWriter = new IniWriter(iniPath);
            loginData = new LoginData(iniReader.Read("Login", "Username"), iniReader.Read("Login", "ApiKey"), iniReader.ReadInt("Login", "Server"));
        }

        private void mainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            int theme = iniReader.ReadInt("Settings", "Theme", -1);

            switch (theme)
            {
                case -1:
                    iniWriter.WriteInt("Settings", "Theme", 0);
                    ThemeManager.ChangeAppTheme(Application.Current, "BaseLight");
                    lightMenuItem.IsChecked = true;
                    darkMenuItem.IsChecked = false;
                    break;
                case 0:
                    ThemeManager.ChangeAppTheme(Application.Current, "BaseLight");
                    lightMenuItem.IsChecked = true;
                    darkMenuItem.IsChecked = false;
                    break;
                case 1:
                    ThemeManager.ChangeAppTheme(Application.Current, "BaseDark");
                    lightMenuItem.IsChecked = false;
                    darkMenuItem.IsChecked = true;
                    break;

            }

            if (String.IsNullOrEmpty(loginData.ApiKey))
            {
                new OptionsWindow().ShowDialog();
            }

            ShowLoginDialog();

        }

        private async void ShowLoginDialog()
        {
            LoginDialogData loginDialogData = await this.ShowLoginAsync("League of Legends Login", "", new LoginDialogSettings
            {
                ColorScheme = MetroDialogColorScheme.Theme,
                InitialUsername = loginData.Username,
                UsernameWatermark = "username",
                PasswordWatermark = "password"
            });

            if (loginDialogData != null)
            {
                if (loginData.Username.CompareTo(loginDialogData.Username) != 0)
                {
                    loginData.Username = loginDialogData.Username;
                    iniWriter.Write("Login", "Username", loginData.Username);
                }
                loginData.Password = loginDialogData.Password;

                serverLogin = new ServerLogin(this);
            }
        }

        private TreeViewItem CloneTreeViewItem(TreeViewItem tvi, Presence presence)
        {
            return new TreeViewItem()
            {
                Header = tvi.Header,
                Tag = presence
            };
        }

        public void OnLogin(XmppClient xmppClient)
        {
            this.xmppClient = xmppClient;
            this.xmppClient.OnPresence += new EventHandler<PresenceEventArgs>(OnPresence);
            this.xmppClient.OnMessage += new EventHandler<MessageEventArgs>(OnMessage);
            riotApi = new RiotApi(loginData.ApiKey, loginData.ServerName);
            serverNameLabel.Content = loginData.ServerName;
        }

        private void OnPresence(object sender, PresenceEventArgs e)
        {
            try
            {
                string Entry = riotApi.getSummonerName(e.Presence.From.User);
                if (e.Presence.Type.ToString().CompareTo("available") == 0)
                {
                    foreach (TreeViewItem tvi in offlineTreeViewItem.Items)
                    {
                        if (tvi.Header.Equals(Entry))
                        {
                            onlineTreeViewItem.Items.Add(CloneTreeViewItem(tvi, e.Presence));
                            offlineTreeViewItem.Items.Remove(tvi);

                            onlineTreeViewItem.Items.SortDescriptions.Clear();
                            offlineTreeViewItem.Items.SortDescriptions.Clear();
                            onlineTreeViewItem.Items.SortDescriptions.Add(new SortDescription("Header", ListSortDirection.Ascending));
                            offlineTreeViewItem.Items.SortDescriptions.Add(new SortDescription("Header", ListSortDirection.Ascending));
                            break;
                        }
                    }
                }
                else if (e.Presence.Type.ToString().CompareTo("unavailable") == 0)
                {
                    foreach (TreeViewItem tvi in onlineTreeViewItem.Items)
                    {
                        if (tvi.Header.Equals(Entry))
                        {
                            offlineTreeViewItem.Items.Add(CloneTreeViewItem(tvi, e.Presence));
                            onlineTreeViewItem.Items.Remove(tvi);

                            onlineTreeViewItem.Items.SortDescriptions.Clear();
                            offlineTreeViewItem.Items.SortDescriptions.Clear();
                            onlineTreeViewItem.Items.SortDescriptions.Add(new SortDescription("Header", ListSortDirection.Ascending));
                            offlineTreeViewItem.Items.SortDescriptions.Add(new SortDescription("Header", ListSortDirection.Ascending));
                            break;
                        }
                    }
                }

                
                
            }

            catch { }
        }

        private void OnMessage(object sender, MessageEventArgs e)
        {
            try
            {
                if (!String.IsNullOrEmpty(e.Message.Body) && !String.IsNullOrEmpty(e.Message.From.User))
                {
                    string summonerName = riotApi.getSummonerName(e.Message.From.User);
                    foreach (MetroTabItem mti in tabControl.Items)
                    {
                        if (mti.Header.Equals(summonerName))
                        {
                            tabControl.SelectedItem = mti;
                            GetMessage(summonerName, e.Message.Body);
                            return;
                        }
                    }

                    AddTabItem(summonerName);
                    GetMessage(summonerName, e.Message.Body);
                }
            }
            catch
            {
            }
        }

        public void AddToRoster(RosterEventArgs e)
        {
            offlineTreeViewItem.Items.Add(new TreeViewItem { Header = e.RosterItem.Name }) ;
        }

        private void mainWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (xmppClient != null)
            {
                xmppClient.Close();
            }
            Environment.Exit(0);
        }

        private void exitMenuItem_Click(object sender, RoutedEventArgs e)
        {
            Environment.Exit(0);
        }

        private void optionsMenuItem_Click(object sender, RoutedEventArgs e)
        {
            new OptionsWindow().ShowDialog();
        }

        private void statusSettingsMenuItem_Click(object sender, RoutedEventArgs e)
        {
            new StatusWindow(serverLogin).ShowDialog();
        }

        private async void aboutMenuItem_Click(object sender, RoutedEventArgs e)
        {
            await this.ShowMessageAsync("About League of Legends Messenger",
                "Product: League of Legends Messenger" + Environment.NewLine +
                "Version: 2.1.0.0" + Environment.NewLine +
                "Creator: Kisenon" + Environment.NewLine +
                "Project: http://kisenon.github.io/projects/League%20of%20Legends%20Messenger/");
        }

        private void treeView_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (onlineTreeViewItem.Items.Contains(treeView.SelectedItem) || offlineTreeViewItem.Items.Contains(treeView.SelectedItem))
            {
                string summonerName = (treeView.SelectedItem as TreeViewItem).Header.ToString();
                foreach (MetroTabItem mti in tabControl.Items)
                {
                    if (summonerName.Equals(mti.Header))
                    {
                        tabControl.SelectedItem = mti;
                        return;
                    }
                }
                AddTabItem(summonerName);
            }

        }

        private void lightMenuItem_Checked(object sender, RoutedEventArgs e)
        {
            lightMenuItem.IsChecked = true;
            darkMenuItem.IsChecked = false;
            iniWriter.WriteInt("Settings", "Theme", 0);
            ThemeManager.ChangeAppTheme(Application.Current, "BaseLight");
        }

        private void darkMenuItem_Checked(object sender, RoutedEventArgs e)
        {
            lightMenuItem.IsChecked = false;
            darkMenuItem.IsChecked = true;
            iniWriter.WriteInt("Settings", "Theme", 1);
            ThemeManager.ChangeAppTheme(Application.Current, "BaseDark");
        }

        private void lightMenuItem_Unchecked(object sender, RoutedEventArgs e)
        {
            if (darkMenuItem.IsChecked == false)
            {
                lightMenuItem.IsChecked = true;
            }
        }

        private void darkMenuItem_Unchecked(object sender, RoutedEventArgs e)
        {
            if (lightMenuItem.IsChecked == false)
            {
                darkMenuItem.IsChecked = true;
            }
        }

        private void treeView_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            
            try
            {
                summonerStatusBarItem.Content = "";
                divisionStatusBarItem.Content = "";

                if (onlineTreeViewItem.Items.Contains(treeView.SelectedItem) || offlineTreeViewItem.Items.Contains(treeView.SelectedItem))
                {
                    summonerStatusBarItem.Content = (treeView.SelectedItem as TreeViewItem).Header;
                    SummonerData data = new SummonerData((treeView.SelectedItem as TreeViewItem).Tag as Presence);
                    summonerStatusBarItem.Content += "<" + SummonerData.FirstCharToUpper(data.Availability.ToString().ToLower()) + ">: " + data.StatusMessage;
                    divisionStatusBarItem.Content = "LEVEL: " + data.Level + ", " + data.RankedLeagueTier + " " + data.RankedLeagueDivison + " - " + data.RankedLeagueName.Replace("&apos;", "'") + " (Wins: " + data.RankedWins + ")";
                }
            }
            catch { }
        }

        private void AddTabItem(String summonerName)
        {
            if (String.IsNullOrEmpty(summonerName))
            {
                return;
            }
            MetroTabItem metroTabItem = new MetroTabItem()
            {
                Header = summonerName,
                CloseButtonEnabled = true,
            };
            BindingOperations.SetBinding(metroTabItem, MetroTabItem.CloseTabCommandProperty, new Binding("CloseTabCommand"));
            BindingOperations.SetBinding(metroTabItem, MetroTabItem.CloseTabCommandParameterProperty, new Binding()
            {
                RelativeSource = RelativeSource.Self,
                Path = new PropertyPath("Header")
            });
            TextBox textBox = new TextBox()
            {
                TextWrapping = TextWrapping.Wrap,
                Margin = new Thickness(0, 0, 0, 27),
                IsReadOnly = true,
                VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
                HorizontalScrollBarVisibility = ScrollBarVisibility.Auto,
                HorizontalContentAlignment = HorizontalAlignment.Stretch,
                VerticalContentAlignment = VerticalAlignment.Stretch,
            };
            TextBox inputTextBox = new TextBox()
            {
                TextWrapping = TextWrapping.NoWrap,
                Margin = new Thickness(0, 0, 30, 0),
                VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
                HorizontalScrollBarVisibility = ScrollBarVisibility.Auto,
                HorizontalAlignment = HorizontalAlignment.Stretch,
                VerticalAlignment = VerticalAlignment.Bottom
            };
            Button enterButton = new Button()
            {
                Content = "➜",
                Margin = new Thickness(0, 0, 0, 0),
                Width = 30,
                HorizontalAlignment = HorizontalAlignment.Right,
                VerticalAlignment = VerticalAlignment.Bottom
            };

            inputTextBox.KeyDown += InputTextBox_KeyDown;
            enterButton.Click += EnterButton_Click;
            Grid grid = new Grid() { Background = new SolidColorBrush(Color.FromRgb(229, 220, 229)) };
            grid.Children.Add(textBox);
            grid.Children.Add(inputTextBox);
            grid.Children.Add(enterButton);
            metroTabItem.Content = grid;
            tabControl.SelectedIndex = tabControl.Items.Add(metroTabItem);
        }

        private void InputTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                EnterButton_Click(this, e);
            }
        }

        private void EnterButton_Click(object sender, RoutedEventArgs e)
        {
            MetroTabItem tabItem = tabControl.SelectedItem as MetroTabItem;
            TextBox logTextBox = (tabItem.Content as Grid).Children[0] as TextBox;
            TextBox textBox = (tabItem.Content as Grid).Children[1] as TextBox;
            SendMessage(tabItem.Header.ToString(), textBox.Text, logTextBox, textBox);
        }

        public static string LinkShorten(string url) => new Regex("{\"url\":\"(.*?)\",\"short_code\"").Match(new WebClient().DownloadString("https://api.waa.ai/shorten?url=" + url)).Groups[1].ToString().Replace("\\", "");
        public static string FileUpload(string filePath) => new Regex("\"url\":\"(.*?)\",\"deletelink\"").Match(Encoding.UTF8.GetString(new WebClient().UploadFile("http://www.hnng.moe/upload_d.php", "POST", filePath))).Groups[1].ToString();

        private void SendMessage(string summonerName, string message, TextBox logTextBox, TextBox sendTextBox)
        {
            if (!String.IsNullOrEmpty(message))
            {
                if (message.StartsWith("!shorten "))
                {
                    message = LinkShorten(message.Replace("!shorten ", "")).Trim();
                }
                
                xmppClient.Send(new Message(new RiotApi(loginData.ApiKey, loginData.ServerName).getJid(summonerName), MessageType.chat, message));
                logTextBox.Text += xmppClient.Username + ": " + message + Environment.NewLine;
                logTextBox.ScrollToEnd();
                sendTextBox.Text = "";
            }
        }

        private void GetMessage(string summonerName, string message)
        {
            if (!String.IsNullOrEmpty(message) && !String.IsNullOrEmpty(summonerName.Trim()))
            {
                MetroTabItem mti = tabControl.SelectedItem as MetroTabItem;
                TextBox logTextBox = (mti.Content as Grid).Children[0] as TextBox;
                logTextBox.Text += summonerName + ": " + message + Environment.NewLine;
                logTextBox.ScrollToEnd();
            }
        }

        private void saveAsMenuItem_Click(object sender, RoutedEventArgs e)
        {
            try {
                string summonerName = (tabControl.SelectedItem as MetroTabItem).Header.ToString();
                SaveFileDialog saveFileDialog = new SaveFileDialog()
                {
                    Title = "League of Legends Messenger - Save Chatlog As...",
                    Filter = "Text Documents (*.txt)|*.txt",
                    FileName = ("Chatlog with " + summonerName + " on " + DateTime.Now).Replace(":", "+").Replace("/", "-")
                };

                if (saveFileDialog.ShowDialog() == true)
                {
                    File.WriteAllText(saveFileDialog.FileName, (((tabControl.SelectedItem as MetroTabItem).Content as Grid).Children[0] as TextBox).Text);
                }
            }
            catch { }
        }

        private void tabControl_Drop(object sender, DragEventArgs e)
        {
            String[] fileNames = (String[])e.Data.GetData(DataFormats.FileDrop, true);
            if (fileNames.Length > 0)
            {
                foreach (string fileName in fileNames)
                {
                    if (File.Exists(fileName))
                    {
                        string link = FileUpload(fileName);
                        xmppClient.Send(new Message(new RiotApi(loginData.ApiKey, loginData.ServerName).getJid((tabControl.SelectedItem as MetroTabItem).Header.ToString()), MessageType.chat, link));
                        TextBox logTextBox = ((tabControl.SelectedItem as MetroTabItem).Content as Grid).Children[0] as TextBox;
                        logTextBox.Text += xmppClient.Username + ": " + link + Environment.NewLine;
                    }
                }

            }
            e.Handled = true;
        }
    }
    public class SimpleCommand : ICommand
    {
        public Predicate<object> CanExecuteDelegate { get; set; }
        public Action<object> ExecuteDelegate { get; set; }

        public bool CanExecute(object parameter)
        {
            if (CanExecuteDelegate != null)
                return CanExecuteDelegate(parameter);
            return true; // if there is no can execute default to true
        }

        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        public void Execute(object parameter)
        {
            if (ExecuteDelegate != null)
                ExecuteDelegate(parameter);
        }
    }
}
