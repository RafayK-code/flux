#include <fxpch.h>

#include <flux/project/Project.h>

#include <rfl/json.hpp>
#include <rfl/yaml.hpp>

namespace flux
{
    bool Project::Load(const std::filesystem::path& projectPath)
    {
        std::ifstream file(projectPath);
        if (!file)
            return false;

        std::string contents = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        auto result = rfl::yaml::read<ProjectConfiguration>(contents);
        if (!result.has_value())
            return false;

        config_ = result.value();
        projectPath_ = projectPath;
        return true;
    }

    std::filesystem::path FindProjectFile(const std::filesystem::path& dir)
    {
        try
        {
            for (const auto& entry : std::filesystem::directory_iterator(dir))
            {
                if (entry.is_regular_file() && entry.path().extension() == PROJECT_EXTENSION) 
                {
                    return entry.path();
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {}

        return {};
    }
}