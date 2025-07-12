#pragma once

#include <flux/renderer/UniformBuffer.h>
#include <flux/renderer/Shader.h>

namespace flux
{
    class FLUX_API ShaderInputSet : public RefCounted
    {
    public:
        static Ref<ShaderInputSet> Create(const Ref<Shader>& shader, uint32_t set = 0);

        virtual ~ShaderInputSet() = default;

        virtual uint32_t SetIndex() const = 0;

        virtual void PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding) const = 0;
        virtual void PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding, uint32_t frameInFlight) const = 0;

        virtual void PushSampler(const Ref<Image>& image, uint32_t binding) const = 0;
        virtual void PushSampler(const Ref<Image>& image, uint32_t binding, uint32_t frameInFlight) const = 0;
    };
}