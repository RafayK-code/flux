#pragma once

#include <flux/renderer/VertexBuffer.h>
#include <flux/renderer/IndexBuffer.h>

namespace flux
{
    class FLUX_API VertexArray : public RefCounted
    {
    public:
        static Ref<VertexArray> Create();
        static Ref<VertexArray> Create(const Ref<VertexBuffer>& vb, const Ref<IndexBuffer>& ib);
        
        virtual ~VertexArray() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetVertexBuffer(const Ref<VertexBuffer>& vb) = 0;
        virtual const Ref<VertexBuffer>& GetVertexBuffer() const = 0;

        virtual void SetIndexBuffer(const Ref<IndexBuffer>& ib) = 0;
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        virtual void SetLayout(const VertexBufferLayout& layout) = 0;
        virtual const VertexBufferLayout& Layout() const = 0;
    };
}