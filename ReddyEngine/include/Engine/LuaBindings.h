#pragma once

#include <Engine/IGame.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <vector>
#include <map>
#include <memory>


extern "C" {
    struct lua_State;
}


namespace Engine
{
    class ScriptComponent;

    class Entity;
    using EntityRef = std::shared_ptr<Entity>;


    enum class LuaPropertyType
    {
        Bool,
        Int,
        Float,
        Vec2,
        Color,
        String
    };


    // And those are copied for each instances. Pretty much a waste of ram
    struct LuaProperty
    {
        LuaPropertyType type;

        bool boolValue;
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
        int funcSetBoolProperty(lua_State* L);
        int funcSetIntProperty(lua_State* L);
        int funcSetFloatProperty(lua_State* L);
        int funcSetVec2Property(lua_State* L);
        int funcSetColorProperty(lua_State* L);
        int funcSetStringProperty(lua_State* L);

        int funcGetComponent(lua_State* L);
        int funcGetEntity(lua_State* L);
        int funcDestroy(lua_State* L);

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
        
        int funcSendEvent(lua_State* L);
        int funcIsKeyDown(lua_State* L);
        int funcIsButtonDown(lua_State* L);
        int funcPlaySound(lua_State* L);

        int funcGetSpriteTexture(lua_State* L);
        int funcSetSpriteTexture(lua_State* L);
        int funcGetSpriteColor(lua_State* L);
        int funcSetSpriteColor(lua_State* L);
        int funcGetSpriteOrigin(lua_State* L);
        int funcSetSpriteOrigin(lua_State* L);
        int funcGetFont(lua_State* L);
        int funcSetFont(lua_State* L);
        int funcGetText(lua_State* L);
        int funcSetText(lua_State* L);
        int funcGetTextColor(lua_State* L);
        int funcSetTextColor(lua_State* L);
        int funcGetTextOrigin(lua_State* L);
        int funcSetTextOrigin(lua_State* L);
        int funcGetTextScale(lua_State* L);
        int funcSetTextScale(lua_State* L);
        int funcGetName(lua_State* L);
        int funcSetName(lua_State* L);

        int funcFindEntityByName(lua_State* L);
        int funcFindEntityByComponent(lua_State* L);

        int funcContinueGame(lua_State* L);
        int funcNewGame(lua_State* L);
        int funcQuit(lua_State* L);
        int funcEditor(lua_State* L);
        int funcResume(lua_State* L);
        int funcMainMenu(lua_State* L);

        int funcPlayMusic(lua_State* L);
        int funcStopMusic(lua_State* L);
        int funcPauseMusic(lua_State* L);
        int funcResumeMusic(lua_State* L);

    private:
        void createBindings();

        lua_State* L = nullptr;

        std::map<std::string, LuaComponentDef*> m_componentDefs;
        LuaComponentDef* m_pCurrentComponentDef = nullptr;
        StateChangeRequest m_stateChangeRequest = StateChangeRequest::None;
        std::string m_worldFilenameToLoad;
    };
}


// Helper macros
#define LUA_PUSH_VEC2(v) {lua_getglobal(L, "Vec2"); lua_pushnumber(L, v.x); lua_pushnumber(L, v.y); lua_pcall(L, 2, 1, 0);}
#define LUA_PUSH_COLOR(c) {lua_getglobal(L, "Color"); lua_pushnumber(L, c.r); lua_pushnumber(L, c.g); lua_pushnumber(L, c.b); lua_pushnumber(L, c.a); lua_pcall(L, 4, 1, 0);}
#define LUA_PUSH_ENTITY(e) {lua_getglobal(L, "EINS_t"); if (e) lua_getfield(L, -1, e->luaName.c_str()); else lua_pushnil(L); }

#define LUA_GET_INT(i, defaultValue) LUA_GET_INT_impl(L, i, defaultValue)
#define LUA_GET_NUMBER(i, defaultValue) LUA_GET_NUMBER_impl(L, i, defaultValue)
#define LUA_GET_VEC2(i, defaultValue) LUA_GET_VEC2_impl(L, i, defaultValue)
#define LUA_GET_COLOR(i, defaultValue) LUA_GET_COLOR_impl(L, i, defaultValue)
#define LUA_GET_STRING(i, defaultValue) LUA_GET_STRING_impl(L, i, defaultValue)
#define LUA_GET_ENTITY(i) LUA_GET_ENTITY_impl(L, i, __func__)
#define LUA_GET_COMPONENT(i, component) LUA_GET_COMPONENT_impl<component>(L, i, __func__)

#define LUA_GET_SCRIPT_COMPONENT(i) LUA_GET_SCRIPT_COMPONENT_impl(L, i, __func__)

#define LUA_CLONE_TABLE(L, n) cloneLuaTable(L, n)


// Prototypes
int LUA_GET_INT_impl(lua_State* L, int stackIndex, int defaultValue);
float LUA_GET_NUMBER_impl(lua_State* L, int stackIndex, float defaultValue);
glm::vec2 LUA_GET_VEC2_impl(lua_State* L, int stackIndex, const glm::vec2& defaultValue);
glm::vec4 LUA_GET_COLOR_impl(lua_State* L, int stackIndex, const glm::vec4& defaultValue);
std::string LUA_GET_STRING_impl(lua_State* L, int stackIndex, const std::string& defaultValue);
Engine::EntityRef LUA_GET_ENTITY_impl(lua_State* L, int stackIndex, const char* funcName);
template<typename T>
std::shared_ptr<T> LUA_GET_COMPONENT_impl(lua_State* L, int stackIndex, const char* funcName)
{
    auto pEntity = LUA_GET_ENTITY(stackIndex);
    if (pEntity) return pEntity->getComponent<T>();
    return nullptr;
}

Engine::ScriptComponent* LUA_GET_SCRIPT_COMPONENT_impl(lua_State* L, int stackIndex, const char* funcName);


int cloneLuaTable(lua_State* L, int n);
bool checkLua(lua_State* L, int r);
