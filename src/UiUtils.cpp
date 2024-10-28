#include <raylib.h>
#include <memory>
#include <Configuration.hpp>
#include "UiUtils.hpp"
namespace UiUtils {
    int Button(Rectangle rect, Color tint) {
	DrawRectangleRec(rect, tint);
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		return CheckCollisionPointRec(GetMousePosition(), rect);
	return 0;
}
void LabelDefault(const char* text, Vector2 pos, Color color ) {
	
	DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, text, pos, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, color);
}

int LabelButton(Rectangle rect, const char* text, Color rectColor ) {
	int button = Button(rect, rectColor);

	LabelDefault(text, {rect.x, rect.y}, Helium::Configuration::getInstance().ColorTheme.TextColor);
	return button;
}


int Dropdown(Rectangle rect, Color tint, const std::string& items , int* active) {
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
    if (LabelButton(rect, itemList.front().c_str(), tint )) {
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

            
            Vector2 rightTextSize = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, rightText.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing);
            float rightTextX = itemRect.x + itemRect.width - rightTextSize.x - 5; // Position with a small padding
            Vector2 rightTextPosition = { rightTextX, itemRect.y }; // Adding padding
            
            if (Button(itemRect, tint)) { 
                *active = 0; 
                return i;
            }
            LabelDefault(mainText.c_str(), { itemRect.x, itemRect.y }, Helium::Configuration::getInstance().ColorTheme.TextColor ); // Adding padding
			DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, rightText.c_str(), rightTextPosition, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.SecondaryTextColor);

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