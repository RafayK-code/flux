using Flux.Bootstrapper.Core.Configurations;
using System.Reflection;
using System.Text.Json;
using System.Text.RegularExpressions;

namespace Flux.Bootstrapper.Core;

public class ProjectService : IProjectService
{
    private readonly string assetsDir = "Assets";
    private readonly string sourceDir = "Scripts";
    private readonly string buildDir = "Build";
    private readonly string privateEngineDir = ".flux";

    public void CreateProject(string name, string path)
    {
        if (!Directory.Exists(path))
            Directory.CreateDirectory(path);

        string projectPath = Path.Combine(path, name);
        Directory.CreateDirectory(projectPath);

        Directory.CreateDirectory(Path.Combine(projectPath, assetsDir));
        Directory.CreateDirectory(Path.Combine(projectPath, sourceDir));
        Directory.CreateDirectory(Path.Combine(projectPath, sourceDir, "Lib"));

        string projectPrivateEngineDir = Path.Combine(projectPath, privateEngineDir);
        Directory.CreateDirectory(projectPrivateEngineDir);

        var attributes = File.GetAttributes(projectPrivateEngineDir);
        File.SetAttributes(projectPrivateEngineDir, attributes | FileAttributes.Hidden);

        string namespaceName = IsValidNamespaceIdentifier(name) ? name : "Scripts";

        ProjectFile projectFile = new ProjectFile
        {
            Name = name,
            AssetLocationPath = assetsDir,
            ScriptModulePath = Path.Combine(sourceDir, $"{namespaceName}.csproj"),
            EntryPointNamespace = namespaceName,
            OutputAssembly = $"{namespaceName}.dll",
        };

        YamlDotNet.Serialization.ISerializer serializer = new YamlDotNet.Serialization.SerializerBuilder()
            .WithNamingConvention(YamlDotNet.Serialization.NamingConventions.CamelCaseNamingConvention.Instance)
            .Build();

        string projectYaml = serializer.Serialize(projectFile);
        File.WriteAllText(Path.Combine(projectPath, $"{name}.fxproj"), projectYaml);

        string assemblyFolder = AppContext.BaseDirectory;

        ProjectUserFile projectUserFile = new ProjectUserFile
        {
            EngineInstallPath = assemblyFolder,
            BuildLocationPath = buildDir,
        };

        string projectUserYaml = serializer.Serialize(projectUserFile);
        File.WriteAllText(Path.Combine(projectPath, $"{name}.fxproj.user"), projectUserYaml);

        string csTemplatePath = Path.Combine(assemblyFolder, "TemplateFiles", "Main.cs.template");

        string csTemplate = File.ReadAllText(csTemplatePath);
        string modifiedCsFile = csTemplate.Replace("{{PROJECT_NAME}}", namespaceName);

        string csFile = Path.Combine(projectPath, sourceDir, "Main.cs");
        File.WriteAllText(csFile, modifiedCsFile);

        string csprojTemplatePath = Path.Combine(assemblyFolder, "TemplateFiles", "game.csproj.template");
        string csprojTemplate = File.ReadAllText(csprojTemplatePath);

        string csprojFile = Path.Combine(projectPath, sourceDir, $"{namespaceName}.csproj");
        File.WriteAllText(csprojFile, csprojTemplate);

        File.Copy(Path.Combine(assemblyFolder, "Bin", "Flux.ScriptCore.dll"), Path.Combine(projectPath, sourceDir, "Lib", "Flux.ScriptCore.dll"), true);

        Console.WriteLine("Successfully created new project");
    }

    private static bool IsValidNamespaceIdentifier(string name) => Regex.IsMatch(name, @"^([_a-zA-Z][_a-zA-Z0-9]*)(\.[_a-zA-Z][_a-zA-Z0-9]*)*$");
}
