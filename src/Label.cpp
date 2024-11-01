#include "Label.hpp"
#include "Configuration.hpp"
#include "raylib.h"

namespace UI {
Label::Label(std::string text)
    : _text(text), _pos({0, 0}), _fontSize(Helium::Configuration::getInstance().Formatting.Paragraph), _color(Helium::Configuration::getInstance().ColorTheme.TextColor) {
}

Label::Label(std::string text, int fontSize)
    : _text(text), _pos({0, 0}), _fontSize(fontSize), _color(Helium::Configuration::getInstance().ColorTheme.TextColor) {
}

Label::Label(std::string text, int fontSize, Color color)
    : _text(text), _pos({0, 0}), _fontSize(fontSize), _color(color) {
}

void Label::Draw() {
    DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, _text.c_str(), _pos, _fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing, _color);
}
void Label::SetText(std::string text) {
    _text = text;
}

void Label::SetPosition(Vector2 pos) {
    _pos = pos;
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

std::string &Label::GetText() {
    return _text;
}
}