#include "Engine/ScriptComponent.h"
#include "Engine/LuaBindings.h"
#include "Engine/ReddyEngine.h"
#include "Engine/GUI.h"
#include "Engine/Utils.h"
#include "Engine/Scene.h"


static uint64_t g_nextRuntimeId = 1;


namespace Engine
{
    ScriptComponent::ScriptComponent()
    {
        runtimeId = g_nextRuntimeId++;
    }

    ScriptComponent::~ScriptComponent()
    {
    }

    Json::Value ScriptComponent::serialize()
    {
        auto json = Component::serialize();

        json["name"] = name;

        Json::Value propertiesJson;
        for (auto& luaProperty : m_luaProperties)
        {
            switch (luaProperty.type)
            {
                case LuaPropertyType::Int:
                    propertiesJson[luaProperty.name] = Utils::serializeInt32(luaProperty.intValue);
                    break;
                case LuaPropertyType::Float:
                    propertiesJson[luaProperty.name] = Utils::serializeFloat(luaProperty.floatValue);
                    break;
                case LuaPropertyType::Vec2:
                    propertiesJson[luaProperty.name] = Utils::serializeFloat2(&luaProperty.vec2Value.x);
                    break;
                case LuaPropertyType::Color:
                    propertiesJson[luaProperty.name] = Utils::serializeFloat4(&luaProperty.colorValue.x);
                    break;
                case LuaPropertyType::String:
                    propertiesJson[luaProperty.name] = Utils::serializeString(luaProperty.stringValue);
                    break;
            }
        }

        json["properties"] = propertiesJson;

        return json;
    }

    void ScriptComponent::deserialize(const Json::Value& json)
    {
        Component::deserialize(json);

        name = json["name"].asString();

        m_pLuaComponentDef = getLuaBindings()->getComponentDef(name);
        if (!m_pLuaComponentDef)
        {
            m_luaProperties.clear();
        }
        else
        {
            m_luaProperties = m_pLuaComponentDef->properties; // Copy default properties

            const auto& propertiesJson = json["properties"];
            for (auto& luaProperty : m_luaProperties)
            {
                switch (luaProperty.type)
                {
                    case LuaPropertyType::Int:
                        luaProperty.intValue = Utils::deserializeInt32(propertiesJson[luaProperty.name], luaProperty.intValue);
                        break;
                    case LuaPropertyType::Float:
                        luaProperty.floatValue = Utils::deserializeFloat(propertiesJson[luaProperty.name], luaProperty.floatValue);
                        break;
                    case LuaPropertyType::Vec2:
                        Utils::deserializeFloat2(&luaProperty.vec2Value.x, propertiesJson[luaProperty.name], &luaProperty.vec2Value.x);
                        break;
                    case LuaPropertyType::Color:
                        Utils::deserializeFloat4(&luaProperty.colorValue.x, propertiesJson[luaProperty.name], &luaProperty.colorValue.x);
                        break;
                    case LuaPropertyType::String:
                        luaProperty.stringValue = Utils::deserializeString(propertiesJson[luaProperty.name], luaProperty.stringValue);
                        break;
                }
            }
        }
    }

    bool ScriptComponent::edit()
    {
        bool changed = false;

        if (GUI::stringProperty("Component Name", &name, "Changing this will reset all properties values. Lua scripts should call RegisterComponent(\"name\")."))
        {
            changed = true;
            m_pLuaComponentDef = getLuaBindings()->getComponentDef(name);
            m_luaProperties = m_pLuaComponentDef->properties;
        }

        GUI::beginGroup("Properties");
        for (auto& luaProperty : m_luaProperties)
        {
            switch (luaProperty.type)
            {
                case LuaPropertyType::Int:
                    changed |= GUI::intProperty(luaProperty.name.c_str(), &luaProperty.intValue, -1000000, 1000000, luaProperty.tooltip.c_str());
                    break;
                case LuaPropertyType::Float:
                    changed |= GUI::floatProperty(luaProperty.name.c_str(), &luaProperty.floatValue, -1000000.0f, 1000000.0f, luaProperty.tooltip.c_str());
                    break;
                case LuaPropertyType::Vec2:
                    changed |= GUI::vec2Property(luaProperty.name.c_str(), &luaProperty.vec2Value, luaProperty.tooltip.c_str());
                    break;
                case LuaPropertyType::Color:
                    changed |= GUI::colorProperty(luaProperty.name.c_str(), &luaProperty.colorValue, luaProperty.tooltip.c_str());
                    break;
                case LuaPropertyType::String:
                    changed |= GUI::stringProperty(luaProperty.name.c_str(), &luaProperty.stringValue, luaProperty.tooltip.c_str());
                    break;
            }
        }
        GUI::endGroup();

        return changed;
    }
    
	void ScriptComponent::onCreate()
    {
    }

	void ScriptComponent::onDestroy()
    {
    }

	void ScriptComponent::onEnable()
    {
    }

	void ScriptComponent::onDisable()
    {
    }

	void ScriptComponent::onMouseEnter()
    {
    }

	void ScriptComponent::onMouseLeave()
    {
    }

	void ScriptComponent::onMouseDown()
    {
    }

	void ScriptComponent::onMouseUp()
    {
    }

	void ScriptComponent::onMouseClick()
    {
    }
}
