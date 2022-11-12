#if defined(WIN32)
#define _CRT_NONSTDC_NO_DEPRECATE
#include <Windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <dirent/dirent.h>
#endif
#include "Engine/Log.h"
#include "Engine/Utils.h"

#include <tinyfiledialogs/tinyfiledialogs.h>

#include <algorithm>
#include <fstream>
#include <regex>
#include <filesystem>

#include <SDL.h>


#define MAX_CONCATENATE_CHARS_LENGTH 1024


static void replace(std::string& source, const std::string& reg, const std::string& substitution)
{
    source = std::regex_replace(source, std::regex(reg), substitution);
}


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

        std::string findFile(const std::string& filename, const std::string& lookIn, bool deepSearch, bool ignoreCase)
        {
            DIR* dir;
            struct dirent* ent;
            if ((dir = opendir(lookIn.c_str())) != NULL)
            {
                while ((ent = readdir(dir)) != NULL)
                {
                    if (!strcmp(ent->d_name, "."))
                    {
                        continue;
                    }
                    else if (!strcmp(ent->d_name, ".."))
                    {
                        continue;
                    }

                    if (ignoreCase)
                    {
                        if (stricmp(filename.c_str(), ent->d_name) == 0)
                        {
                            auto ret = lookIn + "/" + ent->d_name;
                            closedir(dir);
                            return ret;
                        }
                    }
                    else
                    {
                        if (filename == ent->d_name)
                        {
                            auto ret = lookIn + "/" + ent->d_name;
                            closedir(dir);
                            return ret;
                        }
                    }

                    if (ent->d_type & DT_DIR && deepSearch)
                    {
                        auto ret = findFile(filename, lookIn + "/" + ent->d_name, deepSearch, ignoreCase);
                        if (!ret.empty())
                        {
                            closedir(dir);
                            return ret;
                        }
                    }
                }
                closedir(dir);
            }
            return "";
        }
        
        std::vector<std::string> findAllFiles(const std::string& lookIn, const std::string& extension, bool deepSearch)
        {
            std::vector<std::string> ret;

            bool all = extension == "*";
            auto upExt = toUpper(extension);
            DIR* dir;
            struct dirent* ent;
            if ((dir = opendir(lookIn.c_str())) != NULL)
            {
                while ((ent = readdir(dir)) != NULL)
                {
                    if (!strcmp(ent->d_name, "."))
                    {
                        continue;
                    }
                    else if (!strcmp(ent->d_name, ".."))
                    {
                        continue;
                    }

                    if (ent->d_type & DT_DIR)
                    {
                        if (deepSearch)
                        {
                            auto ret2 = findAllFiles(lookIn + "/" + ent->d_name, extension, deepSearch);
                            ret.insert(ret.end(), ret2.begin(), ret2.end());
                        }
                    }
                    else
                    {
                        if (all)
                        {
                            ret.push_back(lookIn + "/" + ent->d_name);
                        }
                        else if (toUpper(getExtension(ent->d_name)) == upExt)
                        {
                            ret.push_back(lookIn + "/" + ent->d_name);
                        }
                    }
                }
                closedir(dir);
            }

            return std::move(ret);
        }

        std::string concatenateChars(const char* fmt, ...)
        {
            char buffer[MAX_CONCATENATE_CHARS_LENGTH];
            va_list arg_ptr;

            va_start(arg_ptr, fmt);
            vsprintf(buffer, fmt, arg_ptr);
            va_end(arg_ptr);

            return std::string(buffer);
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

        bool loadJson(Json::Value& out, const std::string& filename)
        {
            std::ifstream file(filename);
            if (!file.is_open())
            {
                CORE_ERROR("Failed to load file: {}", filename);
                return false;
            }
            try
            {
                file >> out;
            }
            catch (...)
            {
                CORE_ERROR("Failed to parse file: {}", filename);
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

        Json::Value serializeJsonValue(int8_t val) { return serializeInt8(val); }
        Json::Value serializeJsonValue(uint8_t val) { return serializeUInt8(val); }
        Json::Value serializeJsonValue(int16_t val) { return serializeInt16(val); }
        Json::Value serializeJsonValue(uint16_t val) { return serializeUInt16(val); }
        Json::Value serializeJsonValue(int32_t val) { return serializeInt32(val); }
        Json::Value serializeJsonValue(uint32_t val) { return serializeUInt32(val); }
        Json::Value serializeJsonValue(int64_t val) { return serializeInt64(val); }
        Json::Value serializeJsonValue(uint64_t val) { return serializeUInt64(val); }
        Json::Value serializeJsonValue(float val) { return serializeFloat(val); }
        Json::Value serializeJsonValue(double val) { return deserializeDouble(val); }
        Json::Value serializeJsonValue(bool val) { return deserializeBool(val); }
        Json::Value serializeJsonValue(const std::string &val) { return serializeString(val); }
        Json::Value serializeJsonValue(const glm::vec2& val) { return serializeFloat2(&val.x); }
        Json::Value serializeJsonValue(const glm::vec3& val) { return serializeFloat3(&val.x); }
        Json::Value serializeJsonValue(const glm::vec4& val) { return serializeFloat4(&val.x); }
        Json::Value serializeJsonValue(const glm::ivec2& val) { return serializeInt2(&val.x); }
        Json::Value serializeJsonValue(const glm::ivec4& val) { return serializeInt4(&val.x); }
        Json::Value serializeJsonValue(const glm::mat4& val) { return serializeMatrix(&val[0][0]); }
        Json::Value serializeJsonValue(const std::vector<std::string> &val) { return serializeStringArray(val); }

        template<> int8_t deserializeJsonValue<int8_t>(const Json::Value &json) { return deserializeInt8(json); }
        template<> uint8_t deserializeJsonValue<uint8_t>(const Json::Value &json) { return deserializeUInt8(json); }
        template<> int16_t deserializeJsonValue<int16_t>(const Json::Value &json) { return deserializeInt16(json); }
        template<> uint16_t deserializeJsonValue<uint16_t>(const Json::Value &json) { return deserializeUInt16(json); }
        template<> int32_t deserializeJsonValue<int32_t>(const Json::Value &json) { return deserializeInt32(json); }
        template<> uint32_t deserializeJsonValue<uint32_t>(const Json::Value &json) { return deserializeUInt32(json); }
        template<> int64_t deserializeJsonValue<int64_t>(const Json::Value &json) { return deserializeInt64(json); }
        template<> uint64_t deserializeJsonValue<uint64_t>(const Json::Value &json) { return deserializeUInt64(json); }
        template<> float deserializeJsonValue<float>(const Json::Value &json) { return deserializeFloat(json); }
        template<> double deserializeJsonValue<double>(const Json::Value &json) { return deserializeDouble(json); }
        template<> bool deserializeJsonValue<bool>(const Json::Value &json) { return deserializeBool(json); }
        template<> std::string deserializeJsonValue<std::string>(const Json::Value &json) { return deserializeString(json); }
        template<> glm::vec2 deserializeJsonValue<glm::vec2>(const Json::Value &json) { glm::vec2 ret; deserializeFloat2(&ret.x, json); return ret; }
        template<> glm::vec3 deserializeJsonValue<glm::vec3>(const Json::Value &json) { glm::vec3 ret; deserializeFloat3(&ret.x, json); return ret; }
        template<> glm::vec4 deserializeJsonValue<glm::vec4>(const Json::Value &json) { glm::vec4 ret; deserializeFloat4(&ret.x, json); return ret; }
        template<> glm::ivec2 deserializeJsonValue<glm::ivec2>(const Json::Value &json) { glm::ivec2 ret; deserializeInt2(&ret.x, json); return ret; }
        template<> glm::ivec4 deserializeJsonValue<glm::ivec4>(const Json::Value &json) { glm::ivec4 ret; deserializeInt4(&ret.x, json); return ret; }
        template<> glm::mat4 deserializeJsonValue<glm::mat4>(const Json::Value &json) { glm::mat4 ret; deserializeMatrix(&ret[0][0], json); return ret; }
        template<> std::vector<std::string> deserializeJsonValue<std::vector<std::string>>(const Json::Value &json) { return deserializeStringArray(json); }

        
        // Some magic found here: http://paulbourke.net/miscellaneous/interpolation/
        //
        // Tension: 1 is high, 0 normal, -1 is low
        // Bias: 0 is even,
        // positive is towards first segment,
        // negative towards the other
        float resamplingHermite(float y0, float y1,
                                float y2, float y3,
                                float mu, float tension, float bias)
        {
            float m0, m1, mu2, mu3;
            float a0, a1, a2, a3;

            mu2 = mu * mu;
            mu3 = mu2 * mu;
            m0 = (y1 - y0)*(1 + bias)*(1 - tension) / 2;
            m0 += (y2 - y1)*(1 - bias)*(1 - tension) / 2;
            m1 = (y2 - y1)*(1 + bias)*(1 - tension) / 2;
            m1 += (y3 - y2)*(1 - bias)*(1 - tension) / 2;
            a0 = 2 * mu3 - 3 * mu2 + 1;
            a1 = mu3 - 2 * mu2 + mu;
            a2 = mu3 - mu2;
            a3 = -2 * mu3 + 3 * mu2;

            return (a0*y1 + a1*m0 + a2*m1 + a3*y2);
        }

        bool lerp(bool from, bool to, float t)
        {
            return (t < .5f) ? from : to;
        }

        int lerp(int from, int to, float t)
        {
            auto result = static_cast<float>(from)+(static_cast<float>(to)-static_cast<float>(from)) * t;
            return static_cast<int>(std::round(result));
        }

        unsigned int lerp(unsigned int from, unsigned int to, float t)
        {
            auto result = static_cast<float>(from)+(static_cast<float>(to)-static_cast<float>(from)) * t;
            result = std::round(result);
            if (result < 0.f) result = 0.f;
            return static_cast<unsigned int>(result);
        }

        float lerp(float from, float to, float t)
        {
            return from + (to - from) * t;
        }

        double lerp(double from, double to, float t)
        {
            return from + (to - from) * static_cast<double>(t);
        }

        std::string lerp(const std::string& from, const std::string& to, float t)
        {
            auto fromLen = from.size();
            auto toLen = to.size();
            auto newLen = static_cast<float>(fromLen)+(static_cast<float>(toLen)-static_cast<float>(fromLen)) * t;
            newLen = round(newLen);
            if (toLen > fromLen)
            {
                auto ret = to.substr(0, static_cast<size_t>(newLen));
                return std::move(ret);
            }
            else
            {
                auto ret = from.substr(0, static_cast<size_t>(newLen));
                return std::move(ret);
            }
        }

        void setClipboard(const std::string& text)
        {
            SDL_SetClipboardText(text.c_str());
        }

        std::string getClipboard()
        {
            auto pClipboard = SDL_GetClipboardText();
            std::string ret = pClipboard;
            SDL_free(pClipboard);
            return ret;
        }

        bool createFolder(const std::string& fullPath)
        {
#if defined(WIN32)
            std::string withBackwardSlashes = fullPath;
            replace(withBackwardSlashes, "/", "\\");
            return system(("mkdir " + withBackwardSlashes).c_str()) == 0;
#else
            return system(("mkdir " + fullPath).c_str()) == 0;
#endif
        }

        bool copyFile(const std::string& from, const std::string& to)
        {
#if defined(WIN32)
            std::string withBackwardSlashes_from = from;
            replace(withBackwardSlashes_from, "/", "\\");
            std::string withBackwardSlashes_to = to;
            replace(withBackwardSlashes_to, "/", "\\");
            std::ifstream src(withBackwardSlashes_from, std::ios::binary);
            std::ofstream dst(withBackwardSlashes_to, std::ios::binary);
            if (!src.is_open() || !dst.is_open()) return false;
            dst << src.rdbuf();
            src.close();
            dst.close();
            return true;
#else
            return system(("cp " + from + " " + to).c_str()) == 0;
#endif
        }

        bool createTextFile(const std::string& path, const std::string& content)
        {
#if defined(WIN32)
            std::string withBackwardSlashes = path;
            replace(withBackwardSlashes, "/", "\\");
            std::ofstream dst(withBackwardSlashes, std::ios::binary);
            if (!dst.is_open()) return false;
            dst << content;
            dst.close();
            return true;
#else
            std::ofstream dst(path, std::ios::binary);
            if (!dst.is_open()) return false;
            dst << content;
            dst.close();
            return true;
#endif
        }

        void showInExplorer(const std::string& path)
        {
#if defined(WIN32)
            std::string pathReverseSlash = path;
            std::replace(pathReverseSlash.begin(), pathReverseSlash.end(), '/', '\\');
            ITEMIDLIST *pidl = ILCreateFromPath(pathReverseSlash.c_str());
            if (pidl)
            {
                SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
                ILFree(pidl);
            }
#elif defined(__APPLE__)
            system(("open " + path).c_str());
#else
            system(("xdg-open " + path).c_str());
#endif
        }

        void openFile(const std::string& file)
        {
#if defined(WIN32)
            ShellExecuteA(0, 0, file.c_str(), 0, 0 , SW_SHOW);
#endif
        }

        bool fileExists(const std::string& file)
        {
            std::filesystem::path path(file);

            if (path.empty()) {
                return false;
            }

            return std::filesystem::exists(path) && (std::filesystem::is_regular_file(path) || std::filesystem::is_symlink(path));
        }
    }
}
