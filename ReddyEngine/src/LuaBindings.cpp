extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "Engine/LuaBindings.h"
#include "Engine/Log.h"
#include "Engine/Utils.h"

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

glm::vec2 LUA_GET_VEC2_impl(lua_State* L, int stackIndex)
{
    if (lua_gettop(L) < 1 || !lua_istable(L, stackIndex)) return  {0, 0};
    glm::vec2 v;
    lua_getfield(L, stackIndex, "x"); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, stackIndex, "y"); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    return v;
}

glm::vec4 LUA_GET_COLOR_impl(lua_State* L, int stackIndex)
{
    if (lua_gettop(L) < 1 || !lua_istable(L, stackIndex)) return {1, 1, 1, 1};
    glm::vec4 v;
    lua_getfield(L, stackIndex, "r"); v.r = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, stackIndex, "g"); v.g = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, stackIndex, "b"); v.b = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    lua_getfield(L, stackIndex, "a"); v.a = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    return v;
}

// [dsl] Found this code on stackoverflow. Turns out it's really hard to deep copy a table in Lua
static int deepCopyLuaTable(lua_State* L,int n,int CacheT)
{
    int copyIndex = 0;
    switch (lua_type(L, n))
    {
    case LUA_TNIL:
        lua_pushnil(L);
        copyIndex = lua_gettop(L);
        break;
    case LUA_TBOOLEAN:
        lua_pushboolean(L, lua_toboolean(L, n));
        copyIndex = lua_gettop(L);
        break;
    case LUA_TNUMBER:
        lua_pushnumber(L, lua_tonumber(L, n));
        copyIndex = lua_gettop(L);
        break;
    case LUA_TSTRING:
        lua_pushlstring(L, lua_tostring(L, n), lua_rawlen(L, n));
        copyIndex = lua_gettop(L);
        break;
    case LUA_TLIGHTUSERDATA:
    case LUA_TUSERDATA:
        lua_pushlightuserdata(L, (void*)lua_touserdata(L, n));
        copyIndex = lua_gettop(L);
        break;
    case LUA_TFUNCTION:
        lua_pushvalue(L, n);
        copyIndex = lua_gettop(L);
        break;
    case LUA_TTHREAD:
        lua_pushvalue(L, n);
        copyIndex = lua_gettop(L);
        break;
    case LUA_TTABLE:
    {

            //push key
            lua_pushvalue(L, n);
            //try to get cached obj(should pop key from stack and push get value onto stack)
            int type = lua_gettable(L, CacheT);
            if (type == LUA_TTABLE)
            {
                //just return
                copyIndex = lua_gettop(L);//push 1
            }
            else 
            {
                //pop the pushed get table return value
                lua_pop(L, 1);
                {
                    lua_newtable(L);
                    copyIndex = lua_gettop(L);


                    //push key
                    lua_pushvalue(L, n);
                    //push value
                    lua_pushvalue(L, copyIndex);
                    //save created table into cacheT
                    lua_settable(L, CacheT);


                    /* table is in the stack at index 't' */
                    lua_pushnil(L);  /* first key */
                    while (lua_next(L, n) != 0) {
                        /* uses 'key' (at index -2) and 'value' (at index -1) */
                        int keyIndex = lua_absindex(L, -2);//1
                        int valueIndex = lua_absindex(L, -1);//2
                        int copyedKey = deepCopyLuaTable(L, keyIndex, CacheT);//3
                        int copyedValue = deepCopyLuaTable(L, valueIndex, CacheT);//4
                        //push key
                        lua_pushvalue(L, copyedKey);
                        //push value
                        lua_pushvalue(L, copyedValue);
                        lua_settable(L, copyIndex);
                        /* removes 'value'; keeps 'key' for next iteration */
                        lua_pop(L, 3);
                    }

                    if (1 == lua_getmetatable(L, n))//try to get metatable of n(push onto stack if return 1)
                    {
                        int metaIndex = lua_gettop(L);
                        metaIndex = lua_absindex(L, -1);
                        //push 1
                        int copyedMeta = deepCopyLuaTable(L, metaIndex, CacheT);//try to copy meta table push onto stack
                        lua_setmetatable(L, copyIndex);//set meta table and pop copyedMeta
                        lua_pop(L, 1);//pop lua_getmetatable pushed value
                    }
                    else
                    {
                        ;//do nothing
                    }
                }
            }
        break;
    }

    }
    return copyIndex;
}

//following c++ equals lua logic like this:
/*
function _G.tclone(value)
    local function __tclone(value,cached)
        local copy
        local cacheT = cached or {}
        if type(value) == 'table' then
            --if has been already cloned just return handle recursive
            if nil ~= cacheT[value] then
                copy =   cacheT[value]
            else
                copy = {}
                cacheT[value] = copy
                for k,v in pairs(value) do
                    --clone key                --clone value
                    copy[__tclone(k,cacheT)] = __tclone(v,cacheT)
                end
                --clone metatable
                setmetatable(copy, __tclone(getmetatable(value), cacheT))
            end
        else
            copy = value
        end
        return copy
    end
    return __tclone(value,nil)
end
---
*/
int cloneLuaTable(lua_State* L, int n)
{
    int cacheT;
    int copy;
    lua_newtable(L);
    cacheT = lua_gettop(L);
    copy = deepCopyLuaTable(L, n, cacheT);
    lua_remove(L, cacheT);
    return copy;
}
