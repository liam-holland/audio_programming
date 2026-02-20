/*
 MattsAudioHeader.h
 A single header #include to start experimenting with audio output in C++

 Current Functionality

 - Sample rate fixed at 44100
 - bit depth fixed at 16-bit
 - Channel config fixed to mono
 */

#pragma once

 //------------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <fstream>
#include <cstddef>
#include <cstdlib>
#include <cstdint>

#if defined _WIN32 || defined _WIN64
#pragma comment(lib, "Winmm")
#include <windows.h>
#endif
//------------------------------------------------------------------------------
/// Header for a WAVE format file. defaults to 16-bit, 44100 Hz Mono
struct WaveHeader
{
    /** waveFormatHeader: The first 4 bytes of a wav file should be the characters "RIFF" */
    char chunkID[4] = { 'R', 'I', 'F', 'F' };
    /** waveFormatHeader: This is the size of the entire file in bytes minus 8 bytes */
    uint32_t chunkSize;
    /** waveFormatHeader" The should be characters "WAVE" */
    char format[4] = { 'W', 'A', 'V', 'E' };
    /** waveFormatHeader" This should be the letters "fmt ", note the space character */
    char subChunk1ID[4] = { 'f', 'm', 't', ' ' };
    /** waveFormatHeader: For PCM == 16, since audioFormat == uint16_t */
    uint32_t subChunk1Size = 16;
    /** waveFormatHeader: For PCM this is 1, other values indicate compression */
    uint16_t audioFormat = 1;
    /** waveFormatHeader: Mono = 1, Stereo = 2, etc. */
    uint16_t numChannels = 1;
    /** waveFormatHeader: Sample Rate of file */
    uint32_t sampleRate = 44100;
    /** waveFormatHeader: SampleRate * NumChannels * BitsPerSample/8 */
    uint32_t byteRate = 44100 * 2;
    /** waveFormatHeader: The number of bytes for one sample including all channels */
    uint16_t blockAlign = 2;
    /** waveFormatHeader: 8 bits = 8, 16 bits = 16 */
    uint16_t bitsPerSample = 16;
    /** waveFormatHeader: Contains the letters "data" */
    char subChunk2ID[4] = { 'd', 'a', 't', 'a' };
    /** waveFormatHeader: == NumberOfFrames * NumChannels * BitsPerSample/8
     This is the number of bytes in the data.
     */
    uint32_t subChunk2Size;

    WaveHeader(uint32_t samplingFrequency = 44100, uint16_t bitDepth = 16, uint16_t numberOfChannels = 1)
    {
        numChannels = numberOfChannels;
        sampleRate = samplingFrequency;
        bitsPerSample = bitDepth;

        byteRate = sampleRate * numChannels * bitsPerSample / 8;
        blockAlign = numChannels * bitsPerSample / 8;
    };

    /// sets the fields that refer to how large the wave file is
    /// @warning This MUST be set before writing a file, or the file will be unplayable.
    /// @param numberOfFrames total number of audio frames. i.e. total number of samples / number of channels
    void setFileSize(uint32_t numberOfFrames)
    {
        subChunk2Size = numberOfFrames * numChannels * bitsPerSample / 8;
        chunkSize = 36 + subChunk2Size;
    }

};



/// write an array of float data to a 16-bit, 44100 Hz Mono wav file
/// @param audio audio samples, assumed to be 44100 Hz sampling rate
/// @param numberOfSamples total number of samples in audio
/// @param filename filename, should end in .wav and will be written to your Desktop
void writeToWav(float* audio,
    uint32_t numberOfSamples,
    const char* filename,
    const bool toDesktop = true,
    const char* path = "",
    const bool shouldPlay = true)
{
    std::ofstream fs;

#if defined _WIN32 || defined _WIN64
    const char* userProfile = std::getenv("USERPROFILE");
    std::string desktop = (userProfile ? std::string(userProfile) : std::string("")) + "\\Desktop\\";
#else
    std::string desktop = std::string(getenv("HOME")) + std::string("/Desktop/");
#endif

    std::string filepath = ((toDesktop) ? desktop : std::string(path)) + std::string(filename);

    if (filepath.substr(filepath.size() - 4, 4) != std::string(".wav"))
        filepath += std::string(".wav");

    fs.open(filepath, std::fstream::out | std::ios::binary);

    WaveHeader* header = new WaveHeader{};
    header->setFileSize(numberOfSamples);

    fs.write((char*)header, sizeof(WaveHeader));

    int16_t* audioData = new int16_t[numberOfSamples];
    constexpr float max16BitValue = 32768.0f;

    for (int i = 0; i < numberOfSamples; ++i)
    {
        int pcm = int(audio[i] * (max16BitValue));

        if (pcm >= max16BitValue)
            pcm = max16BitValue - 1;
        else if (pcm < -max16BitValue)
            pcm = -max16BitValue;

        audioData[i] = int16_t(pcm);
    }

    fs.write((char*)audioData, header->subChunk2Size);

    fs.close();
    std::cout << filename << " written to:\n" << filepath << std::endl;

    if (shouldPlay)
    {
#if defined _WIN32 || defined _WIN64
        // don't forget to add Add 'Winmm.lib' in Properties > Linker > Input > Additional Dependencies
        PlaySoundW(std::wstring(filepath.begin(), filepath.end()).c_str(),
           NULL,
           SND_FILENAME);

#else
        std::system((std::string("afplay ") + filepath).c_str());
#endif
    }
}


/// write an vector of float data to a 16-bit, 44100 Hz Mono wav file
/// @param audio vector containing the audio samples, assumed to be 44100 Hz sampling rate
/// @param filename filename, should end in .wav and will be written to your Desktop
void writeToWav(std::vector<float>& audio,
                const char* filename,
                const bool toDesktop = true,
                const char* path = "",
                const bool shouldPlay = true)
{
    writeToWav(audio.data(), audio.size(), filename, toDesktop, path, shouldPlay);
}
