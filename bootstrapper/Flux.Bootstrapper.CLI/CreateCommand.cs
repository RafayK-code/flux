using Flux.Bootstrapper.Core;
using McMaster.Extensions.CommandLineUtils;
using System.ComponentModel.DataAnnotations;

namespace Flux.Bootstrapper.CLI;

[Command("create", Description = "Create a new project")]
public class CreateCommand
{
    private readonly IProjectService projectService_;

    public CreateCommand(IProjectService projectService)
    {
        projectService_ = projectService;
    }

    [Required]
    [Option("-n|--name", Description = "Name of the project")]
    public string Name { get; set; }

    [Argument(0, Description = "Location of the project")]
    public string PathArgument { get; set; }

    [Option("-p|--path", Description = "Location of the project")]
    public string PathOption { get; set; }

    private int OnExecute()
    {
        projectService_.CreateProject(Name, GetFullPath());
        return 0;
    }

    private string GetFullPath()
    {
        if (PathArgument != null) 
        {
            if (Path.IsPathFullyQualified(PathArgument))
            {
                return PathArgument;
            }
            else
            {
                return Path.Combine(Directory.GetCurrentDirectory(), PathArgument);
            }
        }
        else if (PathOption != null)
        {
            if (Path.IsPathFullyQualified(PathOption))
            {
                return PathOption;
            }
            else
            {
                return Path.Combine(Directory.GetCurrentDirectory(), PathOption);
            }
        }

        return Directory.GetCurrentDirectory();
    }
}
