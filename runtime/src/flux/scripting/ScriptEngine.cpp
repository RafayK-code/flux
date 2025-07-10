#include <fxpch.h>

#include <flux/scripting/ScriptEngine.h>

namespace flux
{
    ScriptEngine::~ScriptEngine()
    {
        Shutdown();
    }

    bool ScriptEngine::Init()
    {
        mono_set_dirs("C:\\Program Files\\Mono\\lib", "C:\\Program Files\\Mono\\etc");
        domain_ = mono_jit_init(ENGINE_NAME);
        return domain_ != nullptr;
    }

    void ScriptEngine::Shutdown()
    {
        if (domain_)
        {
            mono_jit_cleanup(domain_);
            domain_ = nullptr;
        }
    }

    bool ScriptEngine::LoadCoreAssembly(const std::filesystem::path& path)
    {
        return coreModule_.Load(domain_, path);
    }

    bool ScriptEngine::LoadAppAssembly(const std::filesystem::path& path)
    {
        return appModule_.Load(domain_, path);
    }

    Ref<ScriptObject> ScriptEngine::Instantiate(const ScriptModule& module, const std::string& namespaceName, const std::string& className)
    {
        return module.Instantiate(namespaceName, className);
    }

    Ref<ScriptObject> ScriptEngine::InstantiateFromCore(const std::string& namespaceName, const std::string& className)
    {
        return coreModule_.Instantiate(namespaceName, className);
    }

    Ref<ScriptObject> ScriptEngine::InstantiateFromApp(const std::string& namespaceName, const std::string& className)
    {
        return appModule_.Instantiate(namespaceName, className);
    }
}