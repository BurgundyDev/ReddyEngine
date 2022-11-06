#pragma once

namespace Engine
{
    class Resource;
    using ResourceRef = std::shared_ptr<Resource>;

    class Resource
    {
    public:
        virtual ~Resource() {};
        const std::string& getFilename() { return m_filename; };
    protected:
        std::string m_filename;
    };
}