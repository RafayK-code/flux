#include <fxpch.h>

#include <flux/scripting/ScriptModule.h>

namespace flux
{
    bool ScriptModule::Load(MonoDomain* domain, const std::filesystem::path& assemblyPath)
    {
        assembly_ = mono_domain_assembly_open(domain, assemblyPath.string().c_str());
        if (!assembly_)
            return false;

        image_ = mono_assembly_get_image(assembly_);
        return image_ != nullptr;
    }

    Ref<ScriptObject> ScriptModule::Instantiate(const std::string& namespaceName, const std::string& className) const
    {
        Ref<ScriptObject> script = CreateRef<ScriptObject>();
        if (script->Init(assembly_, image_, namespaceName, className))
            return script;

        return nullptr;
    }
}