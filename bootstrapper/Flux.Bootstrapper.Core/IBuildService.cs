using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Flux.Bootstrapper.Core;

public interface IBuildService
{
    public enum BuildType
    {
        Debug, Release
    }

    public void Build(string path, BuildType buildType = BuildType.Release);
    public void Rebuild();
    public void Clean();
    public void Run();
}
