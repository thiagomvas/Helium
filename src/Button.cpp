#include "Button.hpp"
#include <string>
#include "Label.hpp"

namespace UI {
    Button::Button(std::string text, int fontSize, Color textColor, Color backgroundColor)
    : _label(text, fontSize, textColor), _backgroundColor(backgroundColor), _visible(true) {
    _bounds = {0, 0, 100, 50}; // Default size, can be changed later
}

Button::Button(std::string text, int fontSize, Color textColor, Color backgroundColor, Rectangle bounds)
    : _label(text, fontSize, textColor), _backgroundColor(backgroundColor), _bounds(bounds), _visible(true) {}

void Button::Draw() {
    if (!_visible) return;

    // Draw button background
    DrawRectangleRec(_bounds, _backgroundColor);

    // Center the label within the button
    Vector2 labelPosition = {
        _bounds.x + (_bounds.width - MeasureText(_label.GetText().c_str(), _label.GetFontSize())) / 2,
        _bounds.y + (_bounds.height - _label.GetFontSize()) / 2
    };
    _label.SetPosition(labelPosition);
    _label.Draw();
}

void Button::SetText(std::string text) {
    _label.SetText(text);
}

void Button::SetPosition(Vector2 pos) {
    _bounds.x = pos.x;
    _bounds.y = pos.y;
}

void Button::SetSize(Vector2 size) {
    _bounds.width = size.x;
    _bounds.height = size.y;
}

void Button::SetBackgroundColor(Color color) {
    _backgroundColor = color;
}

void Button::Show() {
    _visible = true;
}

void Button::Hide() {
    _visible = false;
}

bool Button::IsClicked() {
    return _visible && CheckCollisionPointRec(GetMousePosition(), _bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
}