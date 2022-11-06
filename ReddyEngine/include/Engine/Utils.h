#pragma once

#include <json/json.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <string>
#include <vector>
#include <stdarg.h>

namespace Engine
{
    namespace Utils
    {
        // Strings stuff
#if defined(WIN32)
        std::wstring utf8ToWide(const std::string& utf8);
        std::string wideToUtf8(const std::wstring& wide);
#endif
        std::vector<std::string> splitString(const std::string& in_string, char in_delimiter, bool in_removeEmptyElements = true);
        std::vector<std::string> splitString(const std::string& in_string, const std::string& in_delimiters);
        std::string removeChars(const std::string& str, const std::string& charsToRemove);
        std::string toUpper(const std::string& str);
        std::string toLower(const std::string& str);
        std::string trim(const std::string& str);
        size_t utf8Length(const std::string& str);
        size_t utf8Pos(const std::string& str, size_t pos);
        std::string concatenateChars(const char* fmt, ...);

        // Path stuff
        std::string getPath(const std::string& filename);
        std::string getPathWithoutExtension(const std::string& filename);
        std::string getFilename(const std::string& path);
        std::string getFilenameWithoutExtension(const std::string& path);
        std::string getExtension(const std::string& filename);
        std::string getParentFolderName(const std::string& filename);
        std::string getSavePath(const std::string& appName); // On Windows, this returns the Roaming App Data path. On other platforms, it returns local directory "./"
        std::string makeRelativePath(const std::string& path, const std::string& relativeTo);
        std::string findFile(const std::string& filename, const std::string& lookIn, bool deepSearch, bool ignoreCase);
        
        bool createFolder(const std::string& fullPath);
        bool copyFile(const std::string& from, const std::string& to);
        bool createTextFile(const std::string& path, const std::string& content);
        void showInExplorer(const std::string& path);
        void openFile(const std::string& file);

        // Config
        bool loadJson(Json::Value &out, const std::string& filename);
        bool saveJson(const Json::Value &json, const std::string& filename);

        // Json serialize/deserialize
        Json::Value serializeInt8(int8_t val);
        Json::Value serializeUInt8(uint8_t val);
        Json::Value serializeInt16(int16_t val);
        Json::Value serializeUInt16(uint16_t val);
        Json::Value serializeInt32(int32_t val);
        Json::Value serializeUInt32(uint32_t val);
        Json::Value serializeInt64(int64_t val);
        Json::Value serializeUInt64(uint64_t val);
        Json::Value serializeFloat(float val);
        Json::Value serializeDouble(double val);
        Json::Value serializeBool(bool val);
        Json::Value serializeString(const std::string &val);
        Json::Value serializeFloat2(const float *val);
        Json::Value serializeFloat3(const float *val);
        Json::Value serializeFloat4(const float *val);
        Json::Value serializeInt2(const int *val);
        Json::Value serializeInt4(const int *val);
        Json::Value serializeMatrix(const float *val);
        Json::Value serializeStringArray(const std::vector<std::string> &val);

        int8_t deserializeInt8(const Json::Value &json, int8_t in_default = 0);
        uint8_t deserializeUInt8(const Json::Value &json, uint8_t in_default = 0);
        int16_t deserializeInt16(const Json::Value &json, int16_t in_default = 0);
        uint16_t deserializeUInt16(const Json::Value &json, uint16_t in_default = 0);
        int32_t deserializeInt32(const Json::Value &json, int32_t in_default = 0);
        uint32_t deserializeUInt32(const Json::Value &json, uint32_t in_default = 0);
        int64_t deserializeInt64(const Json::Value &json, int64_t in_default = 0);
        uint64_t deserializeUInt64(const Json::Value &json, uint64_t in_default = 0);
        float deserializeFloat(const Json::Value &json, float in_default = 0.0f);
        double deserializeDouble(const Json::Value &json, double in_default = 0.0);
        bool deserializeBool(const Json::Value &json, bool in_default = false);
        std::string deserializeString(const Json::Value &json, const std::string &in_default = "");
        void deserializeFloat2(float* out, const Json::Value &json, const float *in_default = nullptr);
        void deserializeFloat3(float* out, const Json::Value &json, const float *in_default = nullptr);
        void deserializeFloat4(float* out, const Json::Value &json, const float *in_default = nullptr);
        void deserializeInt2(int* out, const Json::Value &json, const int *in_default = nullptr);
        void deserializeInt4(int* out, const Json::Value &json, const int *in_default = nullptr);
        void deserializeMatrix(float* out, const Json::Value &json, const float *in_default = nullptr); // Null = identity
        std::vector<std::string> deserializeStringArray(const Json::Value &json, const std::vector<std::string> &in_default = {});
        
        Json::Value serializeJsonValue(int8_t val);
        Json::Value serializeJsonValue(uint8_t val);
        Json::Value serializeJsonValue(int16_t val);
        Json::Value serializeJsonValue(uint16_t val);
        Json::Value serializeJsonValue(int32_t val);
        Json::Value serializeJsonValue(uint32_t val);
        Json::Value serializeJsonValue(int64_t val);
        Json::Value serializeJsonValue(uint64_t val);
        Json::Value serializeJsonValue(float val);
        Json::Value serializeJsonValue(double val);
        Json::Value serializeJsonValue(bool val);
        Json::Value serializeJsonValue(const std::string &val);
        Json::Value serializeJsonValue(const glm::vec2& val);
        Json::Value serializeJsonValue(const glm::vec3& val);
        Json::Value serializeJsonValue(const glm::vec4& val);
        Json::Value serializeJsonValue(const glm::ivec2& val);
        Json::Value serializeJsonValue(const glm::ivec4& val);
        Json::Value serializeJsonValue(const glm::mat4& val);
        Json::Value serializeJsonValue(const std::vector<std::string> &val);

        template<typename T> T deserializeJsonValue(const Json::Value &json);


        // Curves
        float resamplingHermite(float y0, float y1,
                                float y2, float y3,
                                float mu, float tension = 0.0f, float bias = 0.0f);

        // Clipboard
        void setClipboard(const std::string& text);
        std::string getClipboard();
    }
}
