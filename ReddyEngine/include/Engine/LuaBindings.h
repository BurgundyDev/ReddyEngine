#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <vector>
#include <map>

extern "C" {
    struct lua_State;
}


namespace Engine
{
    class ScriptComponent;


    enum class LuaPropertyType
    {
        Int,
        Float,
        Vec2,
        Color,
        String
    };


    struct LuaProperty
    {
        LuaPropertyType type;

        int intValue;
        float floatValue;
        glm::vec2 vec2Value;
        glm::vec4 colorValue;
        std::string stringValue;

        std::string name;
        std::string tooltip;
    };


    struct LuaComponentDef
    {
        std::string luaName;
        std::vector<LuaProperty> properties;
    };


    class LuaBindings final
    {
    public:
        LuaBindings();
        ~LuaBindings();

        void init(bool doRunFiles = true);
        void runFiles();
        void initComponents();

        void update(float dt);
        void fixedUpdate(float dt);
        void clear();

        LuaComponentDef* getComponentDef(const std::string& name) const;
        lua_State* getState() const { return L; }

        int funcRegisterComponent(lua_State* L);
        int funcSetIntProperty(lua_State* L);
        int funcSetFloatProperty(lua_State* L);
        int funcSetVec2Property(lua_State* L);
        int funcSetColorProperty(lua_State* L);
        int funcSetStringProperty(lua_State* L);

        int funcSendEvent(lua_State* L);
        
        int funcGetPosition(lua_State* L);
        int funcSetPosition(lua_State* L);
        int funcGetWorldPosition(lua_State* L);
        int funcSetWorldPosition(lua_State* L);
        int funcGetRotation(lua_State* L);
        int funcSetRotation(lua_State* L);
        int funcGetScale(lua_State* L);
        int funcSetScale(lua_State* L);

        int funcLength(lua_State* L);
        int funcDistance(lua_State* L);
        int funcNormalize(lua_State* L);
        int funcDot(lua_State* L);

    private:
        void createBindings();

        lua_State* L = nullptr;

        std::map<std::string, LuaComponentDef*> m_componentDefs;
        LuaComponentDef* m_pCurrentComponentDef = nullptr;
    };
}


// Helper macros
#define LUA_PUSH_VEC2(v) {lua_getglobal(L, "Vec2"); lua_pushnumber(L, v.x); lua_pushnumber(L, v.y); lua_pcall(L, 2, 1, 0);}
#define LUA_PUSH_COLOR(c) {lua_getglobal(L, "Color"); lua_pushnumber(L, c.r); lua_pushnumber(L, c.g); lua_pushnumber(L, c.b); lua_pushnumber(L, c.a); lua_pcall(L, 4, 1, 0);}

#define LUA_GET_VEC2(i, defaultValue) LUA_GET_VEC2_impl(L, i, defaultValue)
#define LUA_GET_COLOR(i) LUA_GET_COLOR_impl(L, i)
#define LUA_GET_NUMBER(i, defaultValue) LUA_GET_NUMBER_impl(L, i, defaultValue)
#define LUA_GET_SCRIPT_COMPONENT(i) LUA_GET_SCRIPT_COMPONENT_impl(L, i, __func__)

#define LUA_CLONE_TABLE(L, n) cloneLuaTable(L, n)


// Prototypes
glm::vec2 LUA_GET_VEC2_impl(lua_State* L, int stackIndex, const glm::vec2& defaultValue);
glm::vec4 LUA_GET_COLOR_impl(lua_State* L, int stackIndex);
float LUA_GET_NUMBER_impl(lua_State* L, int stackIndex, float defaultValue);
Engine::ScriptComponent* LUA_GET_SCRIPT_COMPONENT_impl(lua_State* L, int stackIndex, const char* funcName);

int cloneLuaTable(lua_State* L, int n);
bool checkLua(lua_State* L, int r);
