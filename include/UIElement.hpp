#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

#include "raylib.h"

namespace UI {
class UIElement {
    public:
    virtual void Draw() {}
    virtual void Update() {}
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
    virtual void Show() {
        _visible = true;
    }
    virtual void Hide() {
        _visible = false;
    }
    protected:
    Rectangle _bounds;
    bool _visible = true;
};
}

#endif