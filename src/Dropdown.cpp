#include <vector>
#include <string>
#include "Dropdown.hpp"
#include "Button.hpp"
#include "raylib.h"
#include <sstream>
#include <iostream>
namespace UI {
    Dropdown::Dropdown(Rectangle rect, Color backgroundColor, const std::string& items)
    : _bounds(rect), _backgroundColor(backgroundColor), _active(false), _visible(true) {
    
    // Split the items string by ';' and create buttons
    std::stringstream ss(items);
    std::string item;
    int fontSize = 20;  // Set a reasonable font size for dropdown items
    while (std::getline(ss, item, ';')) {
        if (!item.empty()) {
            Button button(item, fontSize, WHITE, _backgroundColor, {rect.x, rect.y + static_cast<float>(_buttons.size()) * rect.height, rect.width, rect.height});
            _buttons.push_back(button);
        }
    }
}
int Dropdown::Draw() {
    if (!_visible) return -1;
    _buttons[0].Draw();
    
    if(_active) {
        for(int i = 1; i < _buttons.size(); i++) {
            _buttons[i].Draw();
            if(_buttons[i].IsClicked()) {
                _active = false;
                return i;
            }
        }
    }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        _active = CheckCollisionPointRec(GetMousePosition(), _bounds);
    }

    return -1;
}



void Dropdown::SetPosition(Vector2 pos) {
    _bounds.x = pos.x;
    _bounds.y = pos.y;
    for (size_t i = 0; i < _buttons.size(); ++i) {
        _buttons[i].SetPosition({pos.x, pos.y + static_cast<float>(i) * _bounds.height});
    }
}

void Dropdown::SetSize(Vector2 size) {
    _bounds.width = size.x;
    _bounds.height = size.y;
    for (auto& button : _buttons) {
        button.SetSize(size);
    }
}

void Dropdown::Show() {
    _visible = true;
}

void Dropdown::Hide() {
    _visible = false;
}

bool Dropdown::IsVisible() const {
    return _visible;
}
}