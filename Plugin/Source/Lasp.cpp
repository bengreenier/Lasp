#include "pa_debugprint.h"
#include "IUnityInterface.h"
#include "Driver.h"
#include <memory>
#include <string>

namespace
{
    void PaPrintCallback(const char* log)
    {
        std::string temp(log);
        // Remove tailing newline character.
        if (!temp.empty() && temp.back() == '\n') temp.pop_back();
        Lasp::Debug::log("PortAudio> %s", temp.c_str());
    }

    struct LaspDeviceInfo
    {
		// indicates a successful operation
		bool isValid;

		int id;
        const char* name;
        double defaultSampleRate;
        int maxInputChannels;
        int maxOutputChannels;
    };
}

extern "C"
{
    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
    {
        PaUtil_SetDebugPrintFunction(PaPrintCallback);

        auto err = Pa_Initialize();
        if (err == paNoError)
            LASP_LOG("Initialized.");
        else
            LASP_PAERROR("Initialization failed.", err);
    }

    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
    {
		auto err = Pa_Terminate();

		if (err == paNoError)
			LASP_LOG("Termianted.");
		else
			LASP_PAERROR("Termination failed.", err);
    }

    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspReplaceLogger(Lasp::Debug::LogFunction p)
    {
        Lasp::Debug::setLogFunction(p);
    }

    int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspGetDeviceCount()
    {
        return Pa_GetDeviceCount();
    }

    LaspDeviceInfo UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspGetDeviceInfo(int deviceId = -1)
    {
		if (deviceId == -1)
		{
			deviceId = Pa_GetDefaultInputDevice();
		}

        LaspDeviceInfo info;
		
		info.isValid = false;

        auto deviceInfo = Pa_GetDeviceInfo(deviceId);

		if (deviceInfo != NULL)
		{
			info.isValid = true;
			info.id = deviceId;
			info.name = deviceInfo->name;
			info.defaultSampleRate = deviceInfo->defaultSampleRate;
			info.maxInputChannels = deviceInfo->maxInputChannels;
			info.maxOutputChannels = deviceInfo->maxOutputChannels;
		}

        return info;
    }

    void UNITY_INTERFACE_EXPORT * UNITY_INTERFACE_API LaspCreateDriver(int deviceId = -1)
    {
        return new Lasp::Driver(deviceId);
    }

    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspDeleteDriver(void* driver)
    {
        delete reinterpret_cast<Lasp::Driver*>(driver);
    }

    bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspOpenStream(void* driver, int channelCount = 1)
    {
        return reinterpret_cast<Lasp::Driver*>(driver)->OpenStream(channelCount);
    }

    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspCloseStream(void* driver)
    {
        reinterpret_cast<Lasp::Driver*>(driver)->CloseStream();
    }

    float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspGetSampleRate(void* driver)
    {
        return reinterpret_cast<Lasp::Driver*>(driver)->getSampleRate();
    }

    float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspGetPeakLevel(void* driver, int bufferIndex, float duration)
    {
        auto pd = reinterpret_cast<Lasp::Driver*>(driver);
        auto range = static_cast<size_t>(pd->getSampleRate() * duration);
        return pd->getBuffer(bufferIndex).getPeakLevel(range);
    }

    float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspCalculateRMS(void* driver, int bufferIndex, float duration)
    {
        auto pd = reinterpret_cast<Lasp::Driver*>(driver);
        auto range = static_cast<size_t>(pd->getSampleRate() * duration);
        return pd->getBuffer(bufferIndex).calculateRMS(range);
    }

    int32_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API LaspRetrieveWaveform(void* driver, int bufferIndex, float* dest, int32_t length)
    {
        auto pd = reinterpret_cast<Lasp::Driver*>(driver);
        auto& buffer = pd->getBuffer(bufferIndex);
        buffer.copyRecentFrames(dest, length);
        return std::min(length, static_cast<int32_t>(buffer.getSize()));
    }
}
