#pragma once

#include <flux/core/base.h>

#include <string>
#include <string_view>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace flux
{
    namespace script
    {
        namespace internal
        {
            inline MonoDomain* GetDomain()
            {
                return mono_domain_get(); // or cache it yourself
            }

            inline void* BoxValue(bool value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "Boolean");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(char value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "Char");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(int8_t value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "SByte");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(uint8_t value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "Byte");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(int16_t value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "Int16");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(uint16_t value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "UInt16");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(int32_t value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "Int32");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(uint32_t value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "UInt32");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(int64_t value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "Int64");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(uint64_t value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "UInt64");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(float value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "Single");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(double value)
            {
                MonoClass* cls = mono_class_from_name(mono_get_corlib(), "System", "Double");
                return mono_value_box(GetDomain(), cls, &value);
            }

            inline void* BoxValue(const std::string& value)
            {
                return mono_string_new(GetDomain(), value.c_str());
            }

            inline void* BoxValue(const char* value)
            {
                return mono_string_new(GetDomain(), value);
            }

            inline void* BoxValue(MonoObject* obj)
            {
                return obj;
            }

            template<typename T>
            inline void* PrepareArg(T& value)
            {
                static_assert(std::is_arithmetic_v<T>, "Use specific overloads for non-arithmetic types");
                return &value;
            }

            inline void* PrepareArg(const std::string& value)
            {
                return mono_string_new(GetDomain(), value.c_str());
            }

            inline void* PrepareArg(const char* value)
            {
                return mono_string_new(GetDomain(), value);
            }

            inline void* PrepareArg(MonoObject* obj)
            {
                return obj;
            }
        }
    }

    class FLUX_API ScriptObject : public RefCounted
    {
    public:
        ScriptObject() : domainHandle_(script::internal::GetDomain()) {}
        ~ScriptObject() = default;

        bool Init(MonoAssembly* assembly, MonoImage* image, const std::string& namespaceName, const std::string& className);

        template<typename... Args>
        void InvokeMethod(std::string_view methodName, Args&&... args)
        {
            MonoMethod* method = mono_class_get_method_from_name(monoClass_, methodName.data(), sizeof...(Args));
            if (!method)
                return;

            if constexpr (sizeof...(Args) > 0)
            {
                void* argsArray[sizeof...(Args)] = { script::internal::PrepareArg(args)... };
                mono_runtime_invoke(method, instance_, argsArray, nullptr);
            }
            else
            {
                mono_runtime_invoke(method, instance_, nullptr, nullptr);
            }
        }

        inline MonoClass* NativeMonoClass() { return monoClass_; }
        inline MonoObject* NativeMonoObject() { return instance_; }

    private:
        MonoDomain* domainHandle_;
        MonoAssembly* assemblyHandle_ = nullptr;
        MonoImage* imageHandle_ = nullptr;

        MonoClass* monoClass_ = nullptr;
        MonoObject* instance_ = nullptr;
    };
}