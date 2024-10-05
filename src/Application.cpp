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


    SetConfigFlags(ConfigFlags::FLAG_WINDOW_RESIZABLE |
                    ConfigFlags::FLAG_BORDERLESS_WINDOWED_MODE);
    InitWindow(screenWidth, screenHeight, "Helium");
    SetTargetFPS(60);
    NoteArea area(_config);
    
    SetExitKey(KEY_NULL);

    Camera2D camera = {0};
    camera.target = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    camera.offset = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    // Initialize raygui
    GuiLoadStyle("styles/raygui-dark");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Adjust text size

    float menuHeight = _config->TopMenuBarHeight;
    Vector2 scroll = {0,0};
    int fileDropdownActive = 0;
    int dropdownEditMode = 0;
    _config->Formatting.loadFonts();

    GuiSetFont(_config->Formatting.DefaultFont);

    area.Initialize(_config->TopMenuBarHeight); // Offset the NoteArea 50px down
    while(isRunning)
    {
        if(WindowShouldClose())
        {
            Stop();
        }


        // Update
        area.Update();
        
        scroll.y -= GetMouseWheelMove() * _config->Formatting.Paragraph * _config->ScrollLineCount;
        if(scroll.y < 0) scroll.y = 0;
        camera.target = {
            GetScreenWidth() * 0.5f + scroll.x,
            GetScreenHeight() * 0.5f + scroll.y,
        };
        camera.offset = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
        // Draw
        BeginDrawing();
        ClearBackground(_config->ColorTheme.Background);

       DrawRectangleRec({(GetScreenWidth() - _config->MaxNoteWidth) * 0.5f, 0, static_cast<float>(_config->MaxNoteWidth), static_cast<float>(GetScreenHeight())}, _config->ColorTheme.Foreground);
        // NOTE AREA
        // ------------------------------------------
        BeginMode2D(camera);
        area.Draw();
        EndMode2D();
        DrawFPS(0,0); 
       
       // UI
        // ------------------------------------------ 
        DrawRectangleRec({0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(_config->TopMenuBarHeight)}, _config->ColorTheme.Foreground);
        if(GuiDropdownBox({0, 0, 125, 25}, "#01#ONE;#02#TWO;#03#THREE;#04#FOUR", &fileDropdownActive, dropdownEditMode)) {
            dropdownEditMode = !dropdownEditMode;
        }

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
