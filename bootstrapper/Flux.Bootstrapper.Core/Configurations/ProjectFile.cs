namespace Flux.Bootstrapper.Core.Configurations;

internal class ProjectFile
{
    public string Name { get; set; } = "App";
    public string AssetLocationPath { get; set; } = "Assets";
    public string ScriptModulePath { get; set; } = "Scripts";
    public string EntryPointNamespace { get; set; } = "App";
    public string EntryPointClass { get; set; } = "GameMain";
    public string OutputAssembly { get; set; } = "App.dll";
}
