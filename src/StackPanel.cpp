#include "StackPanel.hpp"
#include "UIElement.hpp"
#include "raylib.h"
#include <vector>

namespace UI {
StackPanel::StackPanel(Orientation orientation, float spacing)
    : _orientation(orientation), _spacing(spacing) {
}

void StackPanel::AddElement(UIElement *element) {
    _elements.push_back(element);
}

void StackPanel::SetSpacing(float spacing) {
    _spacing = spacing;
}

void StackPanel::SetOrientation(Orientation orientation) {
    _orientation = orientation;
}

void StackPanel::Clear() {
    _elements.clear();
}

void StackPanel::Draw() {
    Vector2 offset = {_bounds.x, _bounds.y};
    for (UIElement *element : _elements) {
        element->SetPosition(offset);
        element->Draw();

        if (_orientation == Orientation::Vertical) {
            offset.y += element->GetBounds().height + _spacing;
        } else {
            offset.x += element->GetBounds().width + _spacing;
        }
    }
}
}