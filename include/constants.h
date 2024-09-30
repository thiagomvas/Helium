#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "raylib.h"

namespace Config {
    constexpr const char* APP_NAME = "Placeholder";
}

namespace Formatting {
    constexpr int H1 = 48;
    constexpr int H2 = 40;
    constexpr int H3 = 32;
    constexpr int H4 = 26;
    constexpr int H5 = 22;
    constexpr int PARAGRAPH = 20;

    constexpr int GetFontSizeForHeader(int level) {
        switch (level) {
            case 1: return H1;
            case 2: return H2;
            case 3: return H3;
            case 4: return H4;
            case 5: return H5;
            default: return PARAGRAPH;  // Return paragraph size for invalid or non-header levels
        }
    }
}

namespace Colors {
    constexpr Color BACKGROUND = { 12, 12, 12, 255 };
}
#endif

