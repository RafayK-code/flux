#include <pch.h>

#include <flux/renderer/Pipeline.h>
#include <flux/renderer/RendererAPI.h>

#include <platform/vulkan/VulkanPipeline.h>

namespace flux
{
    Ref<Pipeline> Pipeline::Create(const PipelineSpecification& specification)
    {
        switch (RendererAPI::Current())
        {
        case RendererAPI::Type::Vulkan: return CreateRef<VulkanPipeline>(specification);
        case RendererAPI::Type::OpenGL: return nullptr;
        }

        DBG_ASSERT(false, "Invalid renderer API");
        return nullptr;
    }
}