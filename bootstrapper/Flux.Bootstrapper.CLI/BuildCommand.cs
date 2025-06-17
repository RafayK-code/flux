using McMaster.Extensions.CommandLineUtils;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;

namespace Flux.Bootstrapper.CLI;

[Command("build", Description = "Build a project")]
public class BuildCommand
{
    [Argument(0, Description = "Path to the desired project")]
    public string ProjectPathArgument { get; set; }

    [Option("-p|--project", Description = "Path to the desired project")]
    public string ProjectPathOption { get; set; }

    [Option("-c|--configuration", Description = "Build configuration (Debug/Release)")]
    public string Configuration { get; set; } = "Debug";

    [Option("--clean", Description = "Clean before building")]
    public bool Clean { get; set; }

    private int OnExecute()
    {
        return 0;
    }
}