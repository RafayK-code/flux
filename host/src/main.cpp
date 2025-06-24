#include <fxpch.h>

#include <flux.h>

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

int main(int argc, char** argv)
{
    FluxHostApplication* app = new FluxHostApplication();
    app->OnInit();
    while (app->IsRunning())
    {
        app->Update();
    }
    app->OnShutdown();
    delete app;

    return 0;
}