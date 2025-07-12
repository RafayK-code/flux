#include <fxpch.h>

#include <flux/scripting/ScriptObject.h>

namespace flux
{
    bool ScriptObject::Init(MonoAssembly* assembly, MonoImage* image, const std::string& namespaceName, const std::string& className)
    {
        assemblyHandle_ = assembly;
        imageHandle_ = image;
        monoClass_ = mono_class_from_name(imageHandle_, namespaceName.c_str(), className.c_str());
        if (!monoClass_)
            return false;

        instance_ = mono_object_new(script::internal::GetDomain(), monoClass_);
        if (!instance_)
            return false;

        mono_runtime_object_init(instance_);
        return true;
    }
}