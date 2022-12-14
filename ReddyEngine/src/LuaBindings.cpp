extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "Engine/LuaBindings.h"
#include "Engine/Log.h"
#include "Engine/Utils.h"
#include "Engine/ScriptComponent.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/ReddyEngine.h"
#include "Engine/Event.h"
#include "Engine/EventSystem.h"

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
        if (L) lua_close(L);
        L = nullptr;
        m_prefabCache.clear();
    }

    void LuaBindings::init(bool doRunFiles)
    {
        L = luaL_newstate();
        luaL_openlibs(L);

        lua_newtable(L);
        lua_setglobal(L, "EINS_t"); // Active entities

        lua_newtable(L);
        lua_setglobal(L, "CINS_t"); // Active components

        lua_newtable(L);
        lua_setglobal(L, "EVTS_t"); // Active event data

        lua_newtable(L);
        lua_setglobal(L, "EVTL_t"); // Active event listeners

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

    ScriptComponentRef LuaBindings::getScriptComponentFromListener(void* pListener, std::string name, std::string* callbackName)
    {
        auto itm = m_scriptEventListeners.find(name);
        if (itm == m_scriptEventListeners.end()) return nullptr;

        // Find our event listener
        auto it = itm->second.find((uintptr_t)pListener);
        if (it == itm->second.end()) return nullptr;

        // Get ScriptComponent, or remove if it weak reference is null
        const auto& scriptEventListener = it->second;
        auto pScriptComponent = scriptEventListener.pScript.lock();
        if (!pScriptComponent)
        {
            itm->second.erase(it);
            return nullptr;
        }

        *callbackName = scriptEventListener.name;
        return pScriptComponent;
    }

    void LuaBindings::onKeyDown(IEvent* pEvent)
    {
        auto pKeyDown = (KeyDownEvent*)pEvent;
        std::string name;
        auto pScriptComponent = getScriptComponentFromListener(pEvent->pListener, "KeyDown", &name);
        if (!pScriptComponent)
        {
            getEventSystem()->deregisterListener<KeyDownEvent>(pEvent->pListener);
            return;
        }

        // Call the function!
        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, pScriptComponent->luaName.c_str());
        lua_getfield(L, -1, name.c_str());

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            lua_pushinteger(L, (lua_Integer)pKeyDown->key.keysym.scancode);
            checkLua(L, lua_pcall(L, 2, 0, 0));
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaBindings::onKeyUp(IEvent* pEvent)
    {
        auto pKeyUp = (KeyUpEvent*)pEvent;
        std::string name;
        auto pScriptComponent = getScriptComponentFromListener(pEvent->pListener, "KeyUp", &name);
        if (!pScriptComponent)
        {
            getEventSystem()->deregisterListener<KeyUpEvent>(pEvent->pListener);
            return;
        }

        // Call the function!
        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, pScriptComponent->luaName.c_str());
        lua_getfield(L, -1, name.c_str());

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            lua_pushinteger(L, (lua_Integer)pKeyUp->key.keysym.scancode);
            checkLua(L, lua_pcall(L, 2, 0, 0));
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaBindings::onMouseDown(IEvent* pEvent)
    {
        auto pMouseDown = (MouseButtonDownEvent*)pEvent;
        std::string name;
        auto pScriptComponent = getScriptComponentFromListener(pEvent->pListener, "MouseDown", &name);
        if (!pScriptComponent)
        {
            getEventSystem()->deregisterListener<MouseButtonDownEvent>(pEvent->pListener);
            return;
        }

        // Call the function!
        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, pScriptComponent->luaName.c_str());
        lua_getfield(L, -1, name.c_str());

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            lua_pushinteger(L, (lua_Integer)pMouseDown->button.button);
            checkLua(L, lua_pcall(L, 2, 0, 0));
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaBindings::onMouseUp(IEvent* pEvent)
    {
        auto pMouseUp = (MouseButtonUpEvent*)pEvent;
        std::string name;
        auto pScriptComponent = getScriptComponentFromListener(pEvent->pListener, "MouseUp", &name);
        if (!pScriptComponent)
        {
            getEventSystem()->deregisterListener<MouseButtonUpEvent>(pEvent->pListener);
            return;
        }

        // Call the function!
        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, pScriptComponent->luaName.c_str());
        lua_getfield(L, -1, name.c_str());

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            lua_pushinteger(L, (lua_Integer)pMouseUp->button.button);
            checkLua(L, lua_pcall(L, 2, 0, 0));
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaBindings::onLuaEvent(IEvent* pEvent)
    {
        auto pLuaEvent = (LuaEvent*)pEvent;
        std::string name;
        auto pScriptComponent = getScriptComponentFromListener(pEvent->pListener, pLuaEvent->name, &name);
        if (!pScriptComponent)
        {
            getEventSystem()->deregisterLuaListener(pLuaEvent->name, pEvent->pListener);
            return;
        }

        // Call the function!
        lua_getglobal(L, "EVTS_t");
        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, pScriptComponent->luaName.c_str());
        lua_getfield(L, -1, name.c_str());

        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            lua_getfield(L, -5, pLuaEvent->dataLuaName.c_str());

            checkLua(L, lua_pcall(L, 2, 0, 0));
        }

        lua_pop(L, lua_gettop(L));
    }

    void LuaBindings::update(float dt)
    {
        if (m_stateChangeRequest != StateChangeRequest::None)
        {
            auto s = m_stateChangeRequest;
            m_stateChangeRequest = StateChangeRequest::None;
            getGame()->changeState(s, m_worldFilenameToLoad);
        }
    }

    void LuaBindings::fixedUpdate(float dt)
    {
        // Nothing to do here
    }

    void LuaBindings::runFiles()
    {
        auto luaFiles = Utils::findAllFiles("assets/scripts", "LUA", true);

        // Put core.lua first, as other files will refer to it
        for (auto it = luaFiles.begin(); it != luaFiles.end(); ++it)
        {
            if (Utils::getFilename(*it) == "core.lua")
            {
                auto file = *it;
                luaFiles.erase(it);
                luaFiles.insert(luaFiles.begin(), file);
                break;
            }
        }

        // Run them
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
    if (lua_gettop(L) < stackIndex || !lua_istable(L, stackIndex)) return  {0, 0};
    glm::vec2 v = defaultValue;
    lua_getfield(L, stackIndex, "x"); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, stackIndex, "y"); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    return v;
}

