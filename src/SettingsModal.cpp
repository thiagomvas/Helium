#include "SettingsModal.hpp"
#include "Button.hpp"
#include "Configuration.hpp"
#include "InputBox.hpp"
#include "Label.hpp"
#include "StackPanel.hpp"
#include "UI.hpp"
#include "constants.h"
#include "raylib.h"
#include "utils.hpp"
#include <algorithm>
#include <vector>
#include <filesystem>
#include <fstream>
#include "DataPath.hpp"
SettingsModal::SettingsModal(Rectangle bounds)
    : _modalRect(bounds),
      _visible(false),
      _scrollOffset(0),
      _settingItemHeight(25 + Constants::MODAL_PADDING),
      closeModalBtn("X", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().ColorTheme.TextColor, BLANK),
      resetSettingsBtn("Reset", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().ColorTheme.TextColor, BLANK),
      _settingListRect({bounds.x + Constants::MODAL_PADDING, bounds.y + Constants::TOP_BAR_MENU_HEIGHT + Constants::MODAL_PADDING, bounds.width - 2 * Constants::MODAL_PADDING, bounds.height - Constants::TOP_BAR_MENU_HEIGHT - 2 * Constants::MODAL_PADDING}) {
}

void SettingsModal::RegisterSetting(std::string key, std::string value) {
    UI::StackPanel *sp = new UI::StackPanel(UI::Orientation::Horizontal, 50);
    UI::Label *l = new UI::Label(key);
    UI::InputBox *b = new UI::InputBox(value);
    b->SetText(value);
    b->SetColor(Helium::Configuration::getInstance().ColorTheme.Foreground);
    b->SetBounds({0, 0, (value == "" ? 0.0f : _modalRect.width - l->GetBounds().width - 50 - Constants::MODAL_PADDING * 2), 25});
    sp->AddElement(l);
    sp->AddElement(b);
    _settingItems.push_back(sp);
}

void SettingsModal::Show() {
    _visible = true;    
    for (auto e : _settingItems) {
        e->ClearAndFree();
        delete e;
    }
    _settingItems.clear();
    for (const auto &[key, value] : Helium::Configuration::getInstance().serializeIntoMap()) {
        if (key.contains("MACROS")) break;
        if (key.contains("COLORTHEME") || key.contains("FORMATTING"))
            RegisterSetting("", "");
        RegisterSetting(key, value);
    }
}

void SettingsModal::Hide() {
    _visible = false;
    std::string newConfig;
    for (auto e : _settingItems) {
        auto *label = dynamic_cast<UI::Label *>(&e->GetElement(0));
        auto *box = dynamic_cast<UI::InputBox *>(&e->GetElement(1));
        if (box->GetText() == "")
            newConfig += label->GetText() + "\n";
        else
            newConfig += label->GetText() + ":" + box->GetText() + "\n";
        e->ClearAndFree();
        delete e;
    }
    Helium::Configuration::getInstance().deserialize(newConfig);
    std::string appDataPath = GetAppDataPath();
    std::filesystem::path heliumFolderPath = appDataPath + "/Helium";
    if (!std::filesystem::exists(heliumFolderPath)) {
        std::filesystem::create_directories(heliumFolderPath);
    }

    std::filesystem::path filePath = heliumFolderPath / "config.txt";

    std::ofstream configFile(filePath);
    configFile << Helium::Configuration::getInstance().serialize();
    configFile.close();
    _settingItems.clear();
}

void SettingsModal::Update() {
    if (!_visible) return;
    float offset = GetMouseY() - _modalRect.y;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), _modalRect) && offset > 0 && offset < Constants::TOP_BAR_MENU_HEIGHT) {
        _isDragging = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        _isDragging = false;
    }
    if (_isDragging) {
        Vector2 delta = GetMouseDelta();
        _modalRect.x += delta.x;
        _modalRect.y += delta.y;
    }

    if (IsKeyPressed(KEY_DOWN) && _scrollOffset < static_cast<int>(_settingItems.size()) - _visibleItemCount) {
        _scrollOffset++;
    }
    if (IsKeyPressed(KEY_UP && _scrollOffset > 0)) {
        _scrollOffset--;
    }

    if (CheckCollisionPointRec(GetMousePosition(), _modalRect))
        _scrollOffset -= std::clamp(static_cast<int>(GetMouseWheelMove()), -1, 1);

    _scrollOffset = std::clamp(_scrollOffset, 0, static_cast<int>(_settingItems.size()));

    for (size_t i = _scrollOffset; i < _scrollOffset + _visibleItemCount && i < _settingItems.size(); ++i) {
        _settingItems[i]->Update();
    }

    if (closeModalBtn.IsClicked()) {
        Hide();
    }

    if(resetSettingsBtn.IsClicked()) {
        Helium::Configuration::getInstance().resetDefault();
        Show();
    }
}

void SettingsModal::Draw() {
    if (!_visible) return;

    DrawRectangleRounded(_modalRect, 0.1, 8, Helium::Configuration::getInstance().ColorTheme.Background);                                                                                                              // Modal Background
    DrawRectangleRounded({_modalRect.x, _modalRect.y, _modalRect.width, Constants::TOP_BAR_MENU_HEIGHT}, 1, 8, Helium::Configuration::getInstance().ColorTheme.AccentBackground);                                      // Modal top bar
    DrawRectangleRec({_modalRect.x, _modalRect.y + Constants::TOP_BAR_MENU_HEIGHT * 0.5f, _modalRect.width, Constants::TOP_BAR_MENU_HEIGHT * 0.5f}, Helium::Configuration::getInstance().ColorTheme.AccentBackground); // Modal Top bar (straight bottom)
    DrawRectangleRoundedLinesEx(_modalRect, 0.1, 8, 2, Helium::Configuration::getInstance().ColorTheme.AccentBackground);                                                                                              // Modal border
    Utils::DrawText("    Settings", {_modalRect.x, _modalRect.y});                                                                                                                                                 // Modal title
    closeModalBtn.SetBounds({_modalRect.x + _modalRect.width - Constants::MODAL_PADDING - 25, _modalRect.y, 25, 25});
    closeModalBtn.Draw();
    resetSettingsBtn.SetBounds({_modalRect.x + _modalRect.width - Constants::MODAL_PADDING - 125, _modalRect.y, 100, 25});
    resetSettingsBtn.Draw();

    _settingListRect.x = _modalRect.x + Constants::MODAL_PADDING;
    _settingListRect.y = _modalRect.y + Constants::TOP_BAR_MENU_HEIGHT + Constants::MODAL_PADDING;

    _visibleItemCount = static_cast<int>(_settingListRect.height / _settingItemHeight);

    for (size_t i = _scrollOffset; i < _scrollOffset + _visibleItemCount && i < _settingItems.size(); ++i) {
        Rectangle itemRect = {_settingListRect.x, _settingListRect.y + (i - _scrollOffset) * _settingItemHeight, _settingListRect.width, Constants::FILE_ITEM_HEIGHT};
        _settingItems[i]->SetBounds(itemRect);
        _settingItems[i]->Draw();
    }
}

bool SettingsModal::IsVisible() {
    return _visible;
}
