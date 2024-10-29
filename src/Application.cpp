#include "Application.hpp"
#include "Configuration.hpp"
#include "InputCombo.hpp"
#include "NoteArea.hpp"
#include "constants.h"
#include "raylib.h"
#include "rlgl.h"
#include "UiUtils.hpp"
#include "utils.hpp"
#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "OpenFileModal.hpp"
#include <SaveFileModal.hpp>

namespace Helium {

Application::Application() : isRunning(false), _inputHandler(std::make_unique<InputHandler>()), _noteArea(std::make_unique<NoteArea>(_inputHandler)) {
    _noteArea->SetMode(NoteMode::READ);
    _inputHandler->SetMode(NoteMode::READ);

    // GLOBAL ACTIONS
    _inputHandler->AddGlobalAction(InputCombo(KEY_ESCAPE), [this]() {
        _noteArea->SetMode(NoteMode::READ);
        _inputHandler->SetMode(NoteMode::READ);
    });

    _inputHandler->AddGlobalAction(InputCombo(KEY_S, KEY_LEFT_CONTROL), [this]() {
        _noteArea->Save();
    });

    _inputHandler->AddGlobalAction(InputCombo(KEY_F1), [this]() { std::cout << Helium::Configuration::getInstance().serialize(); });

    // READ MODE
     
    // WRITE MODE
    _inputHandler->AddAction(NoteMode::WRITE, InputCombo(KEY_TAB), [this]() { 
        this->_noteArea->GetCursor()->ReplaceWordWithMacro(Helium::Configuration::getInstance().Macros);
        this->_noteArea->SetDirty();
    });
    _inputHandler->AddAction(NoteMode::WRITE, InputCombo(KEY_C, KEY_LEFT_CONTROL), [this]() {
        if(this->_noteArea->GetCursor()->IsHighlighting()) {
            SetClipboardText(_noteArea->GetCursor()->GetHighlightedText().c_str());
            _noteArea->GetCursor()->Deselect();
        }
    });
    // DRAW MODE
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
    SetExitKey(KEY_NULL);
    
    MaximizeWindow();

    Camera2D camera = {0};
    camera.target = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    camera.offset = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    // Initialize raygui
    GuiLoadStyle("styles/raygui-dark");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Adjust text size

    bool isModalOpen = false;
    bool wasModalClosed = false;

    float menuHeight = Helium::Configuration::getInstance().TopMenuBarHeight;
    Vector2 scroll = {0,0};
    int fileDropdownActive = 0;
    int fileDropdownValue = 0;
    Helium::Configuration::getInstance().Formatting.loadFonts();
    bool innerClicked;
    GuiSetFont(Helium::Configuration::getInstance().Formatting.DefaultFont);
    Rectangle modalRect = { 100, 100, 400, 300 };
    OpenFileModal fileOpenModal(modalRect );
    SaveFileModal saveFileModal(modalRect );
    std::vector<std::string> exampleFiles = { "file1.txt", "file2.txt", "file3.txt" };
    _noteArea->Initialize(Helium::Configuration::getInstance().TopMenuBarHeight); // Offset the NoteArea 50px down
    while(isRunning)
    {
        wasModalClosed = isModalOpen;   // Assume this as a temporary value
        isModalOpen = fileOpenModal.IsVisible() || saveFileModal.IsVisible();
        wasModalClosed = !isModalOpen && wasModalClosed; // Check if the modal is closed but was open

        innerClicked = false;  // Reset innerClicked flag each frame
        if(WindowShouldClose())
        {
            Stop();
        }


        // Update
        // --------------------------------------------------------------------------------------------------
        _inputHandler->Update();

        // Modals
        fileOpenModal.Update();
        saveFileModal.Update();

        // Handle updates only if no modal is open
        if(!isModalOpen) {
            _noteArea->Update();

            if(!IsKeyDown(KEY_LEFT_SHIFT))
                scroll.y -= GetMouseWheelMove() * Helium::Configuration::getInstance().Formatting.Paragraph * Helium::Configuration::getInstance().ScrollLineCount;
            if(scroll.y < 0) scroll.y = 0;
            camera.target = {
                GetScreenWidth() * 0.5f + scroll.x,
                GetScreenHeight() * 0.5f + scroll.y,
            };
            camera.offset = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
        }
        // Draw
        // --------------------------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(Helium::Configuration::getInstance().ColorTheme.Background);

        DrawRectangleRec({(GetScreenWidth() - Helium::Configuration::getInstance().MaxNoteWidth) * 0.5f, 0, static_cast<float>(Helium::Configuration::getInstance().MaxNoteWidth), static_cast<float>(GetScreenHeight())}, Helium::Configuration::getInstance().ColorTheme.Foreground);
        // NOTE AREA
        // -------------------------------------
        BeginMode2D(camera);
        _noteArea->Draw();
        EndMode2D();
        DrawFPS(0,0); 
       
        // UI
        // --------------------------------------------------------------------------------------------------
        DrawRectangleRec({0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(Helium::Configuration::getInstance().TopMenuBarHeight)}, Helium::Configuration::getInstance().ColorTheme.Foreground);
        fileOpenModal.Draw();
        saveFileModal.Draw();
    
        switch(fileDropdownValue) {
            case 1: // Open
                fileOpenModal.Show("C:/Users/Thiago");
            break;
            case 2: // Save
                if(_noteArea->GetPath().empty()) {
                    saveFileModal.Show();
                }
            default: break;
        }
        fileDropdownValue = UiUtils::Dropdown({0, 0, 150, 20}, Helium::Configuration::getInstance().ColorTheme.Foreground, "File;Open;Save#CTRL+S" , &fileDropdownActive);
        EndDrawing(); 

        if(wasModalClosed) {
            if(!fileOpenModal.IsVisible() && !fileOpenModal.GetSelectedFile().empty()) {
                if(Utils::IsFile(fileOpenModal.GetSelectedFile())) {
                    _noteArea->TryLoadNote(fileOpenModal.GetSelectedFile());
                } 
            }     
            if(saveFileModal.HasClosed() && !saveFileModal.GetFilePath().empty()) {
                std::cout << saveFileModal.GetFilePath() << std::endl;
            }   
        }
    }
    CloseWindow();
    return;
}

    void Application::Stop() {
        if (isRunning) {
            isRunning = false;
            Helium::Configuration::getInstance().unloadResources();
            std::cout << "Application ended" << std::endl;
            // Save Image to file
        } else {
            std::cout << "Application already stopped" << std::endl;
        }
    }
}
