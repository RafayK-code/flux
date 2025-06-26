using Flux.Bootstrapper.Core;
using McMaster.Extensions.CommandLineUtils;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;

namespace Flux.Bootstrapper.CLI;

[Command("build", Description = "Build a project")]
public class BuildCommand
{
    private readonly IBuildService buildService_;

    public BuildCommand(IBuildService buildService)
    {
        buildService_ = buildService;
    }

    [Argument(0, Description = "Path to the desired project")]
    public string ProjectPathArgument { get; set; }

    [Option("-p|--project", Description = "Path to the desired project")]
    public string ProjectPathOption { get; set; }

    [Option("-c|--configuration", Description = "Build configuration (Debug/Release)")]
    public string Configuration { get; set; } = "Release";

    [Option("--clean", Description = "Clean before building")]
    public bool Clean { get; set; }

    private int OnExecute()
    {
        IBuildService.BuildType buildType = IBuildService.BuildType.Release;
        if (!Enum.TryParse<IBuildService.BuildType>(Configuration, out buildType))
        {
            Console.WriteLine("Unknown build configuration - Defaulting to Release");
        }

        buildService_.Build(GetFullPath(), buildType);
        return 0;
    }

    private string GetFullPath()
    {
        if (ProjectPathArgument != null)
        {
            if (Path.IsPathFullyQualified(ProjectPathArgument))
            {
                return ProjectPathArgument;
            }
            else
            {
                return Path.Combine(Directory.GetCurrentDirectory(), ProjectPathArgument);
            }
        }
        else if (ProjectPathOption != null)
        {
            if (Path.IsPathFullyQualified(ProjectPathOption))
            {
                return ProjectPathOption;
            }
            else
            {
                return Path.Combine(Directory.GetCurrentDirectory(), ProjectPathOption);
            }
        }

        return Directory.GetCurrentDirectory();
    }
}