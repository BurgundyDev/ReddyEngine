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

    struct LuaMethodDef
    {
        bool available = false;
        std::string name;
    };

    struct LuaComponentDef
    {
        std::string luaName;
        std::vector<LuaProperty> properties;
        LuaMethodDef onInit;
        LuaMethodDef onCreate;
        LuaMethodDef onUpdate;
        LuaMethodDef onFixedUpdate;
        LuaMethodDef onDestroy;
        LuaMethodDef onMouseEnter;
        LuaMethodDef onMouseLeave;
        LuaMethodDef onMouseDown;
        LuaMethodDef onMouseUp;
        LuaMethodDef onMouseClick;
        LuaMethodDef onEvent;
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

    private:
        void createBindings();

        lua_State* L = nullptr;

        std::map<std::string, LuaComponentDef*> m_componentDefs;
        LuaComponentDef* m_pCurrentComponentDef = nullptr;
    };
}

glm::vec2 LUA_GET_VEC2_impl(lua_State* L, int stackIndex);
glm::vec4 LUA_GET_COLOR_impl(lua_State* L, int stackIndex);

#define LUA_PUSH_VEC2(v) {lua_getglobal(L, "Vec2"); lua_pushnumber(L, v.x); lua_pushnumber(L, v.y); lua_pcall(L, 2, 1, 0);}
#define LUA_PUSH_COLOR(c) {lua_getglobal(L, "Color"); lua_pushnumber(L, c.r); lua_pushnumber(L, c.g); lua_pushnumber(L, c.b); lua_pushnumber(L, c.a); lua_pcall(L, 4, 1, 0);}

#define LUA_GET_VEC2(i) LUA_GET_VEC2_impl(L, i)
#define LUA_GET_COLOR(i) LUA_GET_COLOR_impl(L, i)
