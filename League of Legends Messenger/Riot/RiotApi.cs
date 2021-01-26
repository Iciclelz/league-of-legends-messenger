using Matrix;
using System;
using System.Net;

namespace League_of_Legends_Messenger.Riot
{
    class RiotApi
    {
        private string apiKey;
        private string serverName;
        public RiotApi(string APIKey, string server)
        {
            apiKey = APIKey;
            serverName = server;
        }

        private string getAPIServerByName(string summonerName)
        {
            string server = "na";
            switch (serverName)
            {
                case "North America":
                    server = "na";
                    break;
                case "EU West":
                    server = "euw";
                    break;
                case "EU Nordic and East":
                    server = "eune";
                    break;
                case "Oceania":
                    server = "oce";
                    break;
                case "Korea":
                    server = "kr";
                    break;
                case "Brazil":
                    server = "br";
                    break;
                case "Russia":
                    server = "ru";
                    break;
                case "Turkey":
                    server = "tr";
                    break;
                default:
                    break;
            }
            return "https://" + server + ".api.pvp.net/api/lol/" + server + "/v1.4/summoner/by-name/" + summonerName + "?api_key=" + apiKey;
        }

        private string getAPIServerById(string summonerId)
        {
            string server = "na";
            switch (serverName)
            {
                case "North America":
                    server = "na";
                    break;
                case "EU West":
                    server = "euw";
                    break;
                case "EU Nordic and East":
                    server = "eune";
                    break;
                case "Oceania":
                    server = "oce";
                    break;
                case "Korea":
                    server = "kr";
                    break;
                case "Brazil":
                    server = "br";
                    break;
                case "Russia":
                    server = "ru";
                    break;
                case "Turkey":
                    server = "tr";
                    break;
                default:
                    break;
            }
            return "https://" + server + ".api.pvp.net/api/lol/" + server + "/v1.4/summoner/" + summonerId + "?api_key=" + apiKey;
        }

        public Jid getJid(string summonerName)
        {
            try
            {
                string response = new WebClient().DownloadString(getAPIServerByName(summonerName));
                return new RiotJid("sum" + response.Substring(response.IndexOf("{\"id\":") + 6).Split(new string[] { ",\"name\"" }, StringSplitOptions.None)[0]).getJid();
            }
            catch { return null; }

        }

        public string getSummonerName(string summonerId)
        {
            try
            {
                string[] parseSum = { "sum" };
                summonerId = summonerId.Substring(3).Split(parseSum, StringSplitOptions.None)[0];

                string response = new WebClient().DownloadString(getAPIServerById(summonerId));
                return response.Substring(response.IndexOf("\"name\":") + 8).Split(new string[]{ "\",\"profileIconId\":" }, StringSplitOptions.None)[0];
            }
            catch
            {
                return null;
            }

        }
    }
    class RiotJid
    {
        public string User = "";
        public RiotJid(string username)
        {
            setUsername(username);
        }
        public void setUsername(string username)
        {
            User = username;
        }
        public Jid getJid()
        {
            return new Jid(User, "pvp.net", "xiff");
        }
    }
}
