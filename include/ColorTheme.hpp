#ifndef COLORTHEME_HPP
#define COLORTHEME_HPP

#include "raylib.h"
#include "Serializer.hpp"
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Helium {
class ColorTheme {
public: 
    Color Background = { 12, 12, 12, 255 };
    Color AccentBackground = { 50, 50, 50, 255 };
    Color Foreground = { 20, 20, 20, 255 };
    Color AccentForeground = { 200, 100, 100, 255 };
    Color TextColor = { 200, 200, 200, 255 };
    Color SecondaryTextColor = { 150, 150, 150, 255 };
    Color BrushBorder = { 200, 200, 200, 255 };
    Color TextHighlight = { 37, 150, 190, 128 };
    Color CodeTextColor = { 150, 150, 150, 255 };
    Color CodeBackgroundColor = { 40, 40, 40, 255 };
    Color QuoteDefault = { 246, 248, 250, 255 }; 
    Color QuoteNote = { 31, 110, 234, 255 };   
    Color QuoteTip = { 31, 110, 234, 255 };    
    Color QuoteImportant = { 137, 86, 228, 255 }; 
    Color QuoteWarning = { 158, 106, 3, 255 }; 
    Color QuoteCaution = { 219, 54, 50, 255 }; 
    Color HorizontalLineColor = { 40, 40, 40, 255 };
    Color ListItemBulletColor = { 200, 100, 100, 255 };
    Color TodoBackgroundColor = { 8, 108, 52, 255 };
    Color TodoForegroundColor = { 200, 200, 200, 255 };
    Color ButtonHoverColor = { 200, 100, 100, 255 };

    std::string serialize();
    Color getQuoteColor(const std::string& type);
    void deserialize(const std::string& data);
};
}

#endif
