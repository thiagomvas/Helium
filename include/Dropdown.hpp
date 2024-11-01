#include "raylib.h"
#include "Button.hpp"
#include <string>
#include <vector>
#ifndef DROPDOWN_HPP
#define DROPDOWN_HPP

namespace UI {
class Dropdown {
    public:
    Dropdown(Rectangle rect, Color backgroundColor, const std::string &items);

    int Draw();

    void SetPosition(Vector2 pos);
    void SetSize(Vector2 size);
    void Show();
    void Hide();
    bool IsVisible() const;

    private:
    Rectangle _bounds;            
    Color _backgroundColor;       
    std::vector<Button> _buttons; 
    bool _active;                 
    bool _visible;                
};
}

#endif