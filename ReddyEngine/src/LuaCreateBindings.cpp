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
#include "Engine/Scene.h"
#include "Engine/Input.h"
#include "Engine/Audio.h"
#include "Engine/Sound.h"
#include "Engine/ResourceManager.h"
#include "Engine/SpriteComponent.h"
#include "Engine/PFXComponent.h"
#include "Engine/TextComponent.h"
#include "Engine/MusicManager.h"


namespace Engine
{
    LuaBindings* g_pLuaBindings = nullptr;

    void LuaBindings::createBindings()
    {
        g_pLuaBindings = this; // We cache is so we don't go through getLuaBindings and dereference a shared_ptr.
#define LUA_REGISTER(name) lua_register(L, #name, [](lua_State* L){return g_pLuaBindings->func ## name(L);})

        LUA_REGISTER(RegisterComponent);
        LUA_REGISTER(SetBoolProperty);
        LUA_REGISTER(SetIntProperty);
        LUA_REGISTER(SetFloatProperty);
        LUA_REGISTER(SetVec2Property);
        LUA_REGISTER(SetColorProperty);
        LUA_REGISTER(SetStringProperty);
        LUA_REGISTER(GetComponent);
        LUA_REGISTER(GetEntity);
        LUA_REGISTER(GetRoot);
        LUA_REGISTER(GetParent);
        LUA_REGISTER(AddChild);
        LUA_REGISTER(CreateEntity);
        LUA_REGISTER(Destroy);
        LUA_REGISTER(AddComponent);
        LUA_REGISTER(RemoveComponent);
        LUA_REGISTER(GetPosition);
        LUA_REGISTER(SetPosition);
        LUA_REGISTER(GetWorldPosition);
        LUA_REGISTER(SetWorldPosition);
        LUA_REGISTER(GetRotation);
        LUA_REGISTER(SetRotation);
        LUA_REGISTER(GetScale);
        LUA_REGISTER(SetScale);
        LUA_REGISTER(Length);
        LUA_REGISTER(Distance);
        LUA_REGISTER(Normalize);
        LUA_REGISTER(Dot);
        LUA_REGISTER(GetMousePosition);
		LUA_REGISTER(IsKeyDown);
        LUA_REGISTER(IsButtonDown);
		LUA_REGISTER(IsButtonJustDown);
        LUA_REGISTER(PlaySound);
        LUA_REGISTER(GetSpriteTexture);
        LUA_REGISTER(SetSpriteTexture);
        LUA_REGISTER(GetSpriteColor);
        LUA_REGISTER(SetSpriteColor);
        LUA_REGISTER(GetSpriteOrigin);
        LUA_REGISTER(SetSpriteOrigin);
        LUA_REGISTER(GetFont);
        LUA_REGISTER(SetFont);
        LUA_REGISTER(GetText);
        LUA_REGISTER(SetText);
        LUA_REGISTER(GetTextColor);
        LUA_REGISTER(SetTextColor);
        LUA_REGISTER(GetTextOrigin);
        LUA_REGISTER(SetTextOrigin);
        LUA_REGISTER(GetTextScale);
        LUA_REGISTER(SetTextScale);
        LUA_REGISTER(GetPFX);
        LUA_REGISTER(SetPFX);
        LUA_REGISTER(PlayPFX);
        LUA_REGISTER(StopPFX);
        LUA_REGISTER(GetName);
        LUA_REGISTER(SetName);
        LUA_REGISTER(FindEntityByName);
        LUA_REGISTER(ContinueGame);
        LUA_REGISTER(NewGame);
        LUA_REGISTER(Quit);
        LUA_REGISTER(Editor);
        LUA_REGISTER(Resume);
        LUA_REGISTER(MainMenu);
        LUA_REGISTER(PlayMusic);
        LUA_REGISTER(StopMusic);
        LUA_REGISTER(PauseMusic);
        LUA_REGISTER(ResumeMusic);
        LUA_REGISTER(Log);
        LUA_REGISTER(GetScreenRect);
        LUA_REGISTER(EmitParticles);
        LUA_REGISTER(SendEvent);
        LUA_REGISTER(RegisterEvent);
        LUA_REGISTER(DeregisterEvent);
        LUA_REGISTER(EnableEntity);
        LUA_REGISTER(DisableEntity);
        LUA_REGISTER(EnableComponent);
        LUA_REGISTER(DisableComponent);
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

    int LuaBindings::funcSetBoolProperty(lua_State* L)
    {
        if (lua_gettop(L) != 2 || !lua_isstring(L, 1) || !lua_isstring(L, 2))
        {
            CORE_ERROR_POPUP("Lua: SetBoolProperty expected (string, string)");
            return 0;
        }
        if (!m_pCurrentComponentDef)
        {
            CORE_ERROR_POPUP("Lua: SetBoolProperty can only be called in initComponent()");
            return 0;
        }

        LuaProperty p;
        p.type = LuaPropertyType::Bool;
        p.name = lua_tostring(L, 1);
        p.tooltip = lua_tostring(L, 2);

        lua_getglobal(L, m_pCurrentComponentDef->luaName.c_str());
        lua_getfield(L, -1, p.name.c_str());
        p.boolValue = lua_toboolean(L, -1) ? true : false;
        lua_pop(L, 2);

        m_pCurrentComponentDef->properties.push_back(p);
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

    int LuaBindings::funcGetComponent(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (pEntity)
        {
            auto scriptComponentName = LUA_GET_STRING(2, "");
            if (!scriptComponentName.empty())
            {
                auto pComponents = pEntity->getComponents();
                for (const auto& pComponent : pComponents)
                {
                    auto pScriptComponent = std::dynamic_pointer_cast<ScriptComponent>(pComponent);
                    if (pScriptComponent)
                    {
                        if (pScriptComponent->name == scriptComponentName)
                        {
                            lua_getglobal(L, "CINS_t");
                            lua_getfield(L, -1, pScriptComponent->luaName.c_str());
                            return 1;
                        }
                    }
                }
            }
        }
        lua_pushnil(L);
        return 1;
    }

    int LuaBindings::funcGetEntity(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (pEntity)
        {
            lua_getglobal(L, "EINS_t");
            lua_getfield(L, -1, pEntity->luaName.c_str());
            return 1;
        }
        lua_pushnil(L);
        return 1;
    }

    int LuaBindings::funcGetPosition(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        auto v = pEntity ? pEntity->getPosition() : glm::vec2(0);
        LUA_PUSH_VEC2(v);
        return 1;
    }

    int LuaBindings::funcSetPosition(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (pEntity)
        {
            auto pos = LUA_GET_VEC2(2, glm::vec2(0));
            pEntity->setPosition(pos);
        }
        return 0;
    }

    int LuaBindings::funcGetWorldPosition(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        auto v = pEntity ? pEntity->getWorldPosition() : glm::vec2(0);
        LUA_PUSH_VEC2(v);
        return 1;
    }

    int LuaBindings::funcSetWorldPosition(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (pEntity)
        {
            auto pos = LUA_GET_VEC2(2, glm::vec2(0));
            pEntity->setWorldPosition(pos);
        }
        return 0;
    }

    int LuaBindings::funcGetRotation(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        lua_pushnumber(L, (lua_Number)(pEntity ? pEntity->getRotation() : 0.0f));
        return 1;
    }

    int LuaBindings::funcSetRotation(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (pEntity)
        {
            auto angle = LUA_GET_NUMBER(2, 0.0f);
            pEntity->setRotation(angle);
        }
        return 0;
    }

    int LuaBindings::funcGetScale(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        auto v = pEntity ? pEntity->getScale() : glm::vec2(1);
        LUA_PUSH_VEC2(v);
        return 1;
    }

    int LuaBindings::funcSetScale(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (pEntity)
        {
            auto scale = LUA_GET_VEC2(2, glm::vec2(1));
            pEntity->setScale(scale);
        }
        return 0;
    }

    int LuaBindings::funcLength(lua_State* L)
    {
        auto v = LUA_GET_VEC2(1, glm::vec2(0));
        lua_pushnumber(L, (lua_Number)v.length());
        return 1;
    }

    int LuaBindings::funcDistance(lua_State* L)
    {
        auto a = LUA_GET_VEC2(1, glm::vec2(0));
        auto b = LUA_GET_VEC2(2, glm::vec2(0));
        lua_pushnumber(L, (lua_Number)(glm::distance(a, b)));
        return 1;
    }

    int LuaBindings::funcNormalize(lua_State* L)
    {
        auto v = glm::normalize(LUA_GET_VEC2(1, glm::vec2(0)));
        LUA_PUSH_VEC2(v);
        return 1;
    }

    int LuaBindings::funcDot(lua_State* L)
    {
        auto a = LUA_GET_VEC2(1, glm::vec2(0));
        auto b = LUA_GET_VEC2(2, glm::vec2(0));
        lua_pushnumber(L, (lua_Number)(glm::dot(a, b)));
        return 1;
    }

	int LuaBindings::funcGetMousePosition(lua_State* L)
	{
		auto position = getScene()->getMousePos();
		LUA_PUSH_VEC2(position);
		return 1;
	}

    int LuaBindings::funcIsKeyDown(lua_State* L)
    {
        auto key = LUA_GET_INT(1, 0);
        auto isDown = getInput()->isKeyDown((SDL_Scancode)key);
        lua_pushboolean(L, isDown ? 1 : 0);
        return 1;
    }

    int LuaBindings::funcIsButtonDown(lua_State* L)
    {
        auto button = LUA_GET_INT(1, 0);
        auto isDown = getInput()->isButtonDown(button);
        lua_pushboolean(L, isDown ? 1 : 0);
        return 1;
    }

	int LuaBindings::funcIsButtonJustDown(lua_State* L)
	{
		auto button = LUA_GET_INT(1, 0);
		auto isDown = getInput()->isButtonJustDown((SDL_Scancode)button);
		lua_pushboolean(L, isDown ? 1 : 0);
		return 1;
	}

    int LuaBindings::funcPlaySound(lua_State* L)
    {
        auto filename = LUA_GET_STRING(1, "");
        auto vol = LUA_GET_NUMBER(2, 1.0f);
        auto bal = LUA_GET_NUMBER(3, 0.0f);
        auto pitch = LUA_GET_NUMBER(4, 1.0f);

        auto pSound = getResourceManager()->getSound(filename);
        if (pSound) pSound->play(vol, bal, pitch); 

        return 0;
    }

    int LuaBindings::funcDestroy(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (pEntity) getScene()->destroyEntity(pEntity);
        return 0;
    }

    int LuaBindings::funcGetSpriteTexture(lua_State* L)
    {
        auto pSprite = LUA_GET_COMPONENT(1, SpriteComponent);
        std::string ret = "";
        if (pSprite)
            if (pSprite->pTexture)
                ret = pSprite->pTexture->getFilename();
        lua_pushstring(L, ret.c_str());
        return 1;
    }

    int LuaBindings::funcSetSpriteTexture(lua_State* L)
    {
        auto pSprite = LUA_GET_COMPONENT(1, SpriteComponent);
        if (pSprite) pSprite->pTexture = getResourceManager()->getTexture(LUA_GET_STRING(2, ""));
        return 0;
    }

    int LuaBindings::funcGetSpriteColor(lua_State* L)
    {
        auto pSprite = LUA_GET_COMPONENT(1, SpriteComponent);
        glm::vec4 color(1);
        if (pSprite) color = pSprite->color;
        LUA_PUSH_COLOR(color);
        return 1;
    }

    int LuaBindings::funcSetSpriteColor(lua_State* L)
    {
        auto pSprite = LUA_GET_COMPONENT(1, SpriteComponent);
        if (pSprite) pSprite->color = LUA_GET_COLOR(2, glm::vec4(1, 1, 1, 1));
        return 0;
    }

    int LuaBindings::funcGetSpriteOrigin(lua_State* L)
    {
        auto pSprite = LUA_GET_COMPONENT(1, SpriteComponent);
        glm::vec2 origin(0.5f);
        if (pSprite) origin = pSprite->origin;
        LUA_PUSH_VEC2(origin);
        return 1;
    }

    int LuaBindings::funcSetSpriteOrigin(lua_State* L)
    {
        auto pSprite = LUA_GET_COMPONENT(1, SpriteComponent);
        if (pSprite) pSprite->origin = LUA_GET_VEC2(2, glm::vec2(0.5f));
        return 0;
    }

    int LuaBindings::funcGetFont(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        std::string ret = "";
        if (pText)
            if (pText->pFont)
                ret = pText->pFont->getFilename();
        lua_pushstring(L, ret.c_str());
        return 1;
    }

    int LuaBindings::funcSetFont(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        if (pText) pText->pFont = getResourceManager()->getFont(LUA_GET_STRING(2, ""));
        return 0;
    }

    int LuaBindings::funcGetText(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        std::string ret;
        if (pText) ret = pText->text;
        lua_pushstring(L, ret.c_str());
        return 1;
    }

    int LuaBindings::funcSetText(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        if (pText) pText->text = LUA_GET_STRING(2, "");
        return 0;
    }

    int LuaBindings::funcGetTextColor(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        glm::vec4 color(1);
        if (pText) color = pText->color;
        LUA_PUSH_COLOR(color);
        return 1;
    }

    int LuaBindings::funcSetTextColor(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        if (pText) pText->color = LUA_GET_COLOR(2, glm::vec4(1, 1, 1, 1));
        return 0;
    }

    int LuaBindings::funcGetTextOrigin(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        glm::vec2 origin(0.5f);
        if (pText) origin = pText->origin;
        LUA_PUSH_VEC2(origin);
        return 1;
    }

    int LuaBindings::funcSetTextOrigin(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        if (pText) pText->origin = LUA_GET_VEC2(2, glm::vec2(0.5f));
        return 0;
    }

    int LuaBindings::funcGetTextScale(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        float ret = 1.0f;
        if (pText) ret = pText->scale;
        lua_pushnumber(L, (lua_Number)ret);
        return 1;
    }

    int LuaBindings::funcSetTextScale(lua_State* L)
    {
        auto pText = LUA_GET_COMPONENT(1, TextComponent);
        if (pText) pText->scale = LUA_GET_NUMBER(2, 1.0f);
        return 0;
    }
    
    int LuaBindings::funcGetPFX(lua_State* L)
    {
        auto pPFXComponent = LUA_GET_COMPONENT(1, PFXComponent);
        std::string ret = "";
        if (pPFXComponent)
            if (pPFXComponent->pPFX)
                ret = pPFXComponent->pPFX->getFilename();
        lua_pushstring(L, ret.c_str());
        return 1;
    }

    int LuaBindings::funcSetPFX(lua_State* L)
    {
        auto pPFXComponent = LUA_GET_COMPONENT(1, PFXComponent);
        if (pPFXComponent) pPFXComponent->pPFX = getResourceManager()->getPFX(LUA_GET_STRING(2, ""));
        return 0;
    }

    int LuaBindings::funcPlayPFX(lua_State* L)
    {
        auto pPFXComponent = LUA_GET_COMPONENT(1, PFXComponent);
        if (pPFXComponent) pPFXComponent->play();
        return 0;
    }

    int LuaBindings::funcStopPFX(lua_State* L)
    {
        auto pPFXComponent = LUA_GET_COMPONENT(1, PFXComponent);
        if (pPFXComponent) pPFXComponent->stop();
        return 0;
    }

    int LuaBindings::funcGetName(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        std::string ret = "";
        if (pEntity) ret = pEntity->name.c_str();
        lua_pushstring(L, ret.c_str());
        return 1;
    }

    int LuaBindings::funcSetName(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (pEntity) pEntity->name = LUA_GET_STRING(2, "");
        return 0;
    }

    int LuaBindings::funcFindEntityByName(lua_State* L)
    {
        auto searchName = LUA_GET_STRING(1, "");
        if (searchName.empty())
        {
            lua_pushnil(L);
            return 1;
        }
        glm::vec2 searchPos = LUA_GET_VEC2(2, glm::vec2(0));
        auto searchRadius = LUA_GET_NUMBER(3, 0.0f);

        if (searchRadius == 0.0f)
        {
            auto pEntity = getScene()->getEntityByName(searchName, true);
            LUA_PUSH_ENTITY(pEntity);
            return 1;
        }
        else
        {
            EntitySearchParams searchParams;
            searchParams.pointInWorld = searchPos;
            searchParams.radius = searchRadius;
            auto pEntity = getScene()->getEntityByName(searchName, searchParams, true);
            LUA_PUSH_ENTITY(pEntity);
            return 1;
        }
    }

    int LuaBindings::funcFindEntityByComponent(lua_State* L)
    {
        //TODO:
        return 0;
    }

    int LuaBindings::funcContinueGame(lua_State* L)
    {
        m_stateChangeRequest = StateChangeRequest::ContinueGame;
        return 0;
    }

    int LuaBindings::funcNewGame(lua_State* L)
    {
        m_stateChangeRequest = StateChangeRequest::NewGame;
        m_worldFilenameToLoad = LUA_GET_STRING(1, "assets/scenes/world.json");
        return 0;
    }

    int LuaBindings::funcQuit(lua_State* L)
    {
        m_stateChangeRequest = StateChangeRequest::Quit;
        return 0;
    }

    int LuaBindings::funcEditor(lua_State* L)
    {
        m_stateChangeRequest = StateChangeRequest::Editor;
        return 0;
    }

    int LuaBindings::funcResume(lua_State* L)
    {
        m_stateChangeRequest = StateChangeRequest::Resume;
        return 0;
    }

    int LuaBindings::funcMainMenu(lua_State* L)
    {
        m_stateChangeRequest = StateChangeRequest::MainMenu;
        return 0;
    }

    int LuaBindings::funcPlayMusic(lua_State* L)
    {
        getMusicManager()->play(LUA_GET_STRING(1, ""));
        return 0;
    }

    int LuaBindings::funcStopMusic(lua_State* L)
    {
        getMusicManager()->stop();
        return 0;
    }

    int LuaBindings::funcPauseMusic(lua_State* L)
    {
        getMusicManager()->pause();
        return 0;
    }

    int LuaBindings::funcResumeMusic(lua_State* L)
    {
        getMusicManager()->resume();
        return 0;
    }

    int LuaBindings::funcGetRoot(lua_State* L)
    {
        auto pRoot = getScene()->getRoot();
        LUA_PUSH_ENTITY(pRoot);
        return 1;
    }

    int LuaBindings::funcCreateEntity(lua_State* L)
    {
        auto pParent = LUA_GET_ENTITY(1);
        auto prefab = LUA_GET_STRING(2, "");

        if (!pParent)
        {
            CORE_ERROR_POPUP("Lua: Expected first argument to be entity in CreateEntity(parent)");
            lua_pushnil(L);
            return 1;
        }

        auto pEntity = getScene()->createEntity(pParent);

        if (!prefab.empty())
        {
            static std::unordered_map<std::string, Json::Value> prefabCache;
            auto it = prefabCache.find(prefab);
            if (it == prefabCache.end())
            {
                Json::Value json;
                if (Utils::loadJson(json, "assets/" + prefab))
                {
                    prefabCache[prefab] = json;
                    it = prefabCache.find(prefab);
                }
            }

            if (it != prefabCache.end())
            {
                const auto& json = it->second;
                pEntity->deserialize(json["root"]);
            }
        }

        LUA_PUSH_ENTITY(pEntity);
        return 1;
    }

    int LuaBindings::funcAddComponent(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (!pEntity)
        {
            CORE_ERROR_POPUP("Lua: AddComponent, invalid entity");
            lua_pushnil(L);
            return 1;
        }

        auto componentName = LUA_GET_STRING(2, "");
        if (componentName.empty())
        {
            CORE_ERROR_POPUP("Lua: AddComponent, invalid name: empty");
            lua_pushnil(L);
            return 1;
        }

        auto pComponent = ComponentFactory::create(componentName);
        if (pComponent)
        {
            if (std::dynamic_pointer_cast<ScriptComponent>(pComponent))
                CORE_ERROR_POPUP("Lua: AddComponent, Cannot create \"Script\" component. Use the actual component name.");
            
            pEntity->addComponent(pComponent);
            lua_pushnil(L);
            return 1; // We added a built-in component, we're all good. We don't return built-ins
        }

        auto pDef = getComponentDef(componentName);
        if (!pDef)
        {
            CORE_ERROR_POPUP("Lua: AddComponent, invalid name: {}", componentName);
            lua_pushnil(L);
            return 1;
        }

        auto pScriptComponent = pEntity->addComponent<Engine::ScriptComponent>();
        pScriptComponent->loadDef(pDef);
        
        lua_getglobal(L, "CINS_t");
        lua_getfield(L, -1, pScriptComponent->luaName.c_str());
        return 1;
    }

    int LuaBindings::funcRemoveComponent(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (!pEntity) return 0;

        auto componentName = LUA_GET_STRING(2, "");
        for (const auto& pComponent : pEntity->getComponents())
        {
            if (pComponent->getType() == "Script")
            {
                auto pScriptComponent = std::dynamic_pointer_cast<ScriptComponent>(pComponent);
                if (pScriptComponent->name == componentName)
                {
                    pEntity->removeComponent(pComponent);
                    return 0;
                }
            }
            if (pComponent->getType() == componentName)
            {
                pEntity->removeComponent(pComponent);
                return 0;
            }
        }

        return 0;
    }

    int LuaBindings::funcGetParent(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (!pEntity)
        {
            lua_pushnil(L);
            return 1;
        }

        LUA_PUSH_ENTITY(pEntity->getParent());
        return 1;
    }

    int LuaBindings::funcAddChild(lua_State* L)
    {
        auto pParent = LUA_GET_ENTITY(1);
        if (!pParent)
        {
            CORE_WARN("Lua: AddChild, invalid parent");
            return 0;
        }

        auto pChild = LUA_GET_ENTITY(2);
        if (!pChild)
        {
            CORE_WARN("Lua: AddChild, invalid child");
            return 0;
        }

        pParent->addChild(pChild);
        return 0;
    }

    int LuaBindings::funcLog(lua_State* L)
    {
        auto text = LUA_GET_STRING(1, "");
        CORE_INFO(text);
        return 0;
    }

    int LuaBindings::funcGetScreenRect(lua_State* L)
    {
        auto screenRect = getScene()->getScreenRect();
        lua_pushnumber(L, (lua_Number)screenRect.x);
        lua_pushnumber(L, (lua_Number)screenRect.y);
        lua_pushnumber(L, (lua_Number)screenRect.z);
        lua_pushnumber(L, (lua_Number)screenRect.w);
        return 4;
    }

    int LuaBindings::funcRegisterEvent(lua_State* L)
    {
        if (lua_gettop(L) < 3 || !lua_isstring(L, 1) || !lua_isstring(L, 3))
        {
            CORE_ERROR_POPUP("Lua: RegisterEvent expected (string, c, string)");
            return 0;
        }

        auto eventName = LUA_GET_STRING(1, "");
        if (eventName.empty()) return 0;

        auto rpScript = LUA_GET_SCRIPT_COMPONENT(2);
        if (!rpScript) return 0;

        auto callbackName = LUA_GET_STRING(3, "");
        if (callbackName.empty()) return 0;

        // Validate that our object have this callback
        {
            lua_getglobal(L, "CINS_t");
            lua_getfield(L, -1, rpScript->luaName.c_str());
            lua_getfield(L, -1, callbackName.c_str());
            if (!lua_isfunction(L, -1)) 
            {
                CORE_ERROR_POPUP("Lua: RegisterEvent, callback function \"{}\" not found in component.", callbackName);
                return 0;
            }
            lua_pop(L, 3);
        }

        // Register event
        {
            if (eventName == "KeyDown")
                getEventSystem()->registerListener<KeyDownEvent>(rpScript, std::bind(&LuaBindings::onKeyDown, this, _1));
            else if (eventName == "KeyUp")
                getEventSystem()->registerListener<KeyUpEvent>(rpScript, std::bind(&LuaBindings::onKeyUp, this, _1));
            else if (eventName == "MouseDown")
                getEventSystem()->registerListener<MouseButtonDownEvent>(rpScript, std::bind(&LuaBindings::onMouseDown, this, _1));
            else if (eventName == "MouseUp")
                getEventSystem()->registerListener<MouseButtonUpEvent>(rpScript, std::bind(&LuaBindings::onMouseUp, this, _1));
            else // Lua event
                getEventSystem()->registerLuaListener(eventName, rpScript, std::bind(&LuaBindings::onLuaEvent, this, _1));
        }


        m_scriptEventListeners[eventName][(uintptr_t)rpScript] = { rpScript->shared_from_this(), callbackName };

        return 0;
    }

    int LuaBindings::funcDeregisterEvent(lua_State* L)
    {
        if (lua_gettop(L) < 2 || !lua_isstring(L, 1))
        {
            CORE_ERROR_POPUP("Lua: RegisterEvent expected (string, c)");
            return 0;
        }

        auto eventName = LUA_GET_STRING(1, "");
        if (eventName.empty()) return 0;

        auto rpScript = LUA_GET_SCRIPT_COMPONENT(2);
        if (!rpScript) return 0;


        {
            if (eventName == "KeyDown")
                getEventSystem()->deregisterListener<KeyDownEvent>(rpScript);
            else if (eventName == "KeyUp")
                getEventSystem()->deregisterListener<KeyUpEvent>(rpScript);
            else if (eventName == "MouseDown")
                getEventSystem()->deregisterListener<MouseButtonDownEvent>(rpScript);
            else if (eventName == "MouseUp")
                getEventSystem()->deregisterListener<MouseButtonUpEvent>(rpScript);
            else // Lua event
                getEventSystem()->deregisterLuaListener(eventName, rpScript);
        }


        auto itm = m_scriptEventListeners.find(eventName);
        if (itm == m_scriptEventListeners.end()) return 0;

        // Find our event listener
        auto it = itm->second.find((uintptr_t)rpScript);
        if (it == itm->second.end()) return 0;

        // Erase
        itm->second.erase(it);

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

    int LuaBindings::funcEmitParticles(lua_State* L)
    {
        std::string pfxName = LUA_GET_STRING(1, "particles/defaultPFX.json");
        glm::vec2 position = LUA_GET_VEC2(2, glm::vec2(0, 0));
        float rotation = LUA_GET_NUMBER(3, 0.0f);
        glm::vec2 scale = glm::vec2(LUA_GET_NUMBER(4, 1.0f));

        auto pEntity = getScene()->createEntity();
        getScene()->getRoot()->addChild(pEntity);

        pEntity->setWorldPosition(position);
        pEntity->setRotation(rotation);
        pEntity->setScale(scale);

        auto pPFX = pEntity->addComponent<PFXComponent>();
        pPFX->pPFX = getResourceManager()->getPFX(pfxName);
        pPFX->play();
        pPFX->destroyWhenDone = true;

        LUA_PUSH_ENTITY(pEntity);
        return 1;
    }

    int LuaBindings::funcEnableEntity(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (!pEntity) return 0;
        pEntity->enable();
        return 0;
    }

    int LuaBindings::funcDisableEntity(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (!pEntity) return 0;
        pEntity->disable();
        return 0;
    }

    int LuaBindings::funcEnableComponent(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (!pEntity) return 0;

        auto componentName = LUA_GET_STRING(2, "");

        for (const auto& pComponent : pEntity->getComponents())
        {
            if (componentName == pComponent->getType())
            {
                pComponent->enable();
                break;
            }
            auto pScriptComponent = std::dynamic_pointer_cast<ScriptComponent>(pComponent);
            if (pScriptComponent)
            {
                if (pScriptComponent->name == componentName)
                {
                    pComponent->enable();
                    return 0;
                }
            }
        }

        return 0;
    }

    int LuaBindings::funcDisableComponent(lua_State* L)
    {
        auto pEntity = LUA_GET_ENTITY(1);
        if (!pEntity) return 0;

        auto componentName = LUA_GET_STRING(2, "");

        for (const auto& pComponent : pEntity->getComponents())
        {
            if (componentName == pComponent->getType())
            {
                pComponent->disable();
                break;
            }
            auto pScriptComponent = std::dynamic_pointer_cast<ScriptComponent>(pComponent);
            if (pScriptComponent)
            {
                if (pScriptComponent->name == componentName)
                {
                    pComponent->disable();
                    return 0;
                }
            }
        }

        return 0;
    }
}
