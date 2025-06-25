#include <fxpch.h>

#include <flux/project/Project.h>

#include <rfl/json.hpp>

namespace flux
{
    bool Project::LoadProject(const std::filesystem::path& projectPath)
    {
        std::ifstream file(projectPath);
        if (!file)
            return false;

        std::string contents = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        auto result = rfl::json::read<ProjectConfiguration>(contents);
        if (!result.has_value())
            return false;

        config_ = result.value();
        projectPath_ = projectPath;
        return true;
    }
}