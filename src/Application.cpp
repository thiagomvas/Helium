#include "Application.hpp"
#include "Configuration.hpp"
#include "InputCombo.hpp"
#include "NoteArea.hpp"
#include "UiUtils.hpp"
#include "constants.h"
#include "raylib.h"
#include "rlgl.h"
#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#define RAYGUI_IMPLEMENTATION
#include "DataPath.hpp"
#include "Dropdown.hpp"
#include "OpenFileModal.hpp"
#include "StackPanel.hpp"
#include "raygui.h"
#include <SaveFileModal.hpp>
#include "SettingsModal.hpp"

namespace Helium {

Application::Application()
    : isRunning(false), _inputHandler(std::make_unique<InputHandler>()), _noteArea(std::make_unique<NoteArea>()), _saveModal({100, 100, 400, 300}, Helium::Configuration::getInstance().SUPPORTED_NOTE_FILE_TYPE) {
    _noteArea->SetMode(NoteMode::READ);
    _inputHandler->SetMode(NoteMode::READ);

    // GLOBAL ACTIONS
    _inputHandler->AddGlobalAction(InputCombo(KEY_ESCAPE), [this]() {
        _noteArea->SetMode(NoteMode::READ);
        _inputHandler->SetMode(NoteMode::READ);
    });

    _inputHandler->AddGlobalAction(InputCombo(KEY_S, KEY_LEFT_CONTROL), [this]() {
        if (_noteArea->GetPath().empty()) {
            OpenSaveModal();
        } else {
            _noteArea->Save();
        }
    });

    _inputHandler->AddGlobalAction(InputCombo(KEY_F1), [this]() {
        std::string appDataPath = GetAppDataPath();
        std::filesystem::path configFilePath = appDataPath + "/Helium/config.txt";

        if (std::filesystem::exists(configFilePath)) {
            std::ifstream configFile(configFilePath);
            std::string configData((std::istreambuf_iterator<char>(configFile)),
                                   std::istreambuf_iterator<char>());
            configFile.close();

            // Reload the configuration from file
            Helium::Configuration::getInstance().deserialize(configData);

            // Print the updated configuration as a serialized string
            std::cout << "New Configuration: "
                      << Helium::Configuration::getInstance().serialize()
                      << std::endl;
        } else {
            std::cout << "Configuration file not found at " << configFilePath
                      << std::endl;
        }
    });
}

Application::~Application() {
    if (isRunning) {
        Stop();
    }
}

void Application::Start() {
    if (isRunning)
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
    Vector2 scroll = {0, 0};
    Helium::Configuration::getInstance().Formatting.loadFonts();
    GuiSetFont(Helium::Configuration::getInstance().Formatting.DefaultFont);
    Rectangle modalRect = {100, 100, 900, 500};
    OpenFileModal fileOpenModal(modalRect, Helium::Configuration::getInstance().SUPPORTED_NOTE_FILE_TYPE);
    SettingsModal settingsModal(modalRect);

    int filedpwidth = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, "File", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;

    UI::Dropdown fileDropdown({0, 0, 100, Constants::TOP_BAR_MENU_HEIGHT}, Helium::Configuration::getInstance().ColorTheme.Foreground, "File;Open;Save#CTRL+S;Settings");
    fileDropdown.Show();
    UI::StackPanel topBar(UI::Orientation::Horizontal, 5.0f);
    topBar.AddElement(&fileDropdown);

    _noteArea->Initialize(Helium::Configuration::getInstance().TopMenuBarHeight); // Offset the NoteArea 50px down
    while (isRunning) {
        wasModalClosed = isModalOpen; // Assume this as a temporary value
        isModalOpen = fileOpenModal.IsVisible() || _saveModal.IsVisible() || settingsModal.IsVisible();
        wasModalClosed = !isModalOpen && wasModalClosed; // Check if the modal is closed but was open

        if (WindowShouldClose()) {
            Stop();
        }

        // Update
        // --------------------------------------------------------------------------------------------------
        _inputHandler->Update();

        // Modals
        fileOpenModal.Update();
        _saveModal.Update();
        settingsModal.Update();
        topBar.SetPosition({0, 0});

        // Handle updates only if no modal is open
        if (!isModalOpen) {
            _noteArea->Update();

            if (!IsKeyDown(KEY_LEFT_SHIFT))
                scroll.y -= GetMouseWheelMove() * Helium::Configuration::getInstance().Formatting.Paragraph * Helium::Configuration::getInstance().ScrollLineCount;
            if (scroll.y < 0) scroll.y = 0;
            camera.target = {
                GetScreenWidth() * 0.5f + scroll.x,
                GetScreenHeight() * 0.5f + scroll.y,
            };
            _noteArea->SetViewOffset(scroll.y);
            camera.offset = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
        }
        // Draw
        // --------------------------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(Helium::Configuration::getInstance().ColorTheme.Background);

        // NOTE AREA
        // -------------------------------------
        BeginMode2D(camera);
        _noteArea->Draw();
        EndMode2D();
        DrawFPS(0, 0);

        // UI
        // --------------------------------------------------------------------------------------------------
        DrawRectangleRec({0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(Helium::Configuration::getInstance().TopMenuBarHeight)}, Helium::Configuration::getInstance().ColorTheme.Foreground);
        fileOpenModal.Draw();
        settingsModal.Draw();
        _saveModal.Draw();
        topBar.Draw();
        switch (fileDropdown.GetSelected()) {
        case 1: // Open
            fileOpenModal.Show(GetUserRootPath());
            break;
        case 2: // Save
            if (_noteArea->GetPath().empty()) {
                _saveModal.Show();
            }
            break;
        case 3:
            settingsModal.Show();
            break;
        default: break;
        }
        EndDrawing();

        if (wasModalClosed) {
            if (!fileOpenModal.IsVisible() && !fileOpenModal.GetSelectedFile().empty()) {
                if (Utils::IsFile(fileOpenModal.GetSelectedFile())) {
                    _noteArea->TryLoadNote(fileOpenModal.GetSelectedFile());
                }
            }
            if (_saveModal.HasClosed() && !_saveModal.GetFilePath().empty()) {
                _noteArea->SetPath(_saveModal.GetFilePath());
                _noteArea->Save();
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
void Application::OpenSaveModal() {
    _saveModal.Show();
}
}
