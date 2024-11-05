#include "InputBox.hpp"
#include "raylib.h"
#include "utils.hpp"
#include <iostream>
namespace UI {
InputBox::InputBox() {
    _bounds = {0, 0, 100, 50};
    Show();
}
InputBox::InputBox(const std::string &placeholder)
    : _placeholder(placeholder), _cursor(), _inputHandler(), _focused(false), _value(std::make_shared<std::string>()) {
    _bounds = {0, 0, 100, 50};
    _cursor.SetTextPter(_value, nullptr);
    _inputHandler.AddGlobalAction(Helium::InputCombo(KEY_BACKSPACE, 0, Helium::InputEvent::Down), [this]() {
        if (!_value->empty())
            _value->pop_back();
    });
    Show();
}
std::string InputBox::GetText() const {
    return *_value;
}
void InputBox::SetText(const std::string &text) {
    *_value = text;
}
void InputBox::SetPlaceholder(const std::string &placeholder) {
    _placeholder = placeholder;
}
void InputBox::Update() {
    if(!_visible) return;

    bool isOnBounds = CheckCollisionPointRec(GetMousePosition(), _bounds);

    if(isOnBounds) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    } else {
        SetMouseCursor(MOUSE_CURSOR_ARROW);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (isOnBounds) {
            Focus();
        } else {
            Unfocus();
        }
    }

    if (_focused) {
        int key;

        while ((key = GetCharPressed()) > 0) {
            if (key >= 32) {
                *_value += (char)key;
            }
        }
    }
}
void InputBox::Draw() {
    if(!_visible) return;
    DrawRectangleRec(_bounds, _backgroundColor);
    if (!_value->empty() || _focused)
        Utils::DrawText(*_value, {_bounds.x, _bounds.y});
    else
        Utils::DrawText(_placeholder, {_bounds.x, _bounds.y});
}

void InputBox::Focus() {
    _focused = true;
    _value->clear();
}
void InputBox::Unfocus() {
    _focused = false;
}
void InputBox::SetColor(Color color) {
    _backgroundColor = color;
}
}