#include "Application.hpp"
#include "raylib.h"
#include <iostream>

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

    InitWindow(screenWidth, screenHeight, "Paint Application with Raygui");
    SetTargetFPS(60);

    // Create a RenderTexture2D to use as a canvas
    target = LoadRenderTexture(screenWidth, screenHeight);

    // Clear render texture before entering the game loop
    BeginTextureMode(target);
    ClearBackground(RAYWHITE);
    EndTextureMode();

    bool drawing = false;
    Vector2 previousPoint = { 0, 0 };
    float brushSize = 10.0f;

    // Initialize raygui
    GuiLoadStyle("styles/raygui-dark");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Adjust text size

    while (isRunning)    // Detect window close button or ESC key
    {
        if(WindowShouldClose())
        {
            Stop();
        }
        // Update
        //----------------------------------------------------------------------------------
        Vector2 mousePos = GetMousePosition();
        Vector2 adjustedMousePos = (Vector2){ mousePos.x, mousePos.y - menuHeight }; // Adjust mouse position for menu offset


        // Start drawing
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mousePos.y > menuHeight)
        {
            drawing = true;
            previousPoint = adjustedMousePos;
        }

        // Draw on texture
        if (drawing)
        {
            BeginTextureMode(target);
            DrawLineEx(previousPoint, adjustedMousePos, brushSize, BLACK);
            EndTextureMode();
            previousPoint = adjustedMousePos;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            drawing = false;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Draw the menu area
            DrawRectangle(0, 0, screenWidth, menuHeight, LIGHTGRAY);

            // Menu Buttons
            if (GuiButton((Rectangle){ 10, 10, 100, 30 }, "Clear"))
            {
                BeginTextureMode(target);
                ClearBackground(RAYWHITE);
                EndTextureMode();
            }

            if (GuiButton((Rectangle){ 120, 10, 150, 30 }, TextFormat("Brush: %.1f", brushSize)))
            {
                brushSize += 5.0f;
                if (brushSize > 50) brushSize = 10.0f;
            }

            // End raygui

            // Draw the drawing area with vertical flip adjustment
            DrawTextureRec(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, (Vector2){ 0, menuHeight }, WHITE);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);    // Unload render texture
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    }


    void Application::Stop() {
        if (isRunning) {
            isRunning = false;
            std::cout << "Application ended" << std::endl;
            // Save Image to file
            Image image = LoadImageFromTexture(target.texture);
            ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            ImageFlipVertical(&image);
            ExportImage(image, "C:/Users/Thiago/picasso.png");
            UnloadImage(image);
 
        } else {
            std::cout << "Application already stopped" << std::endl;
        }
    }
}
