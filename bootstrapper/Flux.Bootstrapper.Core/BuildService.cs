using Flux.Bootstrapper.Core.Configurations;
using Flux.Bootstrapper.Core.Utils;
using System.Diagnostics;
using System.Runtime.InteropServices;

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

        string dotnetArgs = $"build \"{project.ScriptModulePath}\" -c {buildType} -o {internalBuildDir} -property:BaseIntermediateOutputPath={Path.Combine(internalBuildDir, "obj/")}";
        ExecuteDotnetCommand(path, dotnetArgs).Wait();

        string buildConfigDir = Path.Combine(buildPath, buildType.ToString());
        if (!Directory.Exists(buildConfigDir))
        {
            Directory.CreateDirectory(buildConfigDir);
        }

        // COPY FILES
        File.Copy(projectPath, Path.Combine(buildConfigDir, Path.GetFileName(projectPath)), true);

        File.Copy(Path.Combine(internalBuildDir, project.OutputAssembly), Path.Combine(buildConfigDir, project.OutputAssembly), true);

        File.Copy(Path.Combine(internalBuildDir, "Flux.ScriptCore.dll"), Path.Combine(buildConfigDir, "Flux.ScriptCore.dll"), true);

        string exeExt = RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? ".exe" : "";
        File.Copy(Path.Combine(projectUser.EngineInstallPath, "Bin", $"flux-host{exeExt}"), Path.Combine(buildConfigDir, $"{project.Name}{exeExt}"), true);

        if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            Process.Start("chmod", $"+x \"{Path.Combine(buildConfigDir, $"{project.Name}{exeExt}")}\"")?.WaitForExit();
        }

        string sharedLibExt = ".dll";
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
        {
            sharedLibExt = ".so";
        }
        else if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
        {
            sharedLibExt = ".dylib";
        }

        File.Copy(Path.Combine(projectUser.EngineInstallPath, "Bin", $"mono-2.0-sgen{sharedLibExt}"), Path.Combine(buildConfigDir, $"mono-2.0-sgen{sharedLibExt}"), true);
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

        Console.WriteLine("Output:\n" + output);
        if (!string.IsNullOrEmpty(error))
            Console.WriteLine("Error:\n" + error);
    }
}
