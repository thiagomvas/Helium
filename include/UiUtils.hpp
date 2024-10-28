#ifndef UI_UTILS_HPP
#define UI_UTILS_HPP

#include <raylib.h>
#include <memory>
#include <string>

namespace UiUtils {
    int Button(Rectangle rect, Color tint);
    void LabelDefault(const char* text, Vector2 pos, Color color, std::shared_ptr<Helium::Configuration> config);
    int LabelButton(Rectangle rect, const char* text, Color rectColor, std::shared_ptr<Helium::Configuration> config);
    int Dropdown(Rectangle rect, Color tint, const std::string& items, std::shared_ptr<Helium::Configuration> config, int* active);
}

#endif // UI_UTILS_HPP