glm::vec4 LUA_GET_COLOR_impl(lua_State* L, int stackIndex, const glm::vec4& defaultValue)
{
    if (lua_gettop(L) < stackIndex || !lua_istable(L, stackIndex)) return {1, 1, 1, 1};
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

Engine::EntityRef LUA_GET_ENTITY_impl(lua_State* L, int stackIndex, const char* funcName)
{
    if (lua_gettop(L) < stackIndex) return nullptr;
    if (lua_isstring(L, stackIndex)) return Engine::getScene()->getEntityByName(lua_tostring(L, stackIndex), true);
    if (!lua_istable(L, stackIndex)) return nullptr;

    lua_getfield(L, stackIndex, "EOBJ");
    if (lua_islightuserdata(L, -1))
    {
        auto pEntity = ((Engine::Entity*)lua_topointer(L, -1))->shared_from_this();
        lua_pop(L, 1);
        return pEntity;
    }
    lua_pop(L, 1);

    lua_getfield(L, stackIndex, "COBJ");
    if (!lua_islightuserdata(L, -1))
    {
        lua_pop(L, 1);
        return nullptr;
    }

    auto pScriptComponent = (Engine::ScriptComponent*)lua_topointer(L, -1);
    lua_pop(L, 1);
    return pScriptComponent->getEntity();
}

Engine::ScriptComponent* LUA_GET_SCRIPT_COMPONENT_impl(lua_State* L, int stackIndex, const char* funcName)
{
    lua_getfield(L, stackIndex, "COBJ");
    if (!lua_islightuserdata(L, -1))
    {
        CORE_ERROR_POPUP("Lua: {} Argument at {} should be a ScriptComponent.", funcName, stackIndex);
        return nullptr;
    }
    auto pScriptComponent = (Engine::ScriptComponent*)lua_topointer(L, -1);
    lua_pop(L, 1);
    return pScriptComponent;
}
