#include <string>
#include "raylib.h"
#ifndef LABEL_HPP
#define LABEL_HPP

namespace UI {
class Label {
    public:
    Label(std::string text);
    Label(std::string text, int fontSize);
    Label(std::string text, int fontSize, Color color);
    void Draw();
    void SetText(std::string text);
    void SetPosition(Vector2 pos);
    int GetFontSize();
    std::string &GetText();
    void Show();
    void Hide();

    private:
    std::string _text;
    Vector2 _pos;
    int _fontSize;
    Color _color;
    bool _visible;
};
}

#endif