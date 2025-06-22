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

    class FLUX_API ShaderUniformElement
    {
    public:
        ShaderUniformElement() = default;
        ShaderUniformElement(const std::string name, uint32_t binding, UniformType type, ShaderStageFlags shaderStage);

        ~ShaderUniformElement() = default;

        std::string name;
        uint32_t binding;
        UniformType type;
        ShaderStageFlags shaderStage;
    };

    class FLUX_API ShaderUniformSetLayout
    {
    public:
        ShaderUniformSetLayout(uint32_t set, std::initializer_list<ShaderUniformElement> elements) : elements_(elements) {}
        ~ShaderUniformSetLayout() = default;

        inline const std::vector<ShaderUniformElement>& Elements() const { return elements_; }

        inline std::vector<ShaderUniformElement>::iterator begin() { return elements_.begin(); }
        inline std::vector<ShaderUniformElement>::iterator end() { return elements_.end(); }
        inline std::vector<ShaderUniformElement>::const_iterator begin() const { return elements_.begin(); }
        inline std::vector<ShaderUniformElement>::const_iterator end() const { return elements_.end(); }

    private:
        std::vector<ShaderUniformElement> elements_;
    };

    class FLUX_API ShaderUniformLayout
    {
    public:
        ShaderUniformLayout(std::initializer_list<ShaderUniformSetLayout> setLayouts) : setLayouts_(setLayouts) {}
        ~ShaderUniformLayout() = default;

        inline const std::vector<ShaderUniformSetLayout>& SetLayouts() const { return setLayouts_; }

        inline std::vector<ShaderUniformSetLayout>::iterator begin() { return setLayouts_.begin(); }
        inline std::vector<ShaderUniformSetLayout>::iterator end() { return setLayouts_.end(); }
        inline std::vector<ShaderUniformSetLayout>::const_iterator begin() const { return setLayouts_.begin(); }
        inline std::vector<ShaderUniformSetLayout>::const_iterator end() const { return setLayouts_.end(); }

    private:
        std::vector<ShaderUniformSetLayout> setLayouts_;
    };

    class FLUX_API Shader : public RefCounted
    {
    public:
        static Ref<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath, const ShaderUniformLayout& layout);

        virtual ~Shader() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;
    };
}