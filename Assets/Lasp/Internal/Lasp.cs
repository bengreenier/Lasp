using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Lasp
{
    public static class Lasp
    {
        public static int DeviceCount
        {
            get
            {
                return PluginEntry.GetDeviceCount();
            }
        }

        public static IEnumerable<LaspDeviceInfo> Devices
        {
            get
            {
                var devInfo = new List<LaspDeviceInfo>();
                for (var i = 0; i < DeviceCount; i++)
                {
                    devInfo.Add(PluginEntry.GetDeviceInfo(i));
                }

                return devInfo;
            }
        }

        public static LaspStream CreateStream()
        {
            // -1 uses default device
            return CreateStream(PluginEntry.DefaultDeviceId);
        }

        public static LaspStream CreateStream(LaspDeviceInfo deviceInfo)
        {
            return CreateStream(deviceInfo.id);
        }

        public static LaspStream CreateStream(int deviceId)
        {
            EnsureLoggingSetup();

            return new LaspStream(deviceId);
        }

        private static bool _isLoggingEnabled = false;

        private static void EnsureLoggingSetup()
        {
            if (!_isLoggingEnabled)
            {
                PluginEntry.SetupLogger();
                _isLoggingEnabled = true;
            }
        }
    }
}
