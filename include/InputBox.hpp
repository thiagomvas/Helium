#ifndef INPUTBOX_HPP
#define INPUTBOX_HPP

#include "UIElement.hpp"
#include "raylib.h"
#include "Cursor.hpp"
#include "InputHandler.hpp"
#include <string>
namespace UI {
class InputBox : public UIElement {
    public:
    InputBox();
    InputBox(const std::string &placeholder);

    std::string GetText() const;
    void SetText(const std::string &text);
    void SetPlaceholder(const std::string &placeholder);

    void Update();
    void Draw() override;

    void Focus();
    void Unfocus();

    void SetColor(Color color);

    private:
    std::string _placeholder;
    std::shared_ptr<std::string> _value;
    bool _focused;
    Color _backgroundColor;
    Helium::Cursor _cursor;
    Helium::InputHandler _inputHandler;
};
}

#endif