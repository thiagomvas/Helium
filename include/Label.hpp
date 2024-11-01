#include <string>
#include "raylib.h"
#include "UIElement.hpp"
#ifndef LABEL_HPP
#define LABEL_HPP

namespace UI {
class Label : public UIElement {
    public:
    Label(std::string text);
    Label(std::string text, int fontSize);
    Label(std::string text, int fontSize, Color color);
    void Draw() override;
    void SetText(std::string text);
    int GetFontSize();
    std::string GetText() const;
    void Show();
    void Hide();

    private:
    std::string _text;
    int _fontSize;
    Color _color;
    bool _visible;
};
}

#endif