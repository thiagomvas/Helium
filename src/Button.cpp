#include "Button.hpp"
#include <string>
#include "UI.hpp"
#include "Label.hpp"

namespace UI {
    Button::Button(std::string text, int fontSize, Color textColor, Color backgroundColor)
    : _label(text, fontSize, textColor), _backgroundColor(backgroundColor), _visible(true) {
    _bounds = {0, 0, 100, 50}; // Default size, can be changed later
}

Button::Button(std::string text, int fontSize, Color textColor, Color backgroundColor, Rectangle bounds)
    : _label(text, fontSize, textColor), _backgroundColor(backgroundColor),  _visible(true) {
        SetBounds(bounds);
    }

void Button::Draw() {
    if (!_visible) return;

    // Draw button background
    DrawRectangleRec(_bounds, _backgroundColor);

    // Adjust label's horizontal position based on alignment
    float labelX = _bounds.x;
    float textWidth = MeasureText(_label.GetText().c_str(), _label.GetFontSize());

    switch (_horizontalAlignment) {
        case HorizontalAlignment::Center:
            labelX += (_bounds.width - textWidth) / 2;
            break;
        case HorizontalAlignment::Left:
            labelX += 10; 
            break;
        case HorizontalAlignment::Right:
            labelX += _bounds.width - textWidth - 10; 
            break;
    }

    Vector2 labelPosition = {
        labelX,
        _bounds.y + (_bounds.height - _label.GetFontSize()) / 2
    };
    _label.SetPosition(labelPosition);
    _label.Draw();
}

std::string Button::GetText() const {
    return _label.GetText();
}

void Button::SetText(std::string text) {
    _label.SetText(text);
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

bool Button::IsClicked() const {
    return _visible && CheckCollisionPointRec(GetMousePosition(), _bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
void Button::SetHorizontalAlignment(HorizontalAlignment alignment) {
    _horizontalAlignment = alignment;
}
}