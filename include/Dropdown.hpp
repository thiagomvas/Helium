#include "raylib.h"
#include "Button.hpp"
#include "UIElement.hpp"
#include <string>
#include <vector>
#ifndef DROPDOWN_HPP
#define DROPDOWN_HPP

namespace UI {
class Dropdown : public UIElement {
    public:
    Dropdown(Rectangle rect, Color backgroundColor, const std::string &items);

    void Draw() override;
    void SetPosition(Vector2 pos) override;
    void SetSize(Vector2 size) override;

    bool IsVisible() const;
    int GetSelected() const;

    private:
    int _selected;
    Color _backgroundColor;       
    std::vector<Button> _buttons; 
    bool _active;                 
    bool _visible;                
};
}

#endif