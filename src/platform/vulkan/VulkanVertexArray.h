#pragma once

#include <flux/renderer/VertexArray.h>

namespace flux
{
    /**
    * One of the few classes that is actually needed in OpenGL and not in Vulkan
    * 
    * Vulkan doesnt really have any concepts of vertex arrays. The layouts and attributes
    * are baked into the pipeline. For this reason, this class will simply behave as a container
    * for a vertex buffer and vertex array and nothing more
    */
    class VulkanVertexArray : public VertexArray
    {
    public:
        VulkanVertexArray() = default;
        VulkanVertexArray(const Ref<VertexBuffer>& vb, const Ref<IndexBuffer>& ib) : vb_(vb), ib_(ib) {}

        virtual ~VulkanVertexArray() = default;

        virtual void Bind() const override {}
        virtual void Unbind() const override {}

        virtual void SetVertexBuffer(const Ref<VertexBuffer>& vb) override { vb_ = vb; }
        virtual const Ref<VertexBuffer>& GetVertexBuffer() const override { return vb_; }

        virtual void SetIndexBuffer(const Ref<IndexBuffer>& ib) override { ib_ = ib; }
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return ib_; }

        virtual void SetLayout(const VertexBufferLayout& layout) { layout_ = layout; }
        virtual const VertexBufferLayout& Layout() const { return layout_; }

    private:
        Ref<VertexBuffer> vb_;
        Ref<IndexBuffer> ib_;

        VertexBufferLayout layout_{};
    };
}