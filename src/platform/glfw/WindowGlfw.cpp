#include <pch.h>

#include <platform/glfw/WindowGlfw.h>

namespace flux
{
    static uint8_t glfwWindowCount = 0;

    WindowGlfw::WindowGlfw(const WindowProps& props)
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

        // TODO: remove this. Not the responsibility of the window, but of the GraphicsContext
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // monitor may need to change from nullptr
        window_ = glfwCreateWindow(static_cast<int>(data_.width), static_cast<int>(data_.height), data_.title.c_str(), nullptr, nullptr);
        glfwWindowCount++;

        glfwMakeContextCurrent(window_);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            DBG_CRIT("Failed to initialize glad, {0}", glfwGetError(nullptr));
        }

        const GLubyte* version = glGetString(GL_VERSION);
        DBG_PRINT("OpenGL version: {0}", reinterpret_cast<const char*>(version));

        glViewport(0, 0, 1280, 720);

        glfwSetWindowUserPointer(window_, &data_);

        glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height)
        {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data->width = static_cast<uint32_t>(width);
            data->height = static_cast<uint32_t>(height);
        });
    }

    WindowGlfw::~WindowGlfw()
    {
        glfwDestroyWindow(window_);
        glfwWindowCount--;

        if (glfwWindowCount == 0)
            glfwTerminate();
    }

    void WindowGlfw::Update()
    {
        glfwPollEvents();
        glfwSwapBuffers(window_);
    }

    void WindowGlfw::Resize(uint32_t width, uint32_t height)
    {
        glfwSetWindowSize(window_, static_cast<int>(width), static_cast<int>(height));
    }

    void WindowGlfw::SetVSync(bool enabled)
    {
        glfwSwapInterval(static_cast<int>(enabled));

        data_.vSync = enabled;
    }

    void WindowGlfw::SetMode(Mode mode)
    {
        data_.mode = mode;
    }
}