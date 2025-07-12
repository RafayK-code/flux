#pragma once

#include <flux/core/Application.h>

namespace flux
{
    extern Application* CreateApplication(int argc, char** argv);
}

int main(int argc, char** argv)
{
    flux::Application* app = flux::CreateApplication(argc, argv);
    app->OnInit();

    while(app->IsRunning())
    {
        app->Update();
    }

    app->OnShutdown();
    delete app;
}
