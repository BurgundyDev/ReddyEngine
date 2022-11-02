#if defined(WIN32)
#include <Windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <dirent/dirent.h>
#endif
#include "Engine/Utils.h"

#include <algorithm>


namespace Engine
{
    namespace Utils
    {
#if defined(WIN32)
        std::wstring utf8ToWide(const std::string& utf8)
        {
            auto requiredSize = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
            std::wstring ret;
            ret.resize(requiredSize - 1);
            MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, (LPWSTR)ret.c_str(), requiredSize);
            return std::move(ret);
        }

        std::string wideToUtf8(const std::wstring& wide)
        {
            auto requiredSize = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string ret;
            ret.resize(requiredSize - 1);
            WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, (LPSTR)ret.c_str(), requiredSize, nullptr, nullptr);
            return std::move(ret);
        }
#endif


        std::string getSavePath(const std::string& appName)
        {
#if defined(WIN32)
            PWSTR path = NULL;
            HRESULT r;

            r = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &path);
            if (path != NULL)
            {
                auto ret = wideToUtf8(path) + "/" + appName + "/";
                CreateDirectoryA(ret.c_str(), NULL);
                CoTaskMemFree(path);
                std::replace(ret.begin(), ret.end(), '\\', '/');
                return ret;
            }

            return "./";
#else
            return "./";
#endif
        }
    }
}
