namespace Flux.Bootstrapper.Core.Configurations;

internal class ProjectFile
{
    public string Name { get; set; } = "App";
    public string AssetLocationPath { get; set; } = "Assets";
    public string ScriptModulePath { get; set; } = "Scripts";
    public string EntryPointNamespace { get; set; } = "App";
    public string EntryPointClass { get; set; } = "GameMain";
    public string OutputAssembly { get; set; } = "App.dll";

    public struct WindowConfiguration
    {
        public WindowConfiguration() { }

        public int Width { get; set; } = 1280;
        public int Height { get; set; } = 720;
        public string Type { get; set; } = "Windowed";
    }

    public WindowConfiguration WindowConfig { get; set; } = new WindowConfiguration();

    public struct RendererConfiguration
    {
        public RendererConfiguration() { }

        public string Api { get; set; } = "Vulkan";
    }

    public RendererConfiguration RendererConfig { get; set; } = new RendererConfiguration();
}
