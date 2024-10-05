#include "Configuration.hpp"
#include "raylib.h"
#include <memory>
#ifndef UIUTILS_HPP
#define UIUTILS_HPP

namespace UiUtils {

int Button(Rectangle rect, Color tint) {
	DrawRectangleRec(rect, tint);
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		return CheckCollisionPointRec(GetMousePosition(), rect);
	return 0;
}
void LabelDefault(const char* text, Vector2 pos, Color color, std::shared_ptr<Helium::Configuration> config) {
	
	DrawTextEx(config->Formatting.DefaultFont, text, pos, config->Formatting.Paragraph, config->Formatting.CharSpacing, color);
}

int LabelButton(Rectangle rect, const char* text, Color rectColor, std::shared_ptr<Helium::Configuration> config) {
	int button = Button(rect, rectColor);

	LabelDefault(text, {rect.x, rect.y}, config->ColorTheme.TextColor, config);
	return button;
}


int Dropdown(Rectangle rect, Color tint, const std::string& items, std::shared_ptr<Helium::Configuration> config, int* active) {
    std::vector<std::string> itemList;

    // String stream to help split the items by ';'
    std::stringstream ss(items);
    std::string item;

    // Split the string using ';' as a delimiter
    while (std::getline(ss, item, ';')) {
        // Only add non-empty items to the list
        if (!item.empty()) {
            itemList.push_back(item);
        }
    }

    // Render the first item (dropdown header) using LabelButton
    if (LabelButton(rect, itemList.front().c_str(), tint, config)) {
        *active = 1;  
    }

    // If dropdown is active, render the rest of the items as LabelButtons
    if (*active) {
        for (int i = 1; i < itemList.size(); ++i) {
            Rectangle itemRect = { rect.x, rect.y + (rect.height * i), rect.width, rect.height };

            DrawRectangleRec(itemRect, tint); 
            
            // Split the item by '#' if it exists
            std::string mainText;
            std::string rightText;
            size_t hashPos = itemList[i].find('#');
            if (hashPos != std::string::npos) {
                mainText = itemList[i].substr(0, hashPos);   // Text before '#'
                rightText = itemList[i].substr(hashPos + 1); // Text after '#'
            } else {
                mainText = itemList[i]; // If no '#' found, use the whole item as main text
            }

            
            Vector2 rightTextSize = MeasureTextEx(config->Formatting.DefaultFont, rightText.c_str(), config->Formatting.Paragraph, config->Formatting.CharSpacing);
            float rightTextX = itemRect.x + itemRect.width - rightTextSize.x - 5; // Position with a small padding
            Vector2 rightTextPosition = { rightTextX, itemRect.y }; // Adding padding
            
            if (Button(itemRect, tint)) { 
                *active = 0; 
                return i;
            }
            LabelDefault(mainText.c_str(), { itemRect.x, itemRect.y }, config->ColorTheme.TextColor, config); // Adding padding
			DrawTextEx(config->Formatting.DefaultFont, rightText.c_str(), rightTextPosition, config->Formatting.Paragraph, config->Formatting.CharSpacing, config->ColorTheme.SecondaryTextColor);

        }
    }

    // Check for clicks outside the dropdown to collapse it
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), rect)) {
        *active = 0;  // Collapse dropdown
    }

    // If no item is pressed, return 0
    return 0;
}

}

#endif
