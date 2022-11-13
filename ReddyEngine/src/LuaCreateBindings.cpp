extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "Engine/LuaBindings.h"
#include "Engine/Log.h"
#include "Engine/ReddyEngine.h"
#include "Engine/EventSystem.h"
#include "Engine/ScriptComponent.h"
#include "Engine/Entity.h"


namespace Engine
{
    void LuaBindings::createBindings()
    {
#define LUA_REGISTER(name) lua_register(L, #name, [](lua_State* L){return Engine::getLuaBindings()->func ## name(L);})

        LUA_REGISTER(RegisterComponent);
        LUA_REGISTER(SetIntProperty);
        LUA_REGISTER(SetFloatProperty);
        LUA_REGISTER(SetVec2Property);
        LUA_REGISTER(SetColorProperty);
        LUA_REGISTER(SetStringProperty);

        LUA_REGISTER(SendEvent);

        LUA_REGISTER(GetPosition);
        LUA_REGISTER(SetPosition);
        LUA_REGISTER(GetWorldPosition);
        LUA_REGISTER(SetWorldPosition);
        LUA_REGISTER(GetRotation);
        LUA_REGISTER(SetRotation);
        LUA_REGISTER(GetScale);
        LUA_REGISTER(SetScale);
    }

    int LuaBindings::funcRegisterComponent(lua_State* L)
    {
        if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_istable(L, 2))
        {
            CORE_ERROR_POPUP("Lua: RegisterComponent expected (string, table)");
            return 0;
        }

        std::string name = lua_tostring(L, 1);

        auto pComponentDef = new LuaComponentDef();

        // Keep global reference on this object so it doesn't get garbage collected
        pComponentDef->luaName = "CDEF_" + name;
        lua_pushvalue(L, 2);
        lua_setglobal(L, pComponentDef->luaName.c_str());

        m_componentDefs[name] = pComponentDef;

