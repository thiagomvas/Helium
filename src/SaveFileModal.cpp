#include "SaveFileModal.hpp"
#include "UiUtils.hpp" // Assuming UiUtils functions are in this file
#include <iostream>

SaveFileModal::SaveFileModal(const Rectangle& rect )
    : modalRect(rect), isVisible(false), hasClosed(false) {}

void SaveFileModal::Show() {
    filePath.clear(); // Clear any previous path
    isVisible = true;
    hasClosed = false; // Reset closed state when showing
}

void SaveFileModal::Hide() {
    isVisible = false;
    hasClosed = true; // Set closed state to true when hiding
}

bool SaveFileModal::IsVisible() const {
    return isVisible;
}

std::string SaveFileModal::GetFilePath() const {
    return filePath;
}

bool SaveFileModal::HasClosed() {
    bool closedState = hasClosed; // Store current state
    hasClosed = false;             // Reset for next frame
    return closedState;            // Return the previous state
}

void SaveFileModal::Update() {
    if (!isVisible) return;

    HandleInput(); // Handle user input for the file path

    // Close modal when clicking outside its bounds
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), modalRect)) {
        Hide();
    }

    // Handle saving logic when the "Save" button is pressed
    if (IsKeyPressed(KEY_ENTER)) {
        Hide(); // Close modal after pressing Enter
    }
}

void SaveFileModal::HandleInput() {
    // Input handling for the file path
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!filePath.empty()) {
            filePath.pop_back(); // Remove last character
        }
    } else {
        int key;
            while ((key = GetCharPressed()) > 0) {
                if (key >= 32) { // Valid character range
                    filePath += (char)key;
                }
            }
    }

    if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
        filePath += GetClipboardText();
    }
}

void SaveFileModal::Draw() {
    if (!isVisible) return;

    // Draw modal background
    DrawRectangleRec(modalRect, Helium::Configuration::getInstance().ColorTheme.Foreground);
    DrawRectangle(modalRect.x, modalRect.y, modalRect.width, Helium::Configuration::getInstance().TopMenuBarHeight, Helium::Configuration::getInstance().ColorTheme.AccentBackground);
    
    // Draw title
    UiUtils::LabelDefault("Save File", { modalRect.x + 10, modalRect.y }, Helium::Configuration::getInstance().ColorTheme.TextColor );

    // Draw the input box for the file path
    UiUtils::LabelDefault("File Path:", { modalRect.x + 10, modalRect.y + 40 }, Helium::Configuration::getInstance().ColorTheme.TextColor );
    UiUtils::LabelDefault(filePath.c_str(), { modalRect.x + 10, modalRect.y + 70 }, Helium::Configuration::getInstance().ColorTheme.TextColor );

    // Draw a "Save" button
    Rectangle saveButton = { modalRect.x + 10, modalRect.y + 110, modalRect.width - 20, 30 };
    if (UiUtils::LabelButton(saveButton, "Save", Helium::Configuration::getInstance().ColorTheme.Foreground )) {
        // Save the file path when the button is pressed
        std::cout << "Save button pressed" << std::endl;
        Hide(); // Close the modal
    }
}
