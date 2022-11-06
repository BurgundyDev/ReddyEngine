#pragma once

namespace Engine
{
    class Resource;
    using ResourceRef = std::shared_ptr<Resource>;

    class Resource
    {
    public:
        virtual ~Resource() {};
        const std::string& getFilename() const { return m_filename; };
        void setFilename(const std::string& filename) { m_filename = filename; }

    protected:
        std::string m_filename;
    };
}