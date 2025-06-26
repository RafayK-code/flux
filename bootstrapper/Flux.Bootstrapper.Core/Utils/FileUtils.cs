using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Flux.Bootstrapper.Core.Utils;

internal class FileUtils
{
    public static string? FindProjectFile(string dir)
    {
        foreach (string file in Directory.EnumerateFiles(dir))
        {
            if (Path.GetExtension(file) == ".fxproj")
                return file;
        }

        return null;
    }

    public static string? FindProjectUserFile(string dir)
    {
        foreach (string file in Directory.EnumerateFiles(dir))
        {
            if (file.EndsWith(".fxproj.user"))
                return file;
        }

        return null;
    }
}
