using Matrix.Xmpp.Client;
using System;
using System.Linq;
using System.Text.RegularExpressions;

namespace League_of_Legends_Messenger.Xmpp
{
    class SummonerData
    {
        public String ProfileIconId;
        public String Level;
        public String Wins;
        public String Leaves;
        public String Tier;
        public String ChampionMasteryScore;
        public String RankedLeagueName;
        public String RankedLeagueDivison;
        public String RankedLeagueTier;
        public String RankedWins;
        public String StatusMessage;
        public AvailabilityEnum Availability;
        
        public SummonerData(Presence presence)
        {
            string status = presence.Show.ToString();
            if (status.CompareTo("chat") == 0)
            {
                Availability = AvailabilityEnum.available;
            }
            else if (status.CompareTo("dnd") == 0)
            {
                Availability = AvailabilityEnum.ingame;
            }
            else
            {
                Availability = AvailabilityEnum.offline;
            }

            string xml = presence.Value;
            try { ProfileIconId = RegexMatch("<profileIcon>(.*?)</profileIcon>", xml); } catch { }
            try { Level = RegexMatch("<level>(.*?)</level>", xml); } catch { }
            try { Wins = RegexMatch("<wins>(.*?)</wins>", xml); } catch { }
            try { Leaves = RegexMatch("<leaves>(.*?)</leaves>", xml); } catch { }
            try { Tier = FirstCharToUpper(RegexMatch("<tier>(.*?)</tier>", xml).ToLower()); } catch { }
            try { ChampionMasteryScore = RegexMatch("<championMasteryScore>(.*?)</championMasteryScore>", xml); } catch { }
            try { RankedLeagueName = RegexMatch("<rankedLeagueName>(.*?)</rankedLeagueName>", xml); } catch { }
            try { RankedLeagueDivison = RegexMatch("<rankedLeagueDivision>(.*?)</rankedLeagueDivision>", xml).Replace("&apos;s", ""); } catch { }
            try { RankedLeagueTier = RegexMatch("<rankedLeagueTier>(.*?)</rankedLeagueTier>", xml); } catch { }
            try { RankedWins = RegexMatch("<rankedWins>(.*?)</rankedWins>", xml); } catch { }
            try { StatusMessage = RegexMatch("<statusMsg>(.*?)</statusMsg>", xml); } catch { }
       }

        
        public String GetSummonerData()
        {
            return "<" + FirstCharToUpper(Availability.ToString().ToLower()) + ":" + StatusMessage + "> " + RankedLeagueTier + " " + RankedLeagueDivison + " - " + RankedLeagueName + "(Wins: " + RankedWins + ") " + "- Level " + Level + ", Score: " + ChampionMasteryScore;
        }

        public static string RegexMatch(string regex, string body)
        {
            return new Regex(regex).Matches(body)[0].Groups[1].ToString();
        }

        public static string FirstCharToUpper(string input)
        {
            if (String.IsNullOrEmpty(input))
                throw new ArgumentException("ARGH!");
            return input.First().ToString().ToUpper() + input.Substring(1);
        }

        public enum AvailabilityEnum
        {
            available = 1,
            ingame = 2,
            offline = 3
        }
    }
}
