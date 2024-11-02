#include "Button.hpp"
#include "Configuration.hpp"
#include "Label.hpp"
#include "UI.hpp"
#include <string>

namespace UI {
Button::Button(std::string text, int fontSize, Color textColor, Color backgroundColor)
    : _label(text, fontSize, textColor), _backgroundColor(backgroundColor), _hoverTime(0) {
    _bounds = {0, 0, 100, 50}; // Default size, can be changed later
    Show();
}

Button::Button(std::string text, int fontSize, Color textColor, Color backgroundColor, Rectangle bounds)
    : _label(text, fontSize, textColor), _backgroundColor(backgroundColor) {
    SetBounds(bounds);
    Show();
}

void Button::Draw() {
    if (!_visible) return;

    bool isHovered = CheckCollisionPointRec(GetMousePosition(), _bounds);
    Color hoverColor = Helium::Configuration::getInstance().ColorTheme.ButtonHoverColor;
    float dur = Helium::Configuration::getInstance().HoverAnimationDurationSeconds;

    if (isHovered) {
        _hoverTime += GetFrameTime();
        if (_hoverTime > dur) _hoverTime = dur;
    } else {
        _hoverTime -= GetFrameTime();
        if (_hoverTime < 0.0f) _hoverTime = 0.0f;
    }

    float t = _hoverTime / dur;
    Color currentColor = {
        static_cast<unsigned char>(_backgroundColor.r + t * (hoverColor.r - _backgroundColor.r)),
        static_cast<unsigned char>(_backgroundColor.g + t * (hoverColor.g - _backgroundColor.g)),
        static_cast<unsigned char>(_backgroundColor.b + t * (hoverColor.b - _backgroundColor.b)),
        static_cast<unsigned char>(_backgroundColor.a + t * (hoverColor.a - _backgroundColor.a))};

    DrawRectangleRec(_bounds, currentColor);

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
        _bounds.y + (_bounds.height - _label.GetFontSize()) / 2};
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

bool Button::IsClicked() const {
    return _visible && CheckCollisionPointRec(GetMousePosition(), _bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
void Button::SetHorizontalAlignment(HorizontalAlignment alignment) {
    _horizontalAlignment = alignment;
}
}