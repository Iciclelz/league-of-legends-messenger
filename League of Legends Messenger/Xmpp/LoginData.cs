using System;

namespace League_of_Legends_Messenger.Xmpp
{
    public class LoginData
    {
        public string Username { get; set; }
        public string Password { get; set; }
        public string ApiKey { get; set; }
        public int ServerId { get; set; }
        public string ServerName { get; set; }

        public LoginData(string username, String apiKey, Int32 serverId)
        {
            Username = username;
            Password = "";
            ApiKey = apiKey;
            ServerId = serverId;
            ServerName = getServerName();
        }

        public LoginData(String username, String password, String apiKey, Int32 serverId)
        {
            Username = username;
            Password = password;
            ApiKey = apiKey;
            ServerId = serverId;
            ServerName = getServerName();
        }

        public LoginData(String username, String password, String apiKey, Int32 serverId, String serverName)
        {
            Username = username;
            Password = password;
            ApiKey = apiKey;
            ServerId = serverId;
            ServerName = serverName;
        }

        private string getServerName()
        {
            switch (ServerId)
            {
                case 0:
                    return "North America";
                case 1:
                    return "EU West";
                case 2:
                    return "EU Nordic and East";
                case 3:
                    return "Oceania";
                case 4:
                    return "Korea";
                case 5:
                    return "Brazil";
                case 6:
                    return "Russia";
                case 7:
                    return "Turkey";
                default:
                    return "";
            }
        }
    }
}
