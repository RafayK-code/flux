using System.Text.Json.Serialization;

namespace Flux.Bootstrapper.Core.ProjectFiles;

internal class ConfigurationFile
{
    [JsonPropertyName("name")]
    public string Name { get; set; }
}