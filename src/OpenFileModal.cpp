#include "OpenFileModal.hpp"
#include "UiUtils.hpp" // Assuming UiUtils functions are in this file
#include <filesystem>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

OpenFileModal::OpenFileModal(const Rectangle& rect )
    : modalRect(rect), isVisible(false), scrollOffset(0) {}

void OpenFileModal::Show(const std::string& rootPath) {
    currentPath = fs::path(rootPath); // Store the root path as a fs::path
    selectedFile.clear();    
    LoadFilesInDirectory();  
    isVisible = true;
}

void OpenFileModal::Hide() {
    isVisible = false;
}

bool OpenFileModal::IsVisible() const {
    return isVisible;
}

std::string OpenFileModal::GetSelectedFile() const {
    return selectedFile;
}

void OpenFileModal::LoadFilesInDirectory() {
    fileList.clear(); // Clear previous file list

    // Check if the current path exists
    if (fs::exists(currentPath) && fs::is_directory(currentPath)) {
        // Add ".." to go back to the parent directory
        if (currentPath != fs::path("/")) { // Check if the path is not root
            fileList.push_back("..");
        }

        // Iterate through the directory and add files and directories to the list
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            fileList.push_back(entry.path().filename().string());
        }
    } else {
        // Optional: Handle the case where the directory does not exist
        std::cerr << "Directory does not exist: " << currentPath << std::endl;
    }
}

void OpenFileModal::Update() {
    if (!isVisible) return;

    // Handle scrolling input
    if (IsKeyPressed(KEY_DOWN) && scrollOffset < static_cast<int>(fileList.size()) - visibleItemCount) {
        scrollOffset++;
    }
    if (IsKeyPressed(KEY_UP && scrollOffset > 0)) {
        scrollOffset--;
    }

    if(CheckCollisionPointRec(GetMousePosition(), modalRect))
        scrollOffset -= std::clamp(static_cast<int>(GetMouseWheelMove()), -1, 1);
    
    scrollOffset = std::clamp(scrollOffset, 0, static_cast<int>(fileList.size()));

    // Update modal logic, such as detecting button clicks
    for (size_t i = scrollOffset; i < scrollOffset + visibleItemCount && i < fileList.size(); ++i) {
        Rectangle itemRect = { modalRect.x + 10, modalRect.y + 50 + ((i - scrollOffset) * 35), modalRect.width - 20, 30 };
        
        if (UiUtils::LabelButton(itemRect, fileList[i].c_str(), Helium::Configuration::getInstance().ColorTheme.Foreground )) {
            // Handle directory navigation
            if (fileList[i] == "..") {
                // Go to the parent directory
                currentPath = currentPath.parent_path(); // Update to parent path
                LoadFilesInDirectory(); // Reload files in the new directory
                scrollOffset = 0; // Reset scroll offset
            } else {
                // Check if the selected item is a directory
                fs::path selectedPath = currentPath / fileList[i];
                if (fs::is_directory(selectedPath)) {
                    currentPath = selectedPath; // Update to the selected directory
                    LoadFilesInDirectory(); // Load files in the selected directory
                    scrollOffset = 0; // Reset scroll offset
                } else {
                    selectedFile = selectedPath.string(); // Store the selected file
                    Hide(); // Close the modal after selection
                }
            }
        }
    }

    // Close modal when clicking outside its bounds
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), modalRect)) {
        Hide();
    }
}

void OpenFileModal::Draw() {
    if (!isVisible) return;

    // Draw modal background
    DrawRectangleRec(modalRect, Helium::Configuration::getInstance().ColorTheme.Foreground);
    DrawRectangle(modalRect.x, modalRect.y, modalRect.width, Helium::Configuration::getInstance().TopMenuBarHeight, Helium::Configuration::getInstance().ColorTheme.AccentBackground);
    
    // Draw title
    UiUtils::LabelDefault("Open File", { modalRect.x + 10, modalRect.y }, Helium::Configuration::getInstance().ColorTheme.TextColor );

    // Calculate the number of visible items based on the modal height
    visibleItemCount = (modalRect.height - 50) / 35; // Adjust based on item height (35 in this case)

    // Draw each file in the modal as a button, taking scrolling into account
    for (size_t i = scrollOffset; i < scrollOffset + visibleItemCount && i < fileList.size(); ++i) {
        Rectangle itemRect = { modalRect.x + 10, modalRect.y + 50 + ((i - scrollOffset) * 35), modalRect.width - 20, 30 };
        UiUtils::LabelButton(itemRect, fileList[i].c_str(), Helium::Configuration::getInstance().ColorTheme.Foreground );
    }
}
