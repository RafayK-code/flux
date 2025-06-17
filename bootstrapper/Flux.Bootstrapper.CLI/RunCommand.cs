using McMaster.Extensions.CommandLineUtils;
using System.ComponentModel.DataAnnotations;

namespace Flux.Bootstrapper.CLI;

[Command("run", Description = "Run a project")]
public class RunCommand
{
    [Argument(0, Description = "Path to the desired project")]
    public string ProjectPathArgument { get; set; }

    [Option("-p|--project", Description = "Path to the desired project")]
    public string ProjectPathOption { get; set; }

    [Option("-c|--configuration", Description = "Configuration to run (Debug/Release)")]
    public string Configuration { get; set; } = "Debug";

    [Option("--args", Description = "Arguments to pass to the application")]
    public string Arguments { get; set; }

    private int OnExecute()
    {
        return 0;
    }
}
