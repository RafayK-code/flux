using System.Text.Json.Serialization;

namespace Flux.Bootstrapper.Core.ProjectFiles;

internal class MetadataFile
{
    [JsonPropertyName("name")]
    public string Name { get; set; }

    [JsonPropertyName("project_created")]
    public DateTime ProjectCreated { get; set; }
}