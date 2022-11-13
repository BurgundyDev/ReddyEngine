extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "Engine/LuaBindings.h"
#include "Engine/Log.h"
#include "Engine/Utils.h"
#include "Engine/ScriptComponent.h"

#include <filesystem>


bool checkLua(lua_State* L, int r)
{
    if (r != LUA_OK)
    {
        CORE_ERROR_POPUP(lua_tostring(L, -1));
        return false;
    }
    return true;
}


namespace Engine
{
    LuaBindings::LuaBindings()
    {
    }

    void LuaBindings::clear()
    {
        for (const auto& kv : m_componentDefs) delete kv.second;
        m_componentDefs.clear();
        lua_close(L);
    }

    void LuaBindings::init(bool doRunFiles)
    {
        L = luaL_newstate();
        luaL_openlibs(L);

        lua_newtable(L);
        lua_setglobal(L, "CINS_t"); // Active components
        lua_newtable(L);
        lua_setglobal(L, "EVTS_t"); // Active event data

        createBindings();
        if (doRunFiles)
        {
            runFiles();
            initComponents();
        }
    }

    LuaBindings::~LuaBindings()
    {
        for (const auto& kv : m_componentDefs) delete kv.second;
        lua_close(L);
    }

    void LuaBindings::update(float dt)
    {
        // Nothing to do here
    }

    void LuaBindings::fixedUpdate(float dt)
    {
        // Nothing to do here
    }

    void LuaBindings::runFiles()
    {
        auto luaFiles = Utils::findAllFiles("assets/scripts", "LUA", true);
        for (const auto& luaFile : luaFiles)
            checkLua(L, luaL_dofile(L, luaFile.c_str()));
    }
    
    LuaComponentDef* LuaBindings::getComponentDef(const std::string& name) const
    {
        auto it = m_componentDefs.find(name);
        if (it == m_componentDefs.end()) return nullptr;
        return it->second;
    }

    void LuaBindings::initComponents()
    {
        for (const auto& kv : m_componentDefs)
        {
            m_pCurrentComponentDef = kv.second;
            
            lua_getglobal(L, m_pCurrentComponentDef->luaName.c_str());
            lua_getfield(L, -1, "initComponent");
            auto top = lua_gettop(L);
            if (lua_isfunction(L, -1))
            {
                lua_pcall(L, 0, 0, 0);
            }
            top = lua_gettop(L);
            lua_pop(L, 1);
        }

        m_pCurrentComponentDef = nullptr;
    }
}

int LUA_GET_INT_impl(lua_State* L, int stackIndex, int defaultValue)
{
    if (lua_gettop(L) >= stackIndex && lua_isnumber(L, stackIndex))
        defaultValue = (int)lua_tointeger(L, stackIndex);
    return defaultValue;
}

float LUA_GET_NUMBER_impl(lua_State* L, int stackIndex, float defaultValue)
{
    if (lua_gettop(L) >= stackIndex && lua_isnumber(L, stackIndex))
        defaultValue = (float)lua_tonumber(L, stackIndex);
    return defaultValue;
}

glm::vec2 LUA_GET_VEC2_impl(lua_State* L, int stackIndex, const glm::vec2& defaultValue)
{
    if (lua_gettop(L) < 1 || !lua_istable(L, stackIndex)) return  {0, 0};
    glm::vec2 v = defaultValue;
    lua_getfield(L, stackIndex, "x"); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, stackIndex, "y"); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    return v;
}

glm::vec4 LUA_GET_COLOR_impl(lua_State* L, int stackIndex, const glm::vec4& defaultValue)
{
    if (lua_gettop(L) < 1 || !lua_istable(L, stackIndex)) return {1, 1, 1, 1};
    glm::vec4 v = defaultValue;
    lua_getfield(L, stackIndex, "r"); v.r = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, stackIndex, "g"); v.g = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, stackIndex, "b"); v.b = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, stackIndex, "a"); v.a = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    return v;
}

std::string LUA_GET_STRING_impl(lua_State* L, int stackIndex, const std::string& defaultValue)
{
    if (lua_gettop(L) >= stackIndex && lua_isstring(L, stackIndex))
        return lua_tostring(L, stackIndex);
    return defaultValue;
}

Engine::ScriptComponent* LUA_GET_SCRIPT_COMPONENT_impl(lua_State* L, int stackIndex, const char* funcName)
{
    lua_getfield(L, 1, "COBJ");
    if (!lua_islightuserdata(L, -1))
    {
        CORE_ERROR_POPUP("Lua: {} Argument at {} should be a ScriptComponent.", funcName, stackIndex);
        return nullptr;
    }
    auto pScriptComponent = (Engine::ScriptComponent*)lua_topointer(L, -1);
    lua_pop(L, 1);
    return pScriptComponent;
}
