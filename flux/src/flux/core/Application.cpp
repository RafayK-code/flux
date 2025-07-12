#include <fxpch.h>

#include <flux/core/Application.h>

#include <flux/events/WindowEvent.h>
#include <flux/events/AppEvent.h>

#include <flux/util/File.h>

namespace flux
{
    Application* Application::instance_ = nullptr;

    Application::Application()
    {
        instance_ = this;

        LogManager::Init("Game");

        project_ = CreateBox<Project>();
        bool res = project_->Load(FindProjectFile(BinaryDirectory()));
        if (!res)
        {
            DBG_WARN("No specified .fxproj file found. Using default configuration");
            project_->LoadDefault();
        }

        window_ = CreateBox<Window>(WindowProps{ .title = project_->Config().name,
            .width = static_cast<uint32_t>(project_->Config().windowConfig.width),
            .height = static_cast<uint32_t>(project_->Config().windowConfig.height) }
        );

        windowListener_.Listen<WindowCloseEvent>(window_->Dispatcher(), [this](const WindowCloseEvent& e) {
            isRunning_ = false;
        });

        scriptEngine_ = CreateBox<ScriptEngine>();
        scriptEngine_->Init();

        isRunning_ = true;
    }

    Application::~Application()
    {
        scriptEngine_.reset();
        window_.reset();
        project_.reset();
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