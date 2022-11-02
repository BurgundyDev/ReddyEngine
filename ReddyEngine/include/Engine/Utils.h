#pragma once

#include <string>

namespace Engine
{
    namespace Utils
    {
        std::wstring utf8ToWide(const std::string& utf8);
        std::string wideToUtf8(const std::wstring& wide);
        std::string getSavePath(const std::string& appName);
    }
}
