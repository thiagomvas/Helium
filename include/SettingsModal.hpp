#ifndef SETTINGSMODAL_HPP
#define SETTINGSMODAL_HPP

#include "raylib.h"
#include <vector>
#include "StackPanel.hpp"
#include "Button.hpp"
class SettingsModal {
    public:
    SettingsModal(Rectangle bounds);

    void Show();
    void Hide();
    void Update();
    void Draw();
    void RegisterSetting(std::string key, std::string value);
    bool IsVisible();

    private:
    Rectangle _modalRect;
    Rectangle _settingListRect;
    bool _visible;
    bool _isDragging;
    int _scrollOffset;
    int _visibleItemCount;
    std::vector<UI::StackPanel*> _settingItems;
    UI::Button closeModalBtn;
    int _settingItemHeight;
};

#endif