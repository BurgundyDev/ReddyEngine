extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "Engine/LuaBindings.h"
#include "Engine/Log.h"
#include "Engine/ReddyEngine.h"


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
}
