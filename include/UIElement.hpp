#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

#include "raylib.h"

namespace UI {
class UIElement {
    public:
    virtual void Draw() {}
    Rectangle GetBounds() {
        return _bounds;
    }
    virtual void SetBounds(Rectangle rect) {
        _bounds = rect;
    }
    virtual void SetPosition(Vector2 pos) {
        _bounds.x = pos.x;
        _bounds.y = pos.y;
    }
    virtual void SetSize(Vector2 size) {
        _bounds.width = size.x;
        _bounds.height = size.y;
    }
    protected:
    Rectangle _bounds;
};
}

#endif