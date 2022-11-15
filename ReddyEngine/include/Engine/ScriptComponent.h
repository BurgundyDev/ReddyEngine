#pragma once

#include <Engine/Component.h>


namespace Engine
{
	class Texture;
	using TextureRef = std::shared_ptr<Texture>;

	struct LuaComponentDef;
	struct LuaProperty;


    class ScriptComponent final : public Component, public std::enable_shared_from_this<ScriptComponent>
    {
    public:
		ScriptComponent();
		~ScriptComponent();

		Json::Value serialize() override;
		void deserialize(const Json::Value& json) override;
		bool edit() override;
		std::string getType() const override { return "Script"; }
		std::string getEditName() const override { return name.empty() ? "Script" : "Script - " + name; }
		LuaComponentDef* getLuaDef() const { return m_pLuaComponentDef; }

		void loadDef(LuaComponentDef* pDef);


		void onCreate() override;
		void onDestroy() override;
		void onEnable() override;
		void onDisable() override;
		void fixedUpdate(float dt) override;
		void update(float dt) override;
		void onMouseEnter() override;
		void onMouseLeave() override;
		void onMouseDown() override;
		void onMouseUp() override;
		void onMouseClick() override;


		std::string name;
		uint64_t runtimeId = 0;
		std::string luaName;

	private:
		void createLuaObj();
		void destroyLuaObj();

		LuaComponentDef* m_pLuaComponentDef = nullptr;
		std::vector<Engine::LuaProperty> m_luaProperties; // This is for lua mostly

		uint16_t m_implLuaCallsMask = 0;
    };
}
