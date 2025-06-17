using System;
using System.ComponentModel.DataAnnotations;
using McMaster.Extensions.CommandLineUtils;

namespace Flux.Bootstrapper.CLI;

[Command("flux")]
[Subcommand(typeof(CreateCommand), typeof(BuildCommand), typeof(RunCommand))]
public class Program
{
    public static void Main(string[] args) => CommandLineApplication.Execute<Program>(args);

    private int OnExecute(CommandLineApplication app, IConsole console)
    {
        app.ShowHelp();
        return 0;
    }
}