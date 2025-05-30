#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    class Shader : public RefCounted
    {
    public:
        static Ref<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath);

        virtual ~Shader() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;
    };
}