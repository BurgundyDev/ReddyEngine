#include "Engine/PFX.h"


namespace Engine
{
    PFXRef PFX::createFromFile(const std::string& filename)
    {
        auto pRet = std::shared_ptr<PFX>(new PFX());
        return pRet;
    }
}
