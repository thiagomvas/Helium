#include "Application.hpp"
#include "NoteArea.hpp"
#include "constants.h"
#include "raylib.h"
#include "rlgl.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

namespace Helium {

    Application::Application() : isRunning(false) {
        std::cout << "Application created" << std::endl;
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
    NoteArea area;
    area.Initialize();
    
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
        ClearBackground(Colors::BACKGROUND);
        area.Draw();
        EndDrawing();
    }
    return;
}

    void Application::Stop() {
        if (isRunning) {
            isRunning = false;
            std::cout << "Application ended" << std::endl;
            // Save Image to file
        } else {
            std::cout << "Application already stopped" << std::endl;
        }
    }
}
