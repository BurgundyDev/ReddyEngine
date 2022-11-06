#pragma once

#include <memory>
#include <string>

#include "Resource.h"


namespace Engine
{
    class PFX;
    using PFXRef = std::shared_ptr<PFX>;


    class PFX final : public Resource
    {
    public:
        static PFXRef createFromFile(const std::string& filename);

    private:
        PFX() {}
    };
}
