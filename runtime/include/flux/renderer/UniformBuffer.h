#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

namespace flux
{
    class FLUX_API UniformBuffer : public RefCounted
    {
    public:
        static Ref<UniformBuffer> Create(uint32_t size);

        virtual ~UniformBuffer() = default;

        virtual void Bind(uint32_t binding, uint32_t set = 0) const = 0;
        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
    };
}