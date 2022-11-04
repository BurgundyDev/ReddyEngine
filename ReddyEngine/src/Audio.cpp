#include "Engine/Audio.h"
#include "Engine/Config.h"
#include "Engine/Log.h"


static const int AUDIO_FREQUENCY = 44100;
static const int AUDIO_CHANNELS = 2;
static const int AUDIO_SAMPLES = 512;


static void audioCallback(void *userdata, Uint8 *stream, int len)
{
    auto pAudio = (Engine::Audio*)userdata;
    pAudio->progress(stream, len);
}


namespace Engine
{
    Audio::Audio()
    {
        memset(&m_audioSpec, 0, sizeof(m_audioSpec));
        m_audioSpec.freq = AUDIO_FREQUENCY; // Should be good enuf?
        m_audioSpec.format = AUDIO_F32;
        m_audioSpec.channels = AUDIO_CHANNELS;
        m_audioSpec.samples = AUDIO_SAMPLES;
        m_audioSpec.callback = audioCallback;
        m_audioSpec.userdata = this;

        if (SDL_OpenAudio(&m_audioSpec, NULL) < 0)
        {
            CORE_ERROR("Failed to SDL_OpenAudio");
            return;
        }

        SDL_PauseAudio(0);

        m_audioEnabled = true;
    }

    Audio::~Audio()
    {
        if (m_audioEnabled)
        {
            SDL_CloseAudio();
        }
    }

    void Audio::progress(Uint8* stream, int len)
    {
        float* pBuffer = (float*)stream;
        memset(pBuffer, 0, sizeof(float) * AUDIO_SAMPLES * AUDIO_CHANNELS);

        int frameCount = len / sizeof(float) / AUDIO_CHANNELS;
        auto sampleRate = m_audioSpec.freq;
        auto channelCount = m_audioSpec.channels;

        
        {
            std::unique_lock<std::mutex> locker(m_streamsMutex);
            for (auto it = m_streams.begin(); it != m_streams.end();)
            {
                auto pInstance = it->lock();
                auto pInstancePtr = pInstance.get();
                if (!pInstancePtr)
                {
                    it = m_streams.erase(it);
                    continue;
                }

                float volume = 1.0f;
                float balance = 0.0f;
                float pitch = 1.0f;

                if (!pInstancePtr->progress(frameCount, sampleRate, channelCount, pBuffer, volume, balance, pitch))
                {
                    pInstancePtr->onStopped();
                    for (auto it2 = m_fireAndForgetStreams.begin(); it2 != m_fireAndForgetStreams.end(); ++it2)
                    {
                        if (pInstance == *it2)
                        {
                            m_fireAndForgetStreams.erase(it2);
                            break;
                        }
                    }
                    it = m_streams.erase(it);
                    continue;
                }

                ++it;
            }
        }

        // Apply master volume
        float masterVolume = Config::masterVolume; // Probably should put a lock on masterVolume, but whatever it's just a float!
        for (int i = 0, len = frameCount * channelCount; i < len; ++i)
        {
            pBuffer[i] *= masterVolume;
        }
    }
    
    void Audio::addStream(const AudioStreamRef& pStream)
    {
        if (!m_audioEnabled) return;

        std::lock_guard<std::mutex> locker(m_streamsMutex);
        m_streams.push_back(pStream);
    }

    void Audio::addFireAndForgetStream(const AudioStreamRef& pStream)
    {
        if (!m_audioEnabled) return;

        std::lock_guard<std::mutex> locker(m_streamsMutex);
        m_fireAndForgetStreams.push_back(pStream);
        m_streams.push_back(pStream);
    }

    void Audio::removeStream(const AudioStreamRef& pStream)
    {
        if (!m_audioEnabled) return;

        auto pStreamRaw = pStream.get();
        std::lock_guard<std::mutex> locker(m_streamsMutex);
        for (auto it = m_streams.begin(); it != m_streams.end(); ++it)
        {
            if (it->lock().get() == pStreamRaw)
            {
                m_streams.erase(it);
                break;
            }
        }
    }
}
