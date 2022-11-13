extern "C" {
	#include <../lua/lua.h>
	#include <../lua/lauxlib.h>
	#include <../lua/lualib.h>
}

#include "Engine/ScriptComponent.h"
#include "Engine/LuaBindings.h"
#include "Engine/ReddyEngine.h"
#include "Engine/GUI.h"
#include "Engine/Utils.h"
#include "Engine/Scene.h"

#include <imgui.h>


#define LUA_FLAG_CREATE         0b1
#define LUA_FLAG_DESTROY        0b10
#define LUA_FLAG_ENABLE         0b100
#define LUA_FLAG_DISABLE        0b1000
#define LUA_FLAG_FIXEDUPDATE    0b10000
#define LUA_FLAG_UPDATE         0b100000
#define LUA_FLAG_MOUSEENTER     0b1000000
#define LUA_FLAG_MOUSELEAVE     0b10000000
#define LUA_FLAG_MOUSEDOWN      0b100000000
#define LUA_FLAG_MOUSEUP        0b1000000000
#define LUA_FLAG_MOUSECLICK     0b10000000000


static uint64_t g_nextRuntimeId = 1;


namespace Engine
{
    ScriptComponent::ScriptComponent()
    {
        runtimeId = g_nextRuntimeId++;
        luaName = "CINS_" + std::to_string(runtimeId);
    }

