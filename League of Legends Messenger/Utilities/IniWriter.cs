using System.Runtime.InteropServices;

namespace League_of_Legends_Messenger.Utilities
{
    class IniWriter
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool WritePrivateProfileString(string lpAppName, string lpKeyName, string lpString, string lpFileName);

        private string filePath;
        public IniWriter(string path)
        {
            filePath = path;
        }
        public bool Write(string lpAppName, string lpKeyName, string lpString)
        {
            return WritePrivateProfileString(lpAppName, lpKeyName, lpString, filePath);
        }

        public bool WriteInt(string lpAppName, string lpKeyName, int iInt)
        {
            return WritePrivateProfileString(lpAppName, lpKeyName, iInt.ToString(), filePath);
        }
    }
}
