using Flux.Bootstrapper.Core;
using McMaster.Extensions.CommandLineUtils;
using Microsoft.Extensions.DependencyInjection;

namespace Flux.Bootstrapper.CLI;

[Command]
[Subcommand(typeof(CreateCommand), typeof(BuildCommand), typeof(RunCommand))]
public class Program
{
    public static int Main(string[] args)
    {
        var services = new ServiceCollection()
            .AddSingleton<IProjectService, ProjectService>()
            .AddSingleton<IConsole>(PhysicalConsole.Singleton)
            .BuildServiceProvider();

        var app = new CommandLineApplication<Program>();
        app.Conventions
            .UseDefaultConventions()
            .UseConstructorInjection(services);

        return app.Execute(args);
    }

    private int OnExecute(CommandLineApplication app, IConsole console)
    {
        app.ShowHelp();
        return 0;
    }
}