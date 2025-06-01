#include <pch.h>

#include <flux/renderer/VertexBuffer.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanVertexBuffer.h>

namespace flux
{
    uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Bool:      return 1;

        case ShaderDataType::Float:     return 4;
        case ShaderDataType::Float2:    return 4 * 2;
        case ShaderDataType::Float3:    return 4 * 3;
        case ShaderDataType::Float4:    return 4 * 4;

        case ShaderDataType::Int:       return 4;
        case ShaderDataType::Int2:      return 4 * 2;
        case ShaderDataType::Int3:      return 4 * 3;
        case ShaderDataType::Int4:      return 4 * 4;

        case ShaderDataType::Mat3:      return 4 * 3 * 3;
        case ShaderDataType::Mat4:      return 4 * 4 * 4;
        }

        DBG_ASSERT(false, "Unknown shader data type!");
        return 0;
    }

    VertexBufferElement::VertexBufferElement(const std::string& name, ShaderDataType type, bool normalized)
        : name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized)
    {
    }

    uint32_t VertexBufferElement::GetComponentCount() const
    {
        switch (type)
        {
        case ShaderDataType::Bool:      return 1;

        case ShaderDataType::Float:     return 1;
        case ShaderDataType::Float2:    return 2;
        case ShaderDataType::Float3:    return 3;
        case ShaderDataType::Float4:    return 4;

        case ShaderDataType::Int:       return 1;
        case ShaderDataType::Int2:      return 2;
        case ShaderDataType::Int3:      return 3;
        case ShaderDataType::Int4:      return 4;

        case ShaderDataType::Mat3:      return 3;
        case ShaderDataType::Mat4:      return 4;
        }

        DBG_ASSERT(false, "Unknown shader data type!");
        return 0;
    }

    VertexBufferLayout::VertexBufferLayout(std::initializer_list<VertexBufferElement> elements)
        : elements_(elements)
    {
        CalculateOffsetStride();
    }

    void VertexBufferLayout::CalculateOffsetStride()
    {
        size_t offset = 0;
        stride_ = 0;

        for (auto& element : elements_)
        {
            element.offset = offset;
            offset += element.size;
            stride_ += element.size;
        }
    }

    Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, uint64_t size, Usage usage)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanVertexBuffer>(vertices, size, usage);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(uint64_t size, Usage usage)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanVertexBuffer>(size, usage);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "unrecognized renderer API");
        return nullptr;
    }
}