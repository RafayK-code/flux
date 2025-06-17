using Flux.Bootstrapper.Core.ProjectFiles;
using System.Reflection;
using System.Text.Json;
using System.Text.RegularExpressions;

namespace Flux.Bootstrapper.Core;

public class ProjectService : IProjectService
{
    private readonly string assetsDir = "assets";
    private readonly string sourceDir = "src";
    private readonly string privateEngineDir = ".flux";

    public void CreateProject(string name, string path)
    {
        if (!Directory.Exists(path))
            Directory.CreateDirectory(path);

        string projectPath = Path.Combine(path, name);
        Directory.CreateDirectory(projectPath);

        Directory.CreateDirectory(Path.Combine(projectPath, assetsDir));
        Directory.CreateDirectory(Path.Combine(projectPath, sourceDir));

        string projectPrivateEngineDir = Path.Combine(projectPath, privateEngineDir);
        Directory.CreateDirectory(projectPrivateEngineDir);

        var attributes = File.GetAttributes(projectPrivateEngineDir);
        File.SetAttributes(projectPrivateEngineDir, attributes | FileAttributes.Hidden);

        JsonSerializerOptions options = new JsonSerializerOptions
        {
            WriteIndented = true
        };

        ConfigurationFile configFile = new ConfigurationFile();
        configFile.Name = name;

        string configJson = JsonSerializer.Serialize(configFile, options);
        File.WriteAllText(Path.Combine(projectPath, "config.json"), configJson);

        MetadataFile metadataFile = new MetadataFile();
        metadataFile.Name = name;
        metadataFile.ProjectCreated = DateTime.Now;

        string metadataJson = JsonSerializer.Serialize(metadataFile, options);
        File.WriteAllText(Path.Combine(projectPath, privateEngineDir, "metadata.json"), metadataJson);

        string assemblyFolder = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
        string cppTemplatePath = Path.Combine(assemblyFolder, "TemplateFiles", "main.cpp.template");

        string className = IsValidCppIdentifier(name) ? name : "MyApp";

        string cppTemplate = File.ReadAllText(cppTemplatePath);
        string modifiedCppFile = cppTemplate.Replace("{{PROJECT_NAME}}", className);

        string cppFile = Path.Combine(projectPath, sourceDir, "main.cpp");
        File.WriteAllText(cppFile, modifiedCppFile);
    }

    private static bool IsValidCppIdentifier(string name) => Regex.IsMatch(name, @"^[a-zA-Z_][a-zA-Z0-9_]*$");
}
