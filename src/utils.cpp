#include "utils.hpp"
#include "raylib.h"
#include <sstream>
#include <string>

namespace Utils {


std::string Serialize(Color color) {
	return std::to_string(color.r) + ',' +
		   std::to_string(color.g) + ',' +
		   std::to_string(color.b) + ',' +
		   std::to_string(color.a);
}
Color DeserializeRaylibColor(std::string& colorString) {
    Color color = { 0, 0, 0, 255 }; // Default color (black with full alpha)

    // Use a single parse operation
    int r, g, b, a;
    if (sscanf(colorString.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a) == 4) {
        color.r = static_cast<unsigned char>(r);
        color.g = static_cast<unsigned char>(g);
        color.b = static_cast<unsigned char>(b);
        color.a = static_cast<unsigned char>(a);
    }

    return color;
}
}
