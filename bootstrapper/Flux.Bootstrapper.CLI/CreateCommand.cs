using McMaster.Extensions.CommandLineUtils;
using System.ComponentModel.DataAnnotations;

namespace Flux.Bootstrapper.CLI;

[Command("create", Description = "Create a new project")]
public class CreateCommand
{
    [Required]
    [Option("-n|--name", Description = "Name of the project")]
    public string Name { get; set; }

    [Argument(0, Description = "Location of the project")]
    public string PathArgument { get; set; }

    [Option("-p|--path", Description = "Location of the project")]
    public string PathOption { get; set; }

    private int OnExecute()
    {
        return 0;
    }
}