    ScriptComponent::~ScriptComponent()
    {
        destroyLuaObj();
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
                case LuaPropertyType::Bool:
                    propertiesJson[luaProperty.name] = Utils::serializeBool(luaProperty.boolValue);
                    break;
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

            auto isEditor = getScene()->isEditorScene();
            if (!isEditor) createLuaObj();
            auto L = getLuaBindings()->getState();

            const auto& propertiesJson = json["properties"];
            if (!m_luaProperties.empty())
            {
                if (!isEditor)
                {
                    lua_getglobal(L, "CINS_t");
                    lua_getfield(L, -1, luaName.c_str());
                }
                for (auto& luaProperty : m_luaProperties)
                {
                    switch (luaProperty.type)
                    {
                        case LuaPropertyType::Bool:
                            luaProperty.boolValue = Utils::deserializeBool(propertiesJson[luaProperty.name], luaProperty.boolValue);
                            if (!isEditor)
                            {
                                lua_pushboolean(L, luaProperty.boolValue ? 1 : 0);
                                lua_setfield(L, -2, luaProperty.name.c_str());
                            }
                            break;
                        case LuaPropertyType::Int:
                            luaProperty.intValue = Utils::deserializeInt32(propertiesJson[luaProperty.name], luaProperty.intValue);
                            if (!isEditor)
                            {
                                lua_pushinteger(L, luaProperty.intValue);
                                lua_setfield(L, -2, luaProperty.name.c_str());
                            }
                            break;
                        case LuaPropertyType::Float:
                            luaProperty.floatValue = Utils::deserializeFloat(propertiesJson[luaProperty.name], luaProperty.floatValue);
                            if (!isEditor)
                            {
                                lua_pushnumber(L, (lua_Number)luaProperty.floatValue);
                                lua_setfield(L, -2, luaProperty.name.c_str());
                            }
                            break;
                        case LuaPropertyType::Vec2:
                            Utils::deserializeFloat2(&luaProperty.vec2Value.x, propertiesJson[luaProperty.name], &luaProperty.vec2Value.x);
                            if (!isEditor)
                            {
                                LUA_PUSH_VEC2(luaProperty.vec2Value);
                                lua_setfield(L, -2, luaProperty.name.c_str());
                            }
                            break;
                        case LuaPropertyType::Color:
                            Utils::deserializeFloat4(&luaProperty.colorValue.x, propertiesJson[luaProperty.name], &luaProperty.colorValue.x);
                            if (!isEditor)
                            {
                                LUA_PUSH_COLOR(luaProperty.colorValue);
                                lua_setfield(L, -2, luaProperty.name.c_str());
                            }
                            break;
                        case LuaPropertyType::String:
                            luaProperty.stringValue = Utils::deserializeString(propertiesJson[luaProperty.name], luaProperty.stringValue);
                            if (!isEditor)
                            {
                                lua_pushstring(L, luaProperty.stringValue.c_str());
                                lua_setfield(L, -2, luaProperty.name.c_str());
                            }
                            break;
                    }
                }
                if (!isEditor) lua_pop(L, lua_gettop(L));
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
            if (m_pLuaComponentDef)
                m_luaProperties = m_pLuaComponentDef->properties;
        }

        if (!m_luaProperties.empty())
        {
            ImGui::Dummy(ImVec2(0, 10)); // So we draw properties full span under icon
            ImGui::Columns();
            GUI::beginGroup("Properties");
            for (auto& luaProperty : m_luaProperties)
            {
                switch (luaProperty.type)
                {
                    case LuaPropertyType::Bool:
                        changed |= GUI::boolProperty(luaProperty.name.c_str(), &luaProperty.boolValue, luaProperty.tooltip.c_str());
                        break;
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
            ImGui::Columns(2, nullptr, false);
        }

        return changed;
    }

    void ScriptComponent::createLuaObj()
    {
        if (!m_pLuaComponentDef) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getglobal(L, m_pLuaComponentDef->luaName.c_str());

        lua_getfield(L, -1, "create"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_CREATE; lua_pop(L, 1);
        lua_getfield(L, -1, "destroy"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_DESTROY; lua_pop(L, 1);
        lua_getfield(L, -1, "enable"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_ENABLE; lua_pop(L, 1);
        lua_getfield(L, -1, "disable"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_DISABLE; lua_pop(L, 1);
        lua_getfield(L, -1, "fixedUpdate"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_FIXEDUPDATE; lua_pop(L, 1);
        lua_getfield(L, -1, "update"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_UPDATE; lua_pop(L, 1);
        lua_getfield(L, -1, "mouseEnter"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_MOUSEENTER; lua_pop(L, 1);
        lua_getfield(L, -1, "mouseLeave"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_MOUSELEAVE; lua_pop(L, 1);
        lua_getfield(L, -1, "mouseDown"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_MOUSEDOWN; lua_pop(L, 1);
        lua_getfield(L, -1, "mouseUp"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_MOUSEUP; lua_pop(L, 1);
        lua_getfield(L, -1, "mouseClick"); if (lua_isfunction(L, -1)) m_implLuaCallsMask |= LUA_FLAG_MOUSECLICK; lua_pop(L, 1);

        LUA_CLONE_TABLE(L, lua_gettop(L));

        // Add pointer to our script component
        lua_pushlightuserdata(L, this);
        lua_setfield(L, -2, "COBJ");

        lua_setfield(L, -3, luaName.c_str());
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::destroyLuaObj()
    {
        if (!m_pLuaComponentDef) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_pushnil(L);
        lua_setfield(L, -2, luaName.c_str());
        lua_pop(L, lua_gettop(L));
    }
    
	void ScriptComponent::onCreate()
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_CREATE)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "create");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            checkLua(L, lua_pcall(L, 1, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::onDestroy()
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_DESTROY)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "destroy");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            checkLua(L, lua_pcall(L, 1, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::onEnable()
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_ENABLE)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "enable");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            checkLua(L, lua_pcall(L, 1, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::onDisable()
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_DISABLE)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "disable");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            checkLua(L, lua_pcall(L, 1, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

    void ScriptComponent::fixedUpdate(float dt)
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_FIXEDUPDATE)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "fixedUpdate");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            lua_pushnumber(L, (lua_Number)dt);
            checkLua(L, lua_pcall(L, 2, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::update(float dt)
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_UPDATE)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "update");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            lua_pushnumber(L, (lua_Number)dt);
            checkLua(L, lua_pcall(L, 2, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::onMouseEnter()
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_MOUSEENTER)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "mouseEnter");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            checkLua(L, lua_pcall(L, 1, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::onMouseLeave()
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_MOUSELEAVE)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "mouseLeave");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            checkLua(L, lua_pcall(L, 1, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::onMouseDown()
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_MOUSEDOWN)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "mouseDown");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            checkLua(L, lua_pcall(L, 1, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::onMouseUp()
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_MOUSEUP)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "mouseUp");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            checkLua(L, lua_pcall(L, 1, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }

	void ScriptComponent::onMouseClick()
    {
        if (!(m_implLuaCallsMask & LUA_FLAG_MOUSECLICK)) return;

        auto L = getLuaBindings()->getState();

        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, luaName.c_str());
        lua_getfield(L, -1, "mouseClick");

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            checkLua(L, lua_pcall(L, 1, 0, 0));
        }
        lua_pop(L, lua_gettop(L));
    }
}
