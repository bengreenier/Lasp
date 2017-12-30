namespace Lasp
{
    public struct LaspDeviceInfo
    {
        // indicates this struct is correctly populated (operation was successful)
        public bool isValid;

        public int id;
        public string name;
        public double defaultSampleRate;
        public int maxInputChannels;
        public int maxOutputChannels;
    }
}
