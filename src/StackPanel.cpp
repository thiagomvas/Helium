#include "StackPanel.hpp"
#include "UIElement.hpp"
#include "raylib.h"
#include <vector>

namespace UI {
StackPanel::StackPanel(Orientation orientation, float spacing)
    : _orientation(orientation), _spacing(spacing) {
        Show();
}

void StackPanel::AddElement(UIElement *element) {
    _elements.push_back(element);
}

UIElement& StackPanel::GetElement(int index) const {
    return *_elements[index];
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

void StackPanel::ClearAndFree() {
    for (auto element : _elements) {
        delete element; 
    }
    _elements.clear();
}

void StackPanel::Update() {
    for(UIElement *element : _elements) {
        element->Update();
    }
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