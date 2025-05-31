#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <string>

namespace flux
{
    enum class ShaderDataType
    {
        None = 0,
        Bool,
        Float, Float2, Float3, Float4,
        Int, Int2, Int3, Int4,
        Mat3, Mat4
    };

    uint32_t ShaderDataTypeSize(ShaderDataType type);

    class VertexBufferElement
    {
    public:
        VertexBufferElement() = default;
        VertexBufferElement(const std::string& name, ShaderDataType type, bool normalized = false);

        ~VertexBufferElement() = default;

        uint32_t GetComponentCount() const;

        std::string name;
        ShaderDataType type;
        uint32_t size;
        size_t offset;
        bool normalized;
    };

    class VertexBufferLayout
    {
    public:
        VertexBufferLayout(std::initializer_list<VertexBufferElement> elements);
        ~VertexBufferLayout() = default;

        inline uint32_t Stride() const { return stride_; }
        inline const std::vector<VertexBufferElement>& Elements() const { return elements_; }

        inline std::vector<VertexBufferElement>::iterator Begin() { return elements_.begin(); }
        inline std::vector<VertexBufferElement>::iterator End() { return elements_.end(); }
        inline std::vector<VertexBufferElement>::const_iterator Begin() const { return elements_.begin(); }
        inline std::vector<VertexBufferElement>::const_iterator End() const { return elements_.end(); }

    private:
        void CalculateOffsetStride();

    private:
        std::vector<VertexBufferElement> elements_;
        uint32_t stride_ = 0;
    };

    class VertexBuffer : public RefCounted
    {
    public:
        enum class Usage
        {
            Static,
            Dynamic,
        };

        static Ref<VertexBuffer> Create(void* vertices, uint64_t size, Usage usage = Usage::Static);
        static Ref<VertexBuffer> Create(uint64_t size, Usage usage = Usage::Dynamic);

        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void* data, uint64_t size, uint64_t offset = 0) = 0;
        virtual uint64_t Size() const = 0;
    };
}