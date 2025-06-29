#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/scripting/ScriptObject.h>

#include <filesystem>

namespace flux
{
    class FLUX_API ScriptModule
    {
    public:
        ScriptModule() = default;
        ~ScriptModule() = default;

        bool Load(MonoDomain* domain, const std::filesystem::path& assemblyPath);

        Ref<ScriptObject> Instantiate(const std::string& namespaceName, const std::string& className) const;

    private:
        MonoAssembly* assembly_ = nullptr;
        MonoImage* image_ = nullptr;
    };
}