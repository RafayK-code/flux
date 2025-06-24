#include <fxpch.h>

#include <flux/core/Application.h>

#include <flux/events/WindowEvent.h>
#include <flux/events/AppEvent.h>

namespace flux
{
    Application* Application::instance_ = nullptr;

    Application::Application()
    {
        instance_ = this;

        LogManager::Init("Game");
        window_ = CreateScope<Window>(WindowProps{ "Hello World", 1280, 720 });

        windowListener_.Listen<WindowCloseEvent>(window_->Dispatcher(), [this](const WindowCloseEvent& e) {
            isRunning_ = false;
        });

        scriptEngine_ = CreateScope<ScriptEngine>();
        scriptEngine_->Init();
        isRunning_ = true;
    }

    Application::~Application()
    {
        scriptEngine_.reset();
        window_.reset();
        LogManager::Shutdown();
        instance_ = nullptr;
    }

    void Application::Update()
    {
        static auto lastTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();

        float dt = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        window_->Update();

        OnUpdate(dt);
        AppUpdateEvent e(dt);
        dispatcher_.DispatchEvent(e);
    }
}