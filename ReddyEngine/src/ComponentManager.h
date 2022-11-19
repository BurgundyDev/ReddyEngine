#pragma once

#include <memory>
#include <vector>


namespace Engine
{
    class Component;
    using ComponentRef = std::shared_ptr<Component>;


    class ComponentManager
    {
    public:
        void clear();

        void addComponent(const ComponentRef& pComponent);
        void removeComponent(const ComponentRef& pComponent);

        void update(float dt);
        void fixedUpdate(float dt);

    private:
        enum class CommandType
        {
            Create,
            Destroy
        };

        struct Command
        {
            CommandType type;
            ComponentRef pComponent;
        };

        void processCommands();

        std::vector<Command> m_commands;
        std::vector<Command> m_commandsCopy;
        std::vector<ComponentRef> m_components; // Collected cache
    };
}
