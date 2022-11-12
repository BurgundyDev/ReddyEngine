#pragma once

#include <lua.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <vector>
#include <map>


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

        void init();
        void update(float dt);
        void fixedUpdate(float dt);

        LuaComponentDef* getComponentDef(const std::string& name) const;

        int funcRegisterComponent(lua_State* L);
        int funcSetFloatProperty(lua_State* L);

    private:
        void createBindings();
        void runFiles();
        void initComponents();

        lua_State* L = nullptr;

        std::map<std::string, LuaComponentDef*> m_componentDefs;
        LuaComponentDef* m_pCurrentComponentDef = nullptr;
    };
}
