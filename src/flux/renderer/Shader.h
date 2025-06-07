#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/renderer/UniformBuffer.h>
#include <flux/renderer/Image.h>

#include <vector>

namespace flux
{
    enum class UniformType
    {
        UniformBuffer,
        Sampler,
    };

    namespace ShaderStage
    {
        enum : uint8_t
        {
            Vertex = 1,
            Fragment = 2,
        };
    }

    using ShaderStageFlags = uint8_t;

    class ShaderUniformElement
    {
    public:
        ShaderUniformElement() = default;
        ShaderUniformElement(const std::string name, uint32_t binding, UniformType type, ShaderStageFlags shaderStage);

        ~ShaderUniformElement() = default;

        std::string name;
        uint32_t binding;
        uint32_t set = 0;
        UniformType type;
        ShaderStageFlags shaderStage;
    };

    class ShaderUniformLayout
    {
    public:
        ShaderUniformLayout(std::initializer_list<ShaderUniformElement> elements) : elements_(elements) {}
        ~ShaderUniformLayout() = default;

        inline const std::vector<ShaderUniformElement>& Elements() const { return elements_; }

        inline std::vector<ShaderUniformElement>::iterator Begin() { return elements_.begin(); }
        inline std::vector<ShaderUniformElement>::iterator End() { return elements_.end(); }
        inline std::vector<ShaderUniformElement>::const_iterator Begin() const { return elements_.begin(); }
        inline std::vector<ShaderUniformElement>::const_iterator End() const { return elements_.end(); }

    private:
        std::vector<ShaderUniformElement> elements_;
    };

    class Shader : public RefCounted
    {
    public:
        static Ref<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath, const ShaderUniformLayout& layout);

        virtual ~Shader() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding) const = 0;
        virtual void PushUniformBuffer(const Ref<UniformBuffer>& ub, uint32_t binding, uint32_t index) const = 0;

        virtual void PushSampler(const Ref<Image>& image, uint32_t binding) const = 0;
        virtual void PushSampler(const Ref<Image>& image, uint32_t binding, uint32_t index) const = 0;
    };
}