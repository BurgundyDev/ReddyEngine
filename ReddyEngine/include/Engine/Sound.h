#pragma once

#include <Engine/Audio.h>

#include <atomic>
#include <memory>
#include <string>


namespace Engine
{
    class Sound;
    using SoundRef = std::shared_ptr<Sound>;

    class SoundInstance;
    using SoundInstanceRef = std::shared_ptr<SoundInstance>;


    class Sound final
    {
    public:
        static SoundRef createFromFile(const std::string& filename);
        static SoundRef createFromData(const float* pSamples, int frameCount, int channelCount, int samplerate);

        ~Sound();

        void play(float volume = 1.f, float balance = 0.f, float pitch = 1.f);

    private:
        friend class SoundInstance;

        Sound() {}

        float* m_pBuffer = nullptr;
        int m_frameCount = 0;
        int m_channelCount = 0;
    };


    class SoundInstance final : public AudioStream, public std::enable_shared_from_this<AudioStream>
    {
    public:
        SoundInstance(const SoundRef& pSound);
        SoundInstance(Sound* pSound);

        void play();
        void pause();
        void stop();

        bool isPlaying() const;
        bool isPaused() const;

        bool getLoop() const;
        void setLoop(bool loop);

        float getVolume() const;
        void setVolume(float volume);

        float getBalance() const;
        void setBalance(float balance);

        float getPitch() const;
        void setPitch(float pitch);

        void onStopped() override;

        bool progress(int frameCount, int sampleRate, int channelCount, float* pOut, float volume = 1.0f, float balance = 0.0f, float pitch = 1.0f) override;

    private:
        bool m_isPaused = true;
        std::atomic<bool> m_loop;
        std::atomic<float> m_volume;
        std::atomic<float> m_balance;
        std::atomic<float> m_pitch;
        std::atomic<int> m_offset;
        Sound* m_pSound = nullptr;
    };
}
