#if defined(WIN32)
#include <Windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <dirent/dirent.h>
#endif
#include "Engine/Log.h"
#include "Engine/Utils.h"

#include <algorithm>
#include <fstream>


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
        
        std::vector<std::string> splitString(const std::string& in_string, char in_delimiter, bool in_removeEmptyElements)
        {
            std::vector<std::string> elems;
            unsigned int start = 0;
            unsigned int end = 0;
            for (; end < in_string.length(); ++end)
            {
                if (in_string[end] == in_delimiter)
                {
                    if (end - start || !in_removeEmptyElements)
                    {
                        elems.push_back(in_string.substr(start, end - start));
                    }
                    start = end + 1;
                }
            }
            if (!in_removeEmptyElements && start == in_string.length())
            {
                elems.push_back("");
            }
            if (start < in_string.length())
            {
                if (end - start || !in_removeEmptyElements)
                {
                    elems.push_back(in_string.substr(start, end - start));
                }
            }
            return elems;
        }

        std::vector<std::string> splitString(const std::string& in_string, const std::string& in_delimiters)
        {
            std::vector<std::string> elems;
            unsigned int start = 0;
            unsigned int end = 0;
            for (; end < in_string.length(); ++end)
            {
                for (auto c : in_delimiters)
                {
                    if (in_string[end] == c)
                    {
                        if (end - start)
                        {
                            elems.push_back(in_string.substr(start, end - start));
                        }
                        start = end + 1;
                    }
                }
            }
            if (start == in_string.length())
            {
                elems.push_back("");
            }
            if (start < in_string.length())
            {
                if (end - start)
                {
                    elems.push_back(in_string.substr(start, end - start));
                }
            }
            return elems;
        }

        std::string removeChars(const std::string& str, const std::string& charsToRemove)
        {
            auto ret = str;
            for (decltype(charsToRemove.size()) i = 0; i < charsToRemove.size(); ++i)
            {
                ret.erase(remove(ret.begin(), ret.end(), charsToRemove[i]), ret.end());
            }
            return ret;
        }

        std::string toUpper(const std::string& str)
        {
            auto ret = str;
            std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
            return ret;
        }

        std::string toLower(const std::string& str)
        {
            auto ret = str;
            std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
            return ret;
        }

        std::string trim(const std::string& str)
        {
            auto s = str.find_first_not_of(" \t\n\r");
            if (s == std::string::npos)
            {
                return "";
            }
            std::string ret;
            if (s > 0)
            {
                ret = str.substr(s);
            }
            else
            {
                ret = str;
            }
            s = ret.find_last_not_of(" \t\n\r");
            return ret.substr(0, s + 1);
        }

        size_t utf8Length(const std::string& str)
        {
            int len = 0;
            auto s = str.data();
            while (*s) len += (*s++ & 0xc0) != 0x80;
            return len;
        }

        size_t utf8Pos(const std::string& str, size_t pos)
        {
            auto s = str.data();
            while (*s && pos)
            {
                if (*s & 0x80)
                {
                    if ((*s & 0xE0) == 0xC0)
                    {
                        ++s;
                    }
                    else if ((*s & 0xF0) == 0xE0)
                    {
                        s += 2;
                    }
                    else
                    {
                        s += 3;
                    }
                }
                --pos;
                ++s;
            }

            return s - str.data();
        }

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
        
        std::string getPath(const std::string& filename)
        {
            return filename.substr(0, filename.find_last_of("\\/"));
        }

        std::string getFilename(const std::string& path)
        {
            auto pos = path.find_last_of("\\/");
            if (pos == std::string::npos) return path;
            return path.substr(pos + 1);
        }

        std::string getPathWithoutExtension(const std::string& path)
        {
            auto pos = path.find_last_of('.');
            if (pos == std::string::npos) return path;
            return path.substr(0, pos);
        }

        std::string getFilenameWithoutExtension(const std::string& path)
        {
            auto filename = getFilename(path);
            auto pos = filename.find_last_of('.');
            if (pos == std::string::npos) return filename;
            return filename.substr(0, pos);
        }

        std::string getExtension(const std::string& filename)
        {
            auto pos = filename.find_last_of('.');
            if (pos == std::string::npos) return "";
            return toUpper(filename.substr(pos + 1));
        }

        std::string getParentFolderName(const std::string& filename)
        {
            auto path = getPath(filename);
            if (path.empty()) return "";
            return path.substr(0, path.find_last_of("\\/"));
        }

        std::string makeRelativePath(const std::string& in_path, const std::string& in_relativeTo)
        {
            auto path = in_path;
            if (path.size() >= 2 && path[0] == '.' && (path[1] == '\\' || path[1] == '/'))
                path = path.substr(2);
            std::replace(path.begin(), path.end(), '\\', '/');
            auto pathSplit = splitString(path, '/');

            auto relativeTo = in_relativeTo;
            std::replace(relativeTo.begin(), relativeTo.end(), '\\', '/');
            auto relativeSplit = splitString(relativeTo, '/');

            while (pathSplit.size() && relativeSplit.size() && pathSplit.front() == relativeSplit.front())
            {
                pathSplit.erase(pathSplit.begin());
                relativeSplit.erase(relativeSplit.begin());
            }

            std::stringstream ss;
            bool bFirst = true;
            for (auto& folder : relativeSplit)
            {
                if (!bFirst) ss << "/";
                bFirst = false;
                ss << "..";
            }
            for (auto& folder : pathSplit)
            {
                if (!bFirst) ss << "/";
                bFirst = false;
                ss << folder;
            }
            return std::move(ss.str());
        }
        
        bool loadJson(Json::Value &out, const std::string& filename)
        {
            std::ifstream file(filename);
            if (!file.is_open())
            {
                CORE_ERROR("Failed to load file: " + filename);
                return false;
            }
            try
            {
                file >> out;
            }
            catch (...)
            {
                CORE_ERROR("Failed to parse file: " + filename);
                file.close();
                return false;
            }
            file.close();
            return true;
        }

        bool saveJson(const Json::Value &json, const std::string& filename)
        {
            std::ofstream file(filename);
            if (!file.is_open())
            {
                CORE_ERROR("Failed to save file: " + filename);
                return false;
            }

            Json::StyledWriter styledWriter;

            auto str = styledWriter.write(json);
            file << str;
            file.close();

            return true;
        }


        Json::Value serializeInt8(int8_t val)
        {
            return val;
        }

        Json::Value serializeUInt8(uint8_t val)
        {
            return val;
        }

        Json::Value serializeInt16(int16_t val)
        {
            return val;
        }

        Json::Value serializeUInt16(uint16_t val)
        {
            return val;
        }

        Json::Value serializeInt32(int32_t val)
        {
            return val;
        }

        Json::Value serializeUInt32(uint32_t val)
        {
            return val;
        }

        Json::Value serializeInt64(int64_t val)
        {
            return val;
        }

        Json::Value serializeUInt64(uint64_t val)
        {
            return val;
        }

        Json::Value serializeFloat(float val)
        {
            return val;
        }

        Json::Value serializeDouble(double val)
        {
            return val;
        }

        Json::Value serializeBool(bool val)
        {
            return val;
        }

        Json::Value serializeString(const std::string &val)
        {
            return val;
        }

        Json::Value serializeFloat2(const float *val)
        {
            Json::Value ret(Json::arrayValue);
            ret.append(val[0]);
            ret.append(val[1]);
            return ret;
        }

        Json::Value serializeFloat3(const float *val)
        {
            Json::Value ret(Json::arrayValue);
            ret.append(val[0]);
            ret.append(val[1]);
            ret.append(val[2]);
            return ret;
        }

        Json::Value serializeFloat4(const float *val)
        {
            Json::Value ret(Json::arrayValue);
            ret.append(val[0]);
            ret.append(val[1]);
            ret.append(val[2]);
            ret.append(val[3]);
            return ret;
        }

        Json::Value serializeInt2(const int *val)
        {
            Json::Value ret(Json::arrayValue);
            ret.append(val[0]);
            ret.append(val[1]);
            return ret;
        }

        Json::Value serializeInt4(const int *val)
        {
            Json::Value ret(Json::arrayValue);
            ret.append(val[0]);
            ret.append(val[1]);
            ret.append(val[2]);
            ret.append(val[3]);
            return ret;
        }

        Json::Value serializeMatrix(const float *val)
        {
            Json::Value ret(Json::arrayValue);
            for (int i = 0; i < 16; ++i)
                ret.append(val[i]);
            return ret;
        }

        Json::Value serializeStringArray(const std::vector<std::string> &val)
        {
            Json::Value ret(Json::arrayValue);
            for (const auto &str : val)
                ret.append(str);
            return ret;
        }

        int8_t deserializeInt8(const Json::Value &json, int8_t in_default)
        {
            if (!json.isInt()) return in_default;
            return (int8_t)json.asInt();
        }

        uint8_t deserializeUInt8(const Json::Value &json, uint8_t in_default)
        {
            if (!json.isUInt()) return in_default;
            return (uint8_t)json.asUInt();
        }

        int16_t deserializeInt16(const Json::Value &json, int16_t in_default)
        {
            if (!json.isInt()) return in_default;
            return (int16_t)json.asInt();
        }

        uint16_t deserializeUInt16(const Json::Value &json, uint16_t in_default)
        {
            if (!json.isUInt()) return in_default;
            return (uint16_t)json.asUInt();
        }

        int32_t deserializeInt32(const Json::Value &json, int32_t in_default)
        {
            if (!json.isInt()) return in_default;
            return (int32_t)json.asInt();
        }

        uint32_t deserializeUInt32(const Json::Value &json, uint32_t in_default)
        {
            if (!json.isUInt()) return in_default;
            return (uint32_t)json.asUInt();
        }

        int64_t deserializeInt64(const Json::Value &json, int64_t in_default)
        {
            if (!json.isInt64()) return in_default;
            return (int64_t)json.asInt64();
        }

        uint64_t deserializeUInt64(const Json::Value &json, uint64_t in_default)
        {
            if (!json.isUInt64()) return in_default;
            return (uint64_t)json.asUInt64();
        }

        float deserializeFloat(const Json::Value &json, float in_default)
        {
            if (!json.isDouble()) return in_default;
            return json.asFloat();
        }

        double deserializeDouble(const Json::Value &json, double in_default)
        {
            if (!json.isDouble()) return in_default;
            return json.asDouble();
        }

        bool deserializeBool(const Json::Value &json, bool in_default)
        {
            if (!json.isBool()) return in_default;
            return json.asBool();
        }

        std::string deserializeString(const Json::Value &json, const std::string &in_default)
        {
            if (!json.isString()) return in_default;
            return json.asString();
        }

        static const float DEFAULT_FLOATS[4]  = {0, 0, 0, 0};
        static const int   DEFAULT_INTS[4]    = {0, 0, 0, 0};
        static const float DEFAULT_MATRIX[16] = {1, 0, 0, 0,
                                                 0, 1, 0, 0,
                                                 0, 0, 1, 0,
                                                 0, 0, 0, 1};

        void deserializeFloat2(float* out, const Json::Value &json, const float *in_default)
        {
            if (in_default == nullptr) in_default = DEFAULT_FLOATS;
            if (!json.isArray() || json.size() != 2)
            {
                memcpy(out, in_default, sizeof(float) * 2);
                return;
            }
            for (int i = 0; i < 2; ++i)
            {
                if (!json[i].isDouble())
                {
                    memcpy(out, in_default, sizeof(float) * 2);
                    return;
                }
                out[i] = json[i].asFloat();
            }
        }

        void deserializeFloat3(float* out, const Json::Value &json, const float *in_default)
        {
            if (in_default == nullptr) in_default = DEFAULT_FLOATS;
            if (!json.isArray() || json.size() != 3)
            {
                memcpy(out, in_default, sizeof(float) * 3);
                return;
            }
            for (int i = 0; i < 3; ++i)
            {
                if (!json[i].isDouble())
                {
                    memcpy(out, in_default, sizeof(float) * 3);
                    return;
                }
                out[i] = json[i].asFloat();
            }
        }

        void deserializeFloat4(float* out, const Json::Value &json, const float *in_default)
        {
            if (in_default == nullptr) in_default = DEFAULT_FLOATS;
            if (!json.isArray() || json.size() != 4)
            {
                memcpy(out, in_default, sizeof(float) * 4);
                return;
            }
            for (int i = 0; i < 4; ++i)
            {
                if (!json[i].isDouble())
                {
                    memcpy(out, in_default, sizeof(float) * 4);
                    return;
                }
                out[i] = json[i].asFloat();
            }
        }

        void deserializeInt2(int* out, const Json::Value &json, const int *in_default)
        {
            if (in_default == nullptr) in_default = DEFAULT_INTS;
            if (!json.isArray() || json.size() != 2)
            {
                memcpy(out, in_default, sizeof(int) * 2);
                return;
            }
            for (int i = 0; i < 2; ++i)
            {
                if (!json[i].isInt())
                {
                    memcpy(out, in_default, sizeof(int) * 2);
                    return;
                }
                out[i] = (int)json[i].asInt();
            }
        }

        void deserializeInt4(int* out, const Json::Value &json, const int *in_default)
        {
            if (in_default == nullptr) in_default = DEFAULT_INTS;
            if (!json.isArray() || json.size() != 4)
            {
                memcpy(out, in_default, sizeof(int) * 4);
                return;
            }
            for (int i = 0; i < 4; ++i)
            {
                if (!json[i].isInt())
                {
                    memcpy(out, in_default, sizeof(float) * 4);
                    return;
                }
                out[i] = (int)json[i].asInt();
            }
        }

        void deserializeMatrix(float* out, const Json::Value &json, const float *in_default)
        {
            if (in_default == nullptr) in_default = DEFAULT_MATRIX;
            if (!json.isArray() || json.size() != 16)
            {
                memcpy(out, in_default, sizeof(float) * 16);
                return;
            }
            for (int i = 0; i < 16; ++i)
            {
                if (!json[i].isDouble())
                {
                    memcpy(out, in_default, sizeof(float) * 16);
                    return;
                }
                out[i] = json[i].asFloat();
            }
        }

        std::vector<std::string> deserializeStringArray(const Json::Value &json, const std::vector<std::string> &in_default)
        {
            if (!json.isArray())
            {
                return in_default;
            }
            std::vector<std::string> ret;
            for (const auto &valJson : json)
            {
                if (valJson.isString())
                {
                    ret.push_back(valJson.asString());
                }
                else
                {
                    ret.push_back("");
                }
            }
            return ret;
        }
    }
}
