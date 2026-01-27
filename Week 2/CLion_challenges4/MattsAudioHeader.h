#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <mmsystem.h>
#endif

// WAV file header
#pragma pack(push, 1)
struct WaveHeader
{
    char chunkID[4] = { 'R', 'I', 'F', 'F' };
    uint32_t chunkSize;
    char format[4] = { 'W', 'A', 'V', 'E' };
    char subChunk1ID[4] = { 'f', 'm', 't', ' ' };
    uint32_t subChunk1Size = 16;
    uint16_t audioFormat = 1;
    uint16_t numChannels = 1;
    uint32_t sampleRate = 44100;
    uint32_t byteRate = 44100 * 2;
    uint16_t blockAlign = 2;
    uint16_t bitsPerSample = 16;
    char subChunk2ID[4] = { 'd', 'a', 't', 'a' };
    uint32_t subChunk2Size;

    WaveHeader(uint32_t samplingFrequency = 44100,
               uint16_t bitDepth = 16,
               uint16_t numberOfChannels = 1)
    {
        numChannels = numberOfChannels;
        sampleRate = samplingFrequency;
        bitsPerSample = bitDepth;

        byteRate = sampleRate * numChannels * bitsPerSample / 8;
        blockAlign = numChannels * bitsPerSample / 8;
    }

    void setFileSize(uint32_t numberOfFrames)
    {
        subChunk2Size = numberOfFrames * numChannels * bitsPerSample / 8;
        chunkSize = 36 + subChunk2Size;
    }
};
#pragma pack(pop)

// Get Desktop path
inline std::string getDesktopPath()
{
#if defined(_WIN32) || defined(_WIN64)
    const char* userProfile = std::getenv("USERPROFILE");
    return (userProfile ? std::string(userProfile) : "") + "\\Desktop\\";
#else
    const char* home = std::getenv("HOME");
    return (home ? std::string(home) : "") + "/Desktop/";
#endif
}

// Write float audio to WAV
inline void writeToWav(float* audio,
                       uint32_t numberOfSamples,
                       const char* filename,
                       bool toDesktop = true,
                       const char* path = "",
                       bool shouldPlay = true)
{
    std::ofstream fs;
    std::string basePath = (toDesktop ? getDesktopPath() : std::string(path));
    std::string filepath = basePath + filename;

    if (filepath.size() < 4 || filepath.substr(filepath.size() - 4) != ".wav")
        filepath += ".wav";

    fs.open(filepath, std::ios::binary);
    if (!fs.is_open())
    {
        std::cerr << "Failed to open " << filepath << " for writing.\n";
        return;
    }

    WaveHeader header{};
    header.setFileSize(numberOfSamples);
    fs.write(reinterpret_cast<char*>(&header), sizeof(WaveHeader));

    std::vector<int16_t> audioData(numberOfSamples);
    constexpr float max16Bit = 32768.0f;

    for (uint32_t i = 0; i < numberOfSamples; ++i)
    {
        int pcm = static_cast<int>(audio[i] * max16Bit);
        if (pcm >= max16Bit) pcm = static_cast<int>(max16Bit) - 1;
        if (pcm < -max16Bit) pcm = -static_cast<int>(max16Bit);
        audioData[i] = static_cast<int16_t>(pcm);
    }

    fs.write(reinterpret_cast<char*>(audioData.data()), header.subChunk2Size);
    fs.close();

    std::cout << filename << " written to: " << filepath << "\n";

// Not working
//     if (shouldPlay)
//     {
// #if defined(_WIN32) || defined(_WIN64)
//         // ANSI version of PlaySound, works with MinGW
//         PlaySoundA(filepath.c_str(), NULL, SND_FILENAME);
// #else
//         std::system((std::string("afplay ") + filepath).c_str());
// #endif
//     }
}

// Overload for vector
inline void writeToWav(std::vector<float>& audio,
                       const char* filename,
                       bool toDesktop = true,
                       const char* path = "",
                       bool shouldPlay = true)
{
    writeToWav(audio.data(), static_cast<uint32_t>(audio.size()), filename, toDesktop, path, shouldPlay);
}
