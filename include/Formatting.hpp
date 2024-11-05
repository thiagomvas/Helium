#ifndef FORMATTING_HPP
#define FORMATTING_HPP

#include "raylib.h"
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Helium {
class Formatting {
public:
    Formatting();

    int H1 = 48;
    int H2 = 40;
    int H3 = 32;
    int H4 = 26;
    int H5 = 22;
    int Paragraph = 20;
    int StrikethroughWidth = 2;
    std::string DefaultFontPath = "resources/Roboto-Bold.ttf";
    std::string BoldFontPath = "resources/Roboto-Black.ttf";
    std::string ItalicFontPath = "resources/Roboto-BoldItalic.ttf";
    std::string CodeFontPath = "resources/Roboto-Regular.ttf";
    float CharSpacing = 1;
    int HorizontalLineThickness = 2;
    float QuoteOffset = 25;
    float ListItemOffset = 25;

    // Loaded during runtime
    // !! DO NOT SERIALIZE !!
    Font DefaultFont;
    Font BoldFont;
    Font ItalicFont;
    Font CodeFont;
    float TodoOffset;

    int GetLineHeight(int fontSize);
    int GetFontSizeForHeader(int level);
    void loadFonts();
    std::string serialize() const;
    void deserialize(const std::string& data);
    void unloadResources();

private:
    std::unordered_map<int, int> lineHeightMap;
}; 
}

#endif
