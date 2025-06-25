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
        flux::Project& project = GetProject();
        flux::ScriptEngine& scriptEngine = GetScriptEngine();

        scriptEngine.LoadAppAssembly(project.ProjectRoot().parent_path() / project.Config().scriptModulePath);
        mainScript_ = scriptEngine.InstantiateApp(project.Config().entryPointNamespace, project.Config().entryPointClass);

        mainScript_->InvokeMethod("OnInit");
    }

    virtual void OnUpdate(float dt) override
    {
        mainScript_->InvokeMethod("OnUpdate", dt);
    }

    virtual void OnShutdown() override
    {
    }

private:
    flux::Ref<flux::ScriptObject> mainScript_;
};

flux::Application* flux::CreateApplication(int argc, char** argv)
{
    return new FluxHostApplication();
}