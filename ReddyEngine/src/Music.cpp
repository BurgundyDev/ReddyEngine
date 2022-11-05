#include "Engine/Music.h"
#include "Engine/Config.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Utils.h"

#include <functional>

#include <stb_vorbis.c> // Keep this header last


static const int MUSIC_BUFFER_COUNT = 5; // 5 seconds pre-buffer


namespace Engine
{
    MusicRef Music::createFromFile(const std::string& filename)
    {
        auto pRet = std::shared_ptr<Music>(new Music());
        pRet->m_filename = filename; // We don't open the file until it's played
        return pRet;
    }

    Music::Music()
        : m_volume(1.0f)
        , m_isPlaying(false)
        , m_loop(false)
    {
    }

    Music::~Music()
    {
        for (auto pBuffer : m_buffers) delete pBuffer;
        m_buffers.clear();

        //------
        // Copy pasted from Stop() with tiny modification... It's a gamejam, don't say anything..
        // stop();
        if (!m_isPlaying) return;
        m_isPlaying = false;
        m_paused = false;

        //if (Engine::getAudio() /* Risk of audio engine destroyed before we clean assets */) Engine::getAudio()->removeStream(shared_from_this());

        m_thread.join();

        stb_vorbis_close(m_pStream);

        m_pStream = nullptr;

        for (auto pBuffer : m_buffers) delete pBuffer;
        m_buffers.clear();
        m_bufferCount = 0;
        //------

        if (m_thread.joinable()) m_thread.join();
        if (m_pSamplings) delete[] m_pSamplings;
    }

    void Music::play(bool loop)
    {
        if (m_isPlaying) return;
        m_loop = loop;

        for (auto pBuffer : m_buffers) delete pBuffer;
        m_buffers.clear();
        m_bufferCount = 0;

        m_pStream = stb_vorbis_open_filename((char*)m_filename.c_str(), NULL, NULL);
        if (!m_pStream) return;

        auto info = stb_vorbis_get_info(m_pStream);
        m_sampleCount = stb_vorbis_stream_length_in_samples(m_pStream);

        m_isPlaying = true;
        m_paused = false;
        m_samplingT = 0.0f;

        // We will buffer 5 seconds
        m_engineChannelCount = m_pStream->channels;
        m_bufferMax = Engine::getAudio()->getSampleRate() * m_engineChannelCount;
        for (int i = 0; i < MUSIC_BUFFER_COUNT; ++i)
        {
            auto pBuffer = new Buffer();
            pBuffer->data.assign(m_bufferMax, 0.0f);
            m_buffers.push_back(pBuffer);
        }
        m_bufferCount = 0;

        if (m_thread.joinable()) m_thread.join();
        m_thread = std::thread(std::bind(&Music::run, this));

        m_musicSampleRate = (int)m_pStream->sample_rate;
        m_musicChannelCount = (int)m_pStream->channels;
        m_pSamplings = new float[4 * m_pStream->channels];
        memset(m_pSamplings, 0, sizeof(float) * 4 * m_pStream->channels);

        Engine::getAudio()->addStream(shared_from_this());
    }

    void Music::stop()
    {
        if (!m_isPlaying) return;
        m_isPlaying = false;
        m_paused = false;

        if (Engine::getAudio() /* Risk of audio engine destroyed before we clean assets */) Engine::getAudio()->removeStream(shared_from_this());

        m_thread.join();

        stb_vorbis_close(m_pStream);

        m_pStream = nullptr;

        for (auto pBuffer : m_buffers) delete pBuffer;
        m_buffers.clear();
        m_bufferCount = 0;
    }

    void Music::pause()
    {
        if (!m_paused && m_isPlaying)
        {
            m_paused = true;
            Engine::getAudio()->removeStream(shared_from_this());
        }
    }

    void Music::resume()
    {
        if (m_paused && m_isPlaying)
        {
            m_paused = false;
            Engine::getAudio()->addStream(shared_from_this());
        }
    }

    void Music::setVolume(float volume)
    {
        m_volume = volume;
    }

    bool Music::isDone()
    {
        return !m_isPlaying && m_done;
    }
    
