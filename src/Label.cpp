#include "Label.hpp"
#include "Configuration.hpp"
#include "raylib.h"

namespace UI {
Label::Label(std::string text)
    : _text(text), _fontSize(Helium::Configuration::getInstance().Formatting.Paragraph), _color(Helium::Configuration::getInstance().ColorTheme.TextColor) {
    Vector2 size = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, text.c_str(), _fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing);
    SetBounds({0, 0, size.x, size.y});
}

Label::Label(std::string text, int fontSize)
    : _text(text), _fontSize(fontSize), _color(Helium::Configuration::getInstance().ColorTheme.TextColor) {
    Vector2 size = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, text.c_str(), _fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing);
    SetBounds({0, 0, size.x, size.y});
}

Label::Label(std::string text, int fontSize, Color color)
    : _text(text), _fontSize(fontSize), _color(color) {
    Vector2 size = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, text.c_str(), _fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing);
    SetBounds({0, 0, size.x, size.y});
}

void Label::Draw() {
    DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, _text.c_str(), {_bounds.x, _bounds.y}, _fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing, _color);
}
void Label::SetText(std::string text) {
    _text = text;
    Vector2 size = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, text.c_str(), _fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing);
    SetSize(size);
}

void Label::Show() {
    _visible = true;
}
void Label::Hide() {
    _visible = false;
}

int Label::GetFontSize() {
    return _fontSize;
}

std::string Label::GetText() const {
    return _text;
}
}