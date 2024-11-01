#include "raylib.h"
#include "Label.hpp"
#include "UI.hpp"
#include "UIElement.hpp"
#include <string>
#ifndef BUTTON_HPP
#define BUTTON_HPP
namespace UI {
class Button : public UIElement {
    public:
    // Constructors for Button class
    Button(std::string text, int fontSize, Color textColor, Color backgroundColor);
    Button(std::string text, int fontSize, Color textColor, Color backgroundColor, Rectangle bounds);

    // Draw method to render button and label
    void Draw() override;

    // Methods to set button properties
    void SetText(std::string text);
    void SetBackgroundColor(Color color);
    void Show();
    void Hide();
    void SetHorizontalAlignment(HorizontalAlignment alignment);
    // Check if the button was clicked
    bool IsClicked();

    private:
    Label _label;           
    Color _backgroundColor; 
    bool _visible;          
    HorizontalAlignment _horizontalAlignment = HorizontalAlignment::Left;
};
}
#endif