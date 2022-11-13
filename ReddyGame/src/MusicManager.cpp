#include "Engine/MusicManager.h"
#include "Engine/ResourceManager.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Music.h"

namespace Engine
{
    void MusicManager::play(const std::string& filename)
    {
        auto pNewMusic = getResourceManager()->getMusic(filename);
        if (pNewMusic && pNewMusic != m_pMusic)
        {
            if (m_pMusic) m_pMusic->stop();
            m_pMusic = pNewMusic;
            m_pMusic->play(true);
        }
    }

    void MusicManager::stop()
    {
        if (m_pMusic)
        {
            m_pMusic->stop();
            m_pMusic = nullptr;
        }
    }

    void MusicManager::pause()
    {
        if (m_pMusic)
            m_pMusic->pause();
    }

    void MusicManager::resume()
    {
        if (m_pMusic)
            m_pMusic->resume();
    }
}
