#pragma once

#include <flux/core/base.h>

#include <string>
#include <filesystem>

namespace flux
{
    struct FLUX_API ProjectConfiguration
    {
        std::string name;

        std::string assetLocationPath;
        std::string scriptModulePath;

        std::string entryPointNamespace;
        std::string entryPointClass;

        std::string outputAssembly;

        struct WindowConfiguration
        {
            int width;
            int height;
            std::string type;
        };

        WindowConfiguration windowConfig;

        struct RendererConfiguration
        {
            std::string api;
        };

        RendererConfiguration rendererConfig;
    };

    class FLUX_API Project
    {
    public:
        Project() = default;
        ~Project() = default;

        bool Load(const std::filesystem::path& projectPath);
        void LoadDefault();

        const ProjectConfiguration& Config() const { return config_; }

        const std::filesystem::path& ProjectRoot() const { return projectPath_; }

    private:
        ProjectConfiguration config_;
        std::filesystem::path projectPath_;
    };

    std::filesystem::path FindProjectFile(const std::filesystem::path& dir);
}