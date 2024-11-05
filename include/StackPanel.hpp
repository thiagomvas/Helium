#include <vector>
#include "UIElement.hpp"
#include "UI.hpp"
#ifndef STACKPANEL_HPP
#define STACKPANEL_HPP
namespace UI {
    
class StackPanel : public UIElement {
public:
    StackPanel(Orientation orientation = Orientation::Vertical, float spacing = 5.0f);

    void AddElement(UIElement* element);
    UIElement& GetElement(int index) const;

    void Update() override;
    void Draw() override;

    void SetSpacing(float spacing);

    void SetOrientation(Orientation orientation);

    void Clear();
    void ClearAndFree();

private:
    std::vector<UIElement*> _elements; 
    Orientation _orientation;          
    float _spacing;                    
};
}
#endif