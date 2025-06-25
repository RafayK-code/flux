#include <fxpch.h>

#include <flux.h>
#include <flux/core/EntryPoint.h>

class FluxHostApplication : public flux::Application
{
public:
    FluxHostApplication() = default;
    ~FluxHostApplication() = default;

    virtual void OnInit() override
    {
        /*
        DBG_PRINT("On Init");
        flux::ScriptEngine& scriptEngine = GetScriptEngine();
        scriptEngine.LoadAppAssembly("DummyLib.dll");
        auto obj = scriptEngine.InstantiateApp("Scripts", "TestScript");
        obj->InvokeMethod("PrintHello");
        obj->InvokeMethod("Add", 5, 10);
        obj->InvokeMethod("PrintString", "Hello World!");
        */
    }

    virtual void OnUpdate(float dt) override
    {
    }

    virtual void OnShutdown() override
    {
    }

private:

};

flux::Application* flux::CreateApplication(int argc, char** argv)
{
    return new FluxHostApplication();
}