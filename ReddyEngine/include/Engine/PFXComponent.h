#pragma once

#include <Engine/Component.h>


namespace Engine
{
	class Texture;
	using TextureRef = std::shared_ptr<Texture>;

	class PFX;
	using PFXRef = std::shared_ptr<PFX>;

	class PFXInstance;
	using PFXInstanceRef = std::shared_ptr<PFXInstance>;


    class PFXComponent final : public Component
    {
    public:
		PFXComponent();
		~PFXComponent() {}

		Json::Value serialize() override;
		void deserialize(const Json::Value& json) override;
		bool edit() override;
		void draw() override;
		void update(float dt) override;
		void onCreate() override;

		void play();
		void stop();

		std::string getType() const override { return "PFX"; }

		PFXRef pPFX;
		PFXInstanceRef pPFXInstance;
		bool triggerOnStart = true;
		bool destroyWhenDone = false;
    };
}
