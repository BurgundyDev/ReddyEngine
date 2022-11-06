// Audio Engine

#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include <SDL_audio.h>

#include "Resource.h"


namespace Engine
{
    class AudioStream : public Resource
    {
    public:
        // Returns false when completed
        virtual bool progress(int frameCount, int sampleRate, int channelCount, float* pOut, float volume = 1.0f, float balance = 0.0f, float pitch = 1.0f) = 0;
        virtual void onStopped() {};
    };

    using AudioStreamRef = std::shared_ptr<AudioStream>;
    using AudioStreamWeak = std::weak_ptr<AudioStream>;


    class Audio final : public Resource
    {
    public:
        Audio();
        ~Audio();

        void progress(Uint8* stream, int len);

        int getSampleRate() const { return m_audioSpec.freq; }
        int getChannels() const { return m_audioSpec.channels; }

        void addStream(const AudioStreamRef& pStream);
        void addFireAndForgetStream(const AudioStreamRef& pStream);
        void removeStream(const AudioStreamRef& pStream);

    private:
        std::mutex m_streamsMutex;
        std::vector<AudioStreamWeak> m_streams;
        std::vector<AudioStreamRef> m_fireAndForgetStreams;
        bool m_audioEnabled = false;
        SDL_AudioSpec m_audioSpec;
    };
}
