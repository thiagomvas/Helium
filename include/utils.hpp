#ifndef UTILS_HPP
#define UTILS_HPP

#include "raylib.h"
#include <string>
namespace Utils {

std::string Serialize(Color color);
Color DeserializeRaylibColor(std::string& colorString);
}

#endif

