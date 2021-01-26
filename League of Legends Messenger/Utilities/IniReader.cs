using System.Runtime.InteropServices;
using System.Text;

namespace League_of_Legends_Messenger.Utilities
{
    class IniReader
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
        private static extern uint GetPrivateProfileString(string lpAppName, string lpKeyName, string lpDefault, StringBuilder lpReturnedString, uint nSize, string lpFileName);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
        static extern uint GetPrivateProfileInt(string lpAppName, string lpKeyName, int nDefault, string lpFileName);

        private string filePath;
        public IniReader(string path)
        {
            filePath = path;
        }

        public string Read(string lpAppName, string lpKeyName)
        {
            StringBuilder readString = new StringBuilder(512);
            GetPrivateProfileString(lpAppName, lpKeyName, "", readString, (uint)readString.Capacity, filePath);
            return readString.ToString();
        }

        public int ReadInt(string lpAppName, string lpKeyName)
        {
            return (int)GetPrivateProfileInt(lpAppName, lpKeyName, 0, filePath);
        }

        public int ReadInt(string lpAppName, string lpKeyName, int defaultInt)
        {
            return (int)GetPrivateProfileInt(lpAppName, lpKeyName, defaultInt, filePath);
        }
    }
}