        return 0;
    }
    
    int LuaBindings::funcSetIntProperty(lua_State* L)
    {
        if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2))
        {
            CORE_ERROR_POPUP("Lua: SetIntProperty expected (string, string)");
            return 0;
        }
        if (!m_pCurrentComponentDef)
        {
            CORE_ERROR_POPUP("Lua: SetIntProperty can only be called in initComponent()");
            return 0;
        }

        LuaProperty p;
        p.type = LuaPropertyType::Int;
        p.name = lua_tostring(L, 1);
        p.tooltip = lua_tostring(L, 2);

        lua_getglobal(L, m_pCurrentComponentDef->luaName.c_str());
        lua_getfield(L, -1, p.name.c_str());
        p.intValue = (int)lua_tointeger(L, -1);
        lua_pop(L, 2);

        m_pCurrentComponentDef->properties.push_back(p);
        return 0;
    }

    int LuaBindings::funcSetFloatProperty(lua_State* L)
    {
        if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2))
        {
            CORE_ERROR_POPUP("Lua: SetFloatProperty expected (string, string)");
            return 0;
        }
        if (!m_pCurrentComponentDef)
        {
            CORE_ERROR_POPUP("Lua: SetFloatProperty can only be called in initComponent()");
            return 0;
        }

        LuaProperty p;
        p.type = LuaPropertyType::Float;
        p.name = lua_tostring(L, 1);
        p.tooltip = lua_tostring(L, 2);

        lua_getglobal(L, m_pCurrentComponentDef->luaName.c_str());
        lua_getfield(L, -1, p.name.c_str());
        p.floatValue = (float)lua_tonumber(L, -1);
        lua_pop(L, 2);

        m_pCurrentComponentDef->properties.push_back(p);
        return 0;
    }

    int LuaBindings::funcSetVec2Property(lua_State* L)
    {
        if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2))
        {
            CORE_ERROR_POPUP("Lua: SetVec2Property expected (string, string)");
            return 0;
        }
        if (!m_pCurrentComponentDef)
        {
            CORE_ERROR_POPUP("Lua: SetVec2Property can only be called in initComponent()");
            return 0;
        }

        LuaProperty p;
        p.type = LuaPropertyType::Vec2;
        p.name = lua_tostring(L, 1);
        p.tooltip = lua_tostring(L, 2);

        lua_getglobal(L, m_pCurrentComponentDef->luaName.c_str());
        lua_getfield(L, -1, p.name.c_str());
        lua_getfield(L, -1, "x"); p.vec2Value.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "y"); p.vec2Value.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pop(L, 2);

        m_pCurrentComponentDef->properties.push_back(p);
        return 0;
    }

    int LuaBindings::funcSetColorProperty(lua_State* L)
    {
        if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2))
        {
            CORE_ERROR_POPUP("Lua: SetColorProperty expected (string, string)");
            return 0;
        }
        if (!m_pCurrentComponentDef)
        {
            CORE_ERROR_POPUP("Lua: SetColorProperty can only be called in initComponent()");
            return 0;
        }

        LuaProperty p;
        p.type = LuaPropertyType::Color;
        p.name = lua_tostring(L, 1);
        p.tooltip = lua_tostring(L, 2);

        lua_getglobal(L, m_pCurrentComponentDef->luaName.c_str());
        lua_getfield(L, -1, p.name.c_str());
        lua_getfield(L, -1, "r"); p.colorValue.r = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "g"); p.colorValue.g = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "b"); p.colorValue.b = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, -1, "a"); p.colorValue.a = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pop(L, 2);

        m_pCurrentComponentDef->properties.push_back(p);
        return 0;
    }

    int LuaBindings::funcSetStringProperty(lua_State* L)
    {
        if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2))
        {
            CORE_ERROR_POPUP("Lua: SetStringProperty expected (string, string)");
            return 0;
        }
        if (!m_pCurrentComponentDef)
        {
            CORE_ERROR_POPUP("Lua: SetStringProperty can only be called in initComponent()");
            return 0;
        }

        LuaProperty p;
        p.type = LuaPropertyType::String;
        p.name = lua_tostring(L, 1);
        p.tooltip = lua_tostring(L, 2);

        lua_getglobal(L, m_pCurrentComponentDef->luaName.c_str());
        lua_getfield(L, -1, p.name.c_str());
        p.stringValue = lua_tostring(L, -1);
        lua_pop(L, 2);

        m_pCurrentComponentDef->properties.push_back(p);
        return 0;
    }

    int LuaBindings::funcSendEvent(lua_State* L)
    {
        if (lua_gettop(L) < 1 || !lua_isstring(L, 1))
        {
            CORE_ERROR_POPUP("Lua: SendEvent expected (string, table{optional})");
            return 0;
        }

        static uint64_t nextEventDataId = 1;

        std::string eventName = lua_tostring(L, 1);
        std::string dataName = "";

        if (lua_gettop(L) >= 2)
        {
            dataName = "EDAT_" + std::to_string(nextEventDataId++);
            lua_getglobal(L, "EVTS_t");
            lua_pushvalue(L, 2);
            lua_setfield(L, -2, dataName.c_str());
            lua_pop(L, 1);
        }

        getEventSystem()->sendEvent(new LuaEvent(eventName, dataName));

        return 0;
    }

    int LuaBindings::funcGetPosition(lua_State* L)
    {
        auto pScriptComponent = LUA_GET_SCRIPT_COMPONENT(1);
        LUA_PUSH_VEC2(pScriptComponent->getEntity()->getPosition());
        return 1;
    }

    int LuaBindings::funcSetPosition(lua_State* L)
    {
        auto pScriptComponent = LUA_GET_SCRIPT_COMPONENT(1);
        auto pos = LUA_GET_VEC2(2, glm::vec2(0));
        pScriptComponent->getEntity()->setPosition(pos);
        return 0;
    }

    int LuaBindings::funcGetWorldPosition(lua_State* L)
    {
        auto pScriptComponent = LUA_GET_SCRIPT_COMPONENT(1);
        LUA_PUSH_VEC2(pScriptComponent->getEntity()->getWorldPosition());
        return 1;
    }

    int LuaBindings::funcSetWorldPosition(lua_State* L)
    {
        auto pScriptComponent = LUA_GET_SCRIPT_COMPONENT(1);
        auto pos = LUA_GET_VEC2(2, glm::vec2(0));
        pScriptComponent->getEntity()->setWorldPosition(pos);
        return 0;
    }

    int LuaBindings::funcGetRotation(lua_State* L)
    {
        auto pScriptComponent = LUA_GET_SCRIPT_COMPONENT(1);
        lua_pushnumber(L, (lua_Number)pScriptComponent->getEntity()->getRotation());
        return 1;
    }

    int LuaBindings::funcSetRotation(lua_State* L)
    {
        auto pScriptComponent = LUA_GET_SCRIPT_COMPONENT(1);
        auto angle = LUA_GET_NUMBER(2, 0.0f);
        pScriptComponent->getEntity()->setRotation(angle);
        return 0;
    }

    int LuaBindings::funcGetScale(lua_State* L)
    {
        auto pScriptComponent = LUA_GET_SCRIPT_COMPONENT(1);
        LUA_PUSH_VEC2(pScriptComponent->getEntity()->getScale());
        return 1;
    }

    int LuaBindings::funcSetScale(lua_State* L)
    {
        auto pScriptComponent = LUA_GET_SCRIPT_COMPONENT(1);
        auto scale = LUA_GET_VEC2(2, glm::vec2(1));
        pScriptComponent->getEntity()->setScale(scale);
        return 0;
    }
}
