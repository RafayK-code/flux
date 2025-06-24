#include <fxpch.h>

#include <flux/window/Window.h>

#include <flux/events/WindowEvent.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace flux
{
    static uint8_t glfwWindowCount = 0;

    Window::Window(const WindowProps& props)
    {
        data_.title = props.title;
        data_.width = props.width;
        data_.height = props.height;

        DBG_PRINT("Creating window: {0}", data_.title);

        if (glfwWindowCount == 0)
        {
            int success = glfwInit();
            DBG_ASSERT(success, "Coult not initialize GLFW");
            glfwSetErrorCallback([](int error, const char* desc)
            {
                DBG_ERROR("GLFW Error ({0}): {1}", error, desc);
            });
        }

        GraphicsContext::PreWindowCreateHints();

        // monitor may need to change from nullptr
        window_ = glfwCreateWindow(static_cast<int>(data_.width), static_cast<int>(data_.height), data_.title.c_str(), nullptr, nullptr);
        glfwWindowCount++;

        data_.context = GraphicsContext::Create(window_);

        glfwSetWindowUserPointer(window_, &data_);

        glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height)
        {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data->width = static_cast<uint32_t>(width);
            data->height = static_cast<uint32_t>(height);

            data->context->OnWindowResize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

            WindowResizeEvent e(data->width, data->height);
            data->dispatcher.DispatchEvent(e);
        });
    }

    Window::~Window()
    {
        data_.context.reset();
        glfwDestroyWindow(window_);
        glfwWindowCount--;

        if (glfwWindowCount == 0)
            glfwTerminate();
    }

    void Window::Update()
    {
        glfwPollEvents();
        data_.context->SwapBuffers();
    }

    void Window::Resize(uint32_t width, uint32_t height)
    {
        glfwSetWindowSize(window_, static_cast<int>(width), static_cast<int>(height));
    }

    void Window::SetVSync(bool enabled)
    {
        glfwSwapInterval(static_cast<int>(enabled));

        data_.vSync = enabled;
    }

    void Window::SetMode(Mode mode)
    {
        data_.mode = mode;
    }
}