#include "Engine/ComponentFactory.h"
#include "Engine/Log.h"

#include "Engine/SpriteComponent.h"
#include "Engine/Slice9Component.h"
#include "Engine/TextComponent.h"
#include "Engine/ScriptComponent.h"
#include "Engine/PFXComponent.h"
#include "Engine/FrameAnimComponent.h"

namespace Engine
{
    void ComponentFactory::initialize()
    {
        registerComponent<SpriteComponent>();
        registerComponent<Slice9Component>();
        registerComponent<TextComponent>();
        registerComponent<ScriptComponent>();
        registerComponent<PFXComponent>();
        registerComponent<FrameAnimComponent>();
    }

    std::map<std::string, CreateComponentFn> ComponentFactory::s_factories;
    std::vector<std::string> ComponentFactory::s_componentNames;

    bool ComponentFactory::registerFactory(const std::string& name, CreateComponentFn fn)
    {
        if (s_factories.find(name) != s_factories.end())
        {
            CORE_FATAL("Duplicated component name: {}", name);
            return false;
        }
        s_factories[name] = fn;
        s_componentNames.push_back(name);

        std::sort(s_componentNames.begin(), s_componentNames.end());

        return true;
    }

    ComponentRef ComponentFactory::create(const std::string& name)
    {
        auto it = s_factories.find(name);
        if (it == s_factories.end())
        {
            //CORE_FATAL("Unknown Component: {}", name); // Don't alert, might be done on purpose
            return nullptr;
        }

        return it->second();
    }

    const std::vector<std::string>& ComponentFactory::getComponentNames()
    {
        return s_componentNames;
    }
}
