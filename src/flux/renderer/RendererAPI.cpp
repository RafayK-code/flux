#pragma once

#include <flux/renderer/RendererAPI.h>

namespace flux
{
    // static initialization
    RendererAPI::Type RendererAPI::currentAPI = RendererAPI::Type::Vulkan;
}