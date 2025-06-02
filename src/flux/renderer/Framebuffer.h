#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    class Framebuffer : public RefCounted
    {
    public:
        static Ref<Framebuffer> Create();

        virtual ~Framebuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
    };
}