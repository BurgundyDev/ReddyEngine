#pragma once

namespace Engine
{
    class Resource;
    using ResourceRef = std::shared_ptr<Resource>;

    class Resource
    {
    public:
        virtual ~Resource() {}
    };
}