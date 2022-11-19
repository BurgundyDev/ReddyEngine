#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>


namespace Engine
{
    class Component;
    using ComponentRef = std::shared_ptr<Component>;

    using CreateComponentFn = ComponentRef(*)();


    class ComponentFactory
    {
    public:
        static void initialize();
        static ComponentRef create(const std::string& name);
        static const std::vector<std::string>& getComponentNames();

    private:
        static bool registerFactory(const std::string& name, CreateComponentFn fn);
        template<typename T>
        static void registerComponent()
        {
            registerFactory(T::getRegisterName(), []() -> std::shared_ptr<Component> { return std::make_shared<T>(); });
        }

        static std::map<std::string, CreateComponentFn> s_factories;
        static std::vector<std::string> s_componentNames;
    };
}

#define DECLARE_COMPONENT(name) \
public: \
    static std::string getRegisterName() { return name; } \
	std::string getType() const override { return name; } \
private:
