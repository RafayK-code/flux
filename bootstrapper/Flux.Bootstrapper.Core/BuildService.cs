using Flux.Bootstrapper.Core.Configurations;
using Flux.Bootstrapper.Core.Utils;
using System.Diagnostics;

namespace Flux.Bootstrapper.Core;

public class BuildService : IBuildService
{
    public void Build(string path, IBuildService.BuildType buildType = IBuildService.BuildType.Release)
    {
        string projectPath = FileUtils.FindProjectFile(path) ?? throw new Exception("Could not find project file");
        string projectUserPath = FileUtils.FindProjectUserFile(path) ?? throw new Exception("Could not find project user file");

        YamlDotNet.Serialization.IDeserializer deserializer = new YamlDotNet.Serialization.DeserializerBuilder()
            .WithNamingConvention(YamlDotNet.Serialization.NamingConventions.CamelCaseNamingConvention.Instance)
            .Build();

        string projectPathYaml = File.ReadAllText(projectPath);
        string projectUserPathYaml = File.ReadAllText(projectUserPath);

        ProjectFile project = deserializer.Deserialize<ProjectFile>(projectPathYaml);
        ProjectUserFile projectUser = deserializer.Deserialize<ProjectUserFile>(projectUserPathYaml);

        string buildPath = Path.Combine(path, projectUser.BuildLocationPath);
        if (!Directory.Exists(buildPath))
        {
            Directory.CreateDirectory(buildPath);
        }

        string internalBuildDir = Path.Combine(buildPath, ".flux");
        if (!Directory.Exists(internalBuildDir))
        {
            Directory.CreateDirectory(internalBuildDir);
            var attributes = File.GetAttributes(internalBuildDir);
            File.SetAttributes(internalBuildDir, attributes | FileAttributes.Hidden);
        }

        //string dotnetArgs = $"build \"{project.ScriptModulePath}\" -c ${buildType} -o {internalBuildDir} -property:BaseIntermediateOutputPath={Path.Combine(internalBuildDir, "obj/")} -p:EnginePath=\"{projectUser.EngineInstallPath}\"";
        string dotnetArgs = $"build \"{project.ScriptModulePath}\" -c {buildType} -o {internalBuildDir} -property:BaseIntermediateOutputPath={Path.Combine(internalBuildDir, "obj/")}";
        ExecuteDotnetCommand(path, dotnetArgs).Wait();

        string buildConfigDir = Path.Combine(buildPath, buildType.ToString());
    }

    public void Rebuild()
    {

    }

    public void Clean()
    {

    }

    public void Run()
    {

    }

    private static async Task ExecuteDotnetCommand(string workDir, string args)
    {
        Process process = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = "dotnet",
                Arguments = args,
                WorkingDirectory = workDir,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            }
        };

        var output = "";
        var error = "";

        process.OutputDataReceived += (sender, e) =>
        {
            if (e.Data != null)
                output += e.Data + Environment.NewLine;
        };

        process.ErrorDataReceived += (sender, e) =>
        {
            if (e.Data != null)
                error += e.Data + Environment.NewLine;
        };

        process.Start();
        process.BeginOutputReadLine();
        process.BeginErrorReadLine();

        await process.WaitForExitAsync();
#if DEBUG
        Console.WriteLine("Output:\n" + output);
        if (!string.IsNullOrEmpty(error))
            Console.WriteLine("Error:\n" + error);
#endif
    }
}
