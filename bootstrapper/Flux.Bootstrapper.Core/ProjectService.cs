using Flux.Bootstrapper.Core.ProjectFiles;
using System.Text.Json;

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
        Directory.CreateDirectory(Path.Combine(projectPath, privateEngineDir));

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
    }
}
