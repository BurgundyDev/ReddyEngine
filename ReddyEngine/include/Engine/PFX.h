#pragma once

#include <memory>
#include <string>


namespace Engine
{
    class PFX;
    using PFXRef = std::shared_ptr<PFX>;


    class PFX final
    {
    public:
        static PFXRef createFromFile(const std::string& filename);

    private:
        PFX() {}
    };
}
