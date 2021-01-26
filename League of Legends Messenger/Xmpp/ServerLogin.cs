using League_of_Legends_Messenger.Windows;
using Matrix;
using Matrix.Xmpp.Client;
using Matrix.Xmpp.Roster;
using Matrix.Xmpp.Sasl;
using System;
using System.Diagnostics;
using System.Threading;
using System.Windows;

namespace League_of_Legends_Messenger.Xmpp
{
    public class ServerLogin : XmppClient
    {
        private MainWindow mainWindow;

        public ServerLogin(MainWindow mainWindow)
        {
            this.mainWindow = mainWindow;
            mainWindow.loginData.Password = "AIR_" + mainWindow.loginData.Password;

            AnonymousLogin = false;
            AutoPresence = true;
            AutoReplyToPing = false;
            AutoRoster = true;
            Compression = true;
            Priority = 0;
            RegisterNewAccount = false;
            UseSso = true;

            SetUsername(mainWindow.loginData.Username);
            SetXmppDomain("pvp.net");
            SetResource("xiff");
            Password = mainWindow.loginData.Password;

            ResolveSrvRecords = false;
            Hostname = ServerToHostName();
            OldStyleSsl = true;
            Port = 5223;

            AutoInvoke = true;

            StartTls = true;

            Transport = Matrix.Net.Transport.Socket;
            StreamManagement = true;
            PreferredSsoSaslMechanism = SaslMechanism.X_OAUTH2;

            OnValidateCertificate += new EventHandler<CertificateEventArgs>(OnValidateCertificateEvent);
            OnRosterItem += new EventHandler<RosterEventArgs>(OnRosterItemEvent);
            OnStreamError += new EventHandler<StreamErrorEventArgs>(OnStreamErrorEvent);
            OnError += new EventHandler<ExceptionEventArgs>(OnErrorEvent);
            OnLogin += new EventHandler<Matrix.EventArgs>(OnLoginEvent);
            OnAuthError += new EventHandler<SaslEventArgs>(OnAuthErrorEvent);

            try { Open(); }
            catch (Exception ex) { MessageBox.Show(ex.Message, "League of Legends Messenger Connection Error", MessageBoxButton.OK, MessageBoxImage.Error); }

            do
            {
                Thread.Sleep(1000);
            } while (!StreamActive);

            SetPresence(getFormattedStatus(new Random().Next(1, 29), "LoL Messenger", 0, 0, 0, "", 0, 0, 0, "", "", "", 0));
        }

        public string ServerToHostName()
        {
            string serverHostName = "na2";
            switch (mainWindow.loginData.ServerId)
            {
                case 0:
                    serverHostName = "na2"; //192.64.174.69
                    break;
                case 1:
                    serverHostName = "eu";
                    break;
                case 2:
                    serverHostName = "eun1";
                    break;
                case 3:
                    serverHostName = "oc1";
                    break;
                case 4:
                    serverHostName = "kr";
                    break;
                case 5:
                    serverHostName = "br";
                    break;
                case 6:
                    serverHostName = "ru";
                    break;
                case 7:
                    serverHostName = "tr";
                    break;
                default:
                    break;
            }
            return "chat." + serverHostName + ".lol.riotgames.com";
        }

        private void OnValidateCertificateEvent(object sender, CertificateEventArgs e)
        {
            e.AcceptCertificate = true;
        }

        private void OnAuthErrorEvent(object sender, SaslEventArgs e)
        {
            MessageBox.Show("The following authorization error has occured: \n" + e, "League of Legends Messenger Authorization Error", MessageBoxButton.OK, MessageBoxImage.Error);
            RelaunchCurrentProcess();
        }

        private void OnLoginEvent(object sender, Matrix.EventArgs args)
        {
            mainWindow.OnLogin(this);
        }

        private void OnErrorEvent(object sender, ExceptionEventArgs e)
        {
            MessageBox.Show(e.Exception.Message, "League of Legends Desktop Messenger Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
            RelaunchCurrentProcess();
        }

        private void OnRosterItemEvent(object sender, RosterEventArgs e)
        {
            mainWindow.AddToRoster(e);
        }

        private void OnStreamErrorEvent(object sender, StreamErrorEventArgs e)
        {
            MessageBox.Show("The following stream error has occured: \n" + e.Error.ToString(), "League of Legends Messenger Stream Error", MessageBoxButton.OK, MessageBoxImage.Error);
            RelaunchCurrentProcess();
        }

        public static string getFormattedStatus(int profileIcon, string statusMsg, int level, int wins, int leaves, string queueType, int rankedWins, int rankedLosses, int rankedRating, string tier, string skinname, string gameStatus, uint timeStamp)
        {
            return "<body><profileIcon>" +
                profileIcon + "</profileIcon><statusMsg>" +
                statusMsg + "</statusMsg><level>" +
                level + "</level><wins>" +
                wins + "</wins><leaves>" +
                leaves + "</leaves><queueType>" +
                queueType + "</queueType><rankedWins>" +
                rankedWins + "</rankedWins><rankedLosses>" +
                rankedLosses + "</rankedLosses><rankedRating>" +
                rankedRating + "</rankedRating><tier>" +
                tier + "</tier><skinname>" +
                skinname + "</skinname><gameStatus>" +
                gameStatus + "</gameStatus><timeStamp>" +
                timeStamp + "</timeStamp></body>";
        }

        public void SetPresence(string status)
        {
            SendPresence(Matrix.Xmpp.Show.chat, status);
        }



        private void RelaunchCurrentProcess()
        {
            Process.Start(Process.GetCurrentProcess().MainModule.FileName);
            Process.GetCurrentProcess().Kill();
        }
    }
}
