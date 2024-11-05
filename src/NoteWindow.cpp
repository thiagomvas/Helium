#include "NoteWindow.hpp"
#include "Configuration.hpp"
#include "Application.hpp"
#include "NoteArea.hpp"
#include "InputCombo.hpp"
#include <InputHandler.hpp>

NoteWindow::NoteWindow() 
    : _inputHandler(std::make_unique<Helium::InputHandler>()), _noteArea(std::make_unique<Helium::NoteArea>()) {
    _noteArea->SetMode(Helium::NoteMode::READ);
    _inputHandler->SetMode(Helium::NoteMode::READ);

    _inputHandler->AddGlobalAction(Helium::InputCombo(KEY_ESCAPE), [this]() {
        _noteArea->SetMode(Helium::NoteMode::READ);
        _inputHandler->SetMode(Helium::NoteMode::READ);
    });

    camera = {0};
    camera.target = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    camera.offset = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    _noteArea->Initialize(Helium::Configuration::getInstance().TopMenuBarHeight);
}

void NoteWindow::Update() {
    _inputHandler->Update();
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

void NoteWindow::Draw() {
    DrawRectangleRec({(GetScreenWidth() - Helium::Configuration::getInstance().MaxNoteWidth) * 0.5f, 0, static_cast<float>(Helium::Configuration::getInstance().MaxNoteWidth), static_cast<float>(GetScreenHeight())}, Helium::Configuration::getInstance().ColorTheme.Foreground);
    BeginMode2D(camera);
    _noteArea->Draw();
    EndMode2D();
}
