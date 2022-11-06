// Music asset is also the music instance. It's pointless to allow multiple instances of a music.. or is it?

#pragma once

#include <Engine/Audio.h>
#include "Engine/Resource.h"

#include <atomic>
#include <memory>
#include <string>
#include <vector>


struct stb_vorbis;


namespace Engine
{
    class Music;
    using MusicRef = std::shared_ptr<Music>;


    class Music final : public AudioStream, public std::enable_shared_from_this<AudioStream>
    {
    public:
        static MusicRef createFromFile(const std::string& filename);

        ~Music();

        void play(bool loop = false);
        void stop();
        void pause();
        void resume();
        void setVolume(float volume); // Separate from master music volume

        bool isPlaying() const { return m_isPlaying; }
        bool isPaused() const { return m_paused && m_isPlaying; }
        bool isDone();

        bool progress(int frameCount, int sampleRate, int channelCount, float* pOut, float volume = 1.0f, float balance = 0.0f, float pitch = 1.0f) override;

    private:
        struct Buffer
        {
            int offset = 0;
            int count = 0;
            std::vector<float> data;
        };

        Music();

        void run();
        
        std::mutex m_mutex;
        std::thread m_thread;
        std::atomic<float> m_volume;
        std::atomic<bool> m_isPlaying;
        std::atomic<bool> m_loop;
        std::vector<Buffer*> m_buffers;
        std::atomic<int> m_bufferCount;
        std::atomic<bool> m_done;
        float* m_pSamplings = nullptr;
        float m_samplingT = 0.0f;
        int m_musicSampleRate = 0;
        int m_musicChannelCount = 0;
        bool m_paused = false;
        unsigned int m_sampleCount = 0;
        int m_bufferMax = 0;
        int m_engineChannelCount = 44100;
        std::string m_filename;
        stb_vorbis* m_pStream = nullptr;
    };
}
