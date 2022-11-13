// This manager ensures that there's always only 1 music playing at the same time.
// Eventually, this could be used to fade between them.

#pragma once

#include <memory>
#include <string>


namespace Engine
{
    class Music;
    using MusicRef = std::shared_ptr<Music>;


    class MusicManager final
    {
    public:
        void play(const std::string& filename);
        void stop();
        void pause();
        void resume();

    private:
        MusicRef m_pMusic;
    };
}
