#pragma once

#include <flux/core/base.h>

#include <flux/scripting/ScriptModule.h>

namespace flux
{
    class FLUX_API ScriptEngine
    {
    public:
        ScriptEngine() = default;
        ~ScriptEngine();

        bool Init();
        void Shutdown();

        bool LoadCoreAssembly(const std::filesystem::path& path);
        bool LoadAppAssembly(const std::filesystem::path& path);

        inline ScriptModule& CoreModule() { return coreModule_; }
        inline const ScriptModule& CoreModule() const { return coreModule_; }

        inline ScriptModule& AppModule() { return appModule_; }
        inline const ScriptModule& AppModule() const { return appModule_; }

        Ref<ScriptObject> Instantiate(const ScriptModule& module, const std::string& namespaceName, const std::string& className);

        Ref<ScriptObject> InstantiateFromCore(const std::string& namespaceName, const std::string& className);
        Ref<ScriptObject> InstantiateFromApp(const std::string& namespaceName, const std::string& className);

    private:
        MonoDomain* domain_ = nullptr;

        ScriptModule coreModule_;
        ScriptModule appModule_;
    };
}