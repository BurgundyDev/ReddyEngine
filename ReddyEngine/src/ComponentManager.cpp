#include "ComponentManager.h"
#include "Engine/Component.h"
#include "Engine/Scene.h"
#include "Engine/ReddyEngine.h"


namespace Engine
{
    void ComponentManager::clear()
    {
        m_components.clear();
        m_commands.clear();
    }

    void ComponentManager::addComponent(const ComponentRef& pComponent)
    {
        m_commands.push_back({CommandType::Create, pComponent});
    }

    void ComponentManager::removeComponent(const ComponentRef& pComponent)
    {
        m_commands.push_back({CommandType::Destroy, pComponent});
    }

    void ComponentManager::processCommands()
    {
        while (!m_commands.empty()) // Reason is, component functions like OnCreate/onDestroy can create/destroy more entities!
        {
            m_commandsCopy = m_commands;
            m_commands.clear();

            for (const auto& command : m_commandsCopy)
            {
                switch (command.type)
                {
                    case CommandType::Create:
                        m_components.push_back(command.pComponent);
                        command.pComponent->onCreate();
                        if (command.pComponent->isEnabled())
                            command.pComponent->onEnable();
                        break;

                    case CommandType::Destroy:
                        if (command.pComponent->isEnabled())
                            command.pComponent->onDisable();
                        command.pComponent->onDestroy();
                        for (auto it = m_components.begin(); it != m_components.end(); ++it)
                        {
                            if (*it == command.pComponent)
                            {
                                it = m_components.erase(it);
                                break;
                            }
                        }
                }
            }
        }

        m_commandsCopy.clear();
    }

    void ComponentManager::update(float dt)
    {
        if (getScene()->isEditorScene()) // Editor doesn't update entities or fire their events
        {
            m_commandsCopy.clear();
            return;
        }

        processCommands();

        for (const auto& pComponent : m_components)
            pComponent->update(dt);

        processCommands();
    }

    void ComponentManager::fixedUpdate(float dt)
    {
        if (getScene()->isEditorScene())  // Editor doesn't update entities or fire their events
        {
            m_commandsCopy.clear();
            return;
        }

        processCommands();

        for (const auto& pComponent : m_components)
            pComponent->update(dt);

        processCommands();
    }
}
