#include <pch.h>

#include <flux/window/Window.h>

#include <platform/glfw/WindowGlfw.h>

namespace flux
{
    Scope<Window> Window::Create(const WindowProps& props)
    {
        return CreateScope<WindowGlfw>(props);
    }
}