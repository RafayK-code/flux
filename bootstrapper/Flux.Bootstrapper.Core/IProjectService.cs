using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Flux.Bootstrapper.Core;

public interface IProjectService
{
    public void CreateProject(string name, string path);
}
