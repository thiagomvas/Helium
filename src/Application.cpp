#include "Application.hpp"
#include "Configuration.hpp"
#include "NoteArea.hpp"
#include "constants.h"
#include "raylib.h"
#include "rlgl.h"
#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

namespace Helium {

    Application::Application(std::shared_ptr<Configuration> config) : isRunning(false) {
        std::cout << "Application created" << std::endl;
        _config = config;
    }

    Application::~Application() {
        if (isRunning) {
            Stop();
        }
    }

void Application::Start() {
    if(isRunning)
        return;
    isRunning = true;
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int menuHeight = 50;  // Height of the menu bar
    SetConfigFlags(ConfigFlags::FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Helium");
    SetTargetFPS(60);
    NoteArea area(_config);
    area.Initialize();
    
    SetExitKey(KEY_NULL);

    bool drawing = false;
    Vector2 previousPoint = { 0, 0 };
    float brushSize = 10.0f;

    // Initialize raygui
    GuiLoadStyle("styles/raygui-dark");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Adjust text size
    
    while(isRunning)
    {
        if(WindowShouldClose())
        {
            Stop();
        }


        // Update
        area.Update();

        // Draw
        BeginDrawing();
        ClearBackground(_config->ColorTheme.Background);
        area.Draw();
        DrawFPS(0,0); 
        EndDrawing();
    }
    CloseWindow();
    return;
}

    void Application::Stop() {
        if (isRunning) {
            isRunning = false;
            _config->unloadResources();
            std::cout << "Application ended" << std::endl;
            // Save Image to file
        } else {
            std::cout << "Application already stopped" << std::endl;
        }
    }
}
