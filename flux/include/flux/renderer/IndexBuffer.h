#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    class FLUX_API IndexBuffer : public RefCounted
    {
    public:
        static Ref<IndexBuffer> Create(const uint32_t* data, uint32_t count);

        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t Count() const = 0;
    };
}