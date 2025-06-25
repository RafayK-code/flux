#pragma once

#include <flux/core/base.h>
#include <flux/core/Ref.h>

#include <flux/window/Window.h>
#include <flux/scripting/ScriptEngine.h>
#include <flux/project/Project.h>

namespace flux
{
    class FLUX_API Application
    {
    public:
        Application();
        virtual ~Application();

        virtual void OnInit() {};
        virtual void OnUpdate(float dt) {};
        virtual void OnShutdown() {};

        void Update();

        inline bool IsRunning() const { return isRunning_; }

        inline EventDispatcher& Dispatcher() { return dispatcher_; }
        inline const EventDispatcher& Dispatcher() const { return dispatcher_; }

        ScriptEngine& GetScriptEngine() { return *scriptEngine_; }

        static Application* Instance() { return instance_; }

    private:
        EventDispatcher dispatcher_;

        Scope<Window> window_;
        EventListener windowListener_;

        Scope<ScriptEngine> scriptEngine_;

        Scope<Project> project_;

        bool isRunning_;
        static Application* instance_;
    };
}