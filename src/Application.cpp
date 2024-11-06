#include "Application.hpp"
#include "Atom.hpp"
#include "Bond.hpp"
#include "Configuration.hpp"
#include "InputCombo.hpp"
#include "Molecule.hpp"
#include "NoteArea.hpp"
#include "UiUtils.hpp"
#include "Window.hpp"
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
#include "NoteWindow.hpp"
#include "OpenFileModal.hpp"
#include "SettingsModal.hpp"
#include "StackPanel.hpp"
#include "raygui.h"
#include <MoleculeWindow.hpp>
#include <SaveFileModal.hpp>
namespace Helium {

Application::Application()
    : isRunning(false), _inputHandler(std::make_unique<InputHandler>()), _noteArea(std::make_unique<NoteArea>()), _saveModal({100, 100, 400, 300}, Helium::Configuration::getInstance().SUPPORTED_NOTE_FILE_TYPE) {
    _noteArea->SetMode(NoteMode::READ);
    _inputHandler->SetMode(NoteMode::READ);
    auto atom1 = std::make_shared<Atom>("Atom 1", "Content A");
    auto atom2 = std::make_shared<Atom>("Atom 2", "Content B");
    auto atom3 = std::make_shared<Atom>("Atom 3", "Content C");
    auto atom4 = std::make_shared<Atom>("Atom 4", "Content D");
    auto atom5 = std::make_shared<Atom>("Atom 5", "Content E"); // Adding an extra atom for branching

    // Create bonds to connect the atoms
    auto bond1 = std::make_shared<Bond>(atom1, atom2); // bond between atom1 and atom2
    auto bond2 = std::make_shared<Bond>(atom2, atom3); // bond between atom2 and atom3
    auto bond3 = std::make_shared<Bond>(atom3, atom4); // bond between atom3 and atom4
    auto bond4 = std::make_shared<Bond>(atom2, atom5); // bond between atom2 and atom5 (branching)

    // Add bonds to each atom (tree-like structure)
    atom1->AddBond(bond1); // Atom 1 connected to Atom 2
    atom2->AddBond(bond1); // Atom 2 connected to Atom 1
    atom2->AddBond(bond2); // Atom 2 connected to Atom 3
    atom3->AddBond(bond2); // Atom 3 connected to Atom 2
    atom3->AddBond(bond3); // Atom 3 connected to Atom 4
    atom4->AddBond(bond3); // Atom 4 connected to Atom 3
    atom2->AddBond(bond4); // Atom 2 connected to Atom 5 (branching point)
    atom5->AddBond(bond4); // Atom 5 connected to Atom 2

    // Create a molecule and add the atoms and bonds
    _loadedMolecule = std::make_shared<Molecule>("Molecule Project");
    _loadedMolecule->AddAtom(atom1);
    _loadedMolecule->AddAtom(atom2);
    _loadedMolecule->AddAtom(atom3);
    _loadedMolecule->AddAtom(atom4);
    _loadedMolecule->AddAtom(atom5); // Adding Atom 5 to the molecule

    _loadedMolecule->AddBond(bond1);
    _loadedMolecule->AddBond(bond2);
    _loadedMolecule->AddBond(bond3);
    _loadedMolecule->AddBond(bond4); // Adding the bond between Atom 2 and Atom 5

    // GLOBAL ACTIONS
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

    // Initialize raygui
    GuiLoadStyle("styles/raygui-dark");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Adjust text size

    bool isModalOpen = false;
    bool wasModalClosed = false;

    NoteWindow noteWindow;
    MoleculeWindow moleculeWindow(_loadedMolecule);

    Window *window = &moleculeWindow;

    float menuHeight = Helium::Configuration::getInstance().TopMenuBarHeight;
    Vector2 scroll = {0, 0};
    Helium::Configuration::getInstance().Formatting.loadFonts();
    GuiSetFont(Helium::Configuration::getInstance().Formatting.DefaultFont);
    Rectangle modalRect = {100, 100, 900, 500};
    OpenFileModal fileOpenModal(modalRect, Helium::Configuration::getInstance().SUPPORTED_NOTE_FILE_TYPE);
    SettingsModal settingsModal(modalRect);

    int filedpwidth = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, "File", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;

    UI::Dropdown fileDropdown({0, 0, 100, Constants::TOP_BAR_MENU_HEIGHT}, Helium::Configuration::getInstance().ColorTheme.Foreground, "File;New Molecule;Open;Save#CTRL+S;Settings");
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
            window->Update();
        }
        // Draw
        // --------------------------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(Helium::Configuration::getInstance().ColorTheme.Background);

        // NOTE AREA
        // -------------------------------------
        window->Draw();
        DrawFPS(0, 0);

        // UI
        // --------------------------------------------------------------------------------------------------
        DrawRectangleRec({0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(Helium::Configuration::getInstance().TopMenuBarHeight)}, Helium::Configuration::getInstance().ColorTheme.Foreground);
        fileOpenModal.Draw();
        settingsModal.Draw();
        _saveModal.Draw();
        topBar.Draw();
        switch (fileDropdown.GetSelected()) {
        case 1:
            _loadedMolecule = std::make_shared<Molecule>("New Molecule");
            break;
        case 2: // Open
            fileOpenModal.Show(GetUserRootPath());
            break;
        case 3: // Save
            if (_noteArea->GetPath().empty()) {
                _saveModal.Show();
            }
            break;
        case 4:
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
std::shared_ptr<Molecule> Application::GetLoadedMolecule() {
    return _loadedMolecule;
}
}