    void Music::run()
    {
        while (m_isPlaying)
        {
            if (m_bufferCount < MUSIC_BUFFER_COUNT)
            {
                m_mutex.lock();
                auto pBuffer = m_buffers.back();
                m_buffers.pop_back();
                m_mutex.unlock();
                pBuffer->count = stb_vorbis_get_samples_float_interleaved(m_pStream, m_engineChannelCount, pBuffer->data.data(), m_bufferMax) * m_engineChannelCount;
                pBuffer->offset = 0;
                if (pBuffer->count)
                {
                    m_mutex.lock();
                    m_buffers.insert(m_buffers.begin() + m_bufferCount, pBuffer);
                    ++m_bufferCount;
                    m_mutex.unlock();
                }
                else
                {
                    if (m_loop)
                    {
                        stb_vorbis_seek_start(m_pStream);
                        m_mutex.lock();
                        m_buffers.push_back(pBuffer);
                        m_mutex.unlock();
                        continue;
                    }
                    else
                    {
                        // Done playing!
                        stb_vorbis_close(m_pStream);
                        m_pStream = nullptr;
                        m_done = true;
                        break;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    bool Music::progress(int frameCount, int sampleRate, int channelCount, float* pOut, float in_volume, float balance, float pitch)
    {
        if (!Engine::getAudio()) return false;
        if (!m_isPlaying) return true;

        float musicVolume = Config::musicVolume;
        float volume = m_volume * in_volume * musicVolume;
        int bufferCount = m_bufferCount;
        int sampleFed = 0;
        int requestSamples = frameCount * channelCount;
        auto pSamplings = m_pSamplings;

        float progress = (float)m_musicSampleRate / (float)sampleRate;
        int frameFed = 0;
        float t = m_samplingT;

        while (frameFed < frameCount && bufferCount > 0)
        {
            m_mutex.lock();
            auto pBuffer = m_buffers.front();
            m_mutex.unlock();
            int count = std::min((pBuffer->count - pBuffer->offset) / m_musicChannelCount, frameCount - frameFed);
            if (count == 0) break;
            float* pData = pBuffer->data.data() + pBuffer->offset;
            float* pOutData = pOut + (frameFed * channelCount);
            int oldIndex = 0;
            int index = 0;
            float sample;

            while (index < count && oldIndex < count)
            {
                if (channelCount == 1 && m_musicChannelCount == 1)
                {
                    pOutData[index] += Utils::resamplingHermite(
                        pSamplings[0],
                        pSamplings[1],
                        pSamplings[2],
                        pSamplings[3], t) * volume;
                }
                else if (channelCount > 1 && m_musicChannelCount == 1)
                {
                    sample = Utils::resamplingHermite(
                        pSamplings[0],
                        pSamplings[1],
                        pSamplings[2],
                        pSamplings[3], t) * volume;
                    for (int i = 0; i < 2; ++i)
                    {
                        pOutData[index * channelCount + i] += sample;
                    }
                }
                else
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        pOutData[index * channelCount + i] += Utils::resamplingHermite(
                            pSamplings[0 * m_musicChannelCount + i],
                            pSamplings[1 * m_musicChannelCount + i],
                            pSamplings[2 * m_musicChannelCount + i],
                            pSamplings[3 * m_musicChannelCount + i], t) * volume;
                    }
                }

                // Progress to the next sample
                t += progress;
                while (t >= 1.0f)
                {
                    if (oldIndex >= count) oldIndex = count - 1;
                    for (int i = 0; i < m_musicChannelCount; ++i)
                    {
                        pSamplings[0 * m_musicChannelCount + i] = pSamplings[1 * m_musicChannelCount + i];
                        pSamplings[1 * m_musicChannelCount + i] = pSamplings[2 * m_musicChannelCount + i];
                        pSamplings[2 * m_musicChannelCount + i] = pSamplings[3 * m_musicChannelCount + i];
                        pSamplings[3 * m_musicChannelCount + i] = pData[oldIndex * m_musicChannelCount + i];
                    }
                    ++oldIndex;
                    t -= 1.0f;
                }
                ++index;
            }
            frameFed += index;
            pBuffer->offset += oldIndex * m_musicChannelCount;
            if (pBuffer->offset >= pBuffer->count)
            {
                // Remove that buffer
                m_mutex.lock();
                m_buffers.erase(m_buffers.begin());
                m_buffers.push_back(pBuffer);
                --m_bufferCount;
                bufferCount = m_bufferCount;
                m_mutex.unlock();
            }

            if (bufferCount <= 0)
            {
                if (m_done)
                {
                    m_isPlaying = false;
                    m_samplingT = t;
                    return false;
                }
                break;
            }
        }

        m_samplingT = t;

        return true;
    }
}
