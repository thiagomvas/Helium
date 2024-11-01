#include "OpenFileModal.hpp"
#include "UiUtils.hpp" // Assuming UiUtils functions are in this file
#include "utils.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

OpenFileModal::OpenFileModal(const Rectangle &rect)
    : modalRect(rect), isVisible(false), scrollOffset(0) {
}

void OpenFileModal::Show(const std::string &rootPath) {
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
    fileList.clear();

    if (fs::exists(currentPath) && fs::is_directory(currentPath)) {
        if (currentPath != fs::path("/")) {
            fileList.push_back("..");
        }

        for (const auto &entry : fs::directory_iterator(currentPath)) {
            fileList.push_back(entry.path().filename().string());
        }
    } else {
        std::cerr << "Directory does not exist: " << currentPath << std::endl;
    }
}

void OpenFileModal::Update() {
    float offset = GetMouseY() - modalRect.y;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), modalRect) && offset > 0 && offset < TOP_BAR_HEIGHT) {
        isDragging = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDragging = false;
    }
    if (isDragging) {
        Vector2 delta = GetMouseDelta();
        modalRect.x += delta.x;
        modalRect.y += delta.y;
    }
}

void OpenFileModal::Draw() {
    // Modal
    DrawRectangleRounded(modalRect, 0.1, 8, Helium::Configuration::getInstance().ColorTheme.Background);
    DrawRectangleRounded({modalRect.x, modalRect.y, modalRect.width, TOP_BAR_HEIGHT}, 1, 8, Helium::Configuration::getInstance().ColorTheme.AccentBackground);
    DrawRectangleRec({modalRect.x, modalRect.y + TOP_BAR_HEIGHT * 0.5f, modalRect.width, TOP_BAR_HEIGHT * 0.5f}, Helium::Configuration::getInstance().ColorTheme.AccentBackground);
    DrawRectangleRoundedLinesEx(modalRect, 0.1, 8, 2, Helium::Configuration::getInstance().ColorTheme.AccentBackground);
    Utils::DrawText("    Open File...", {modalRect.x, modalRect.y});
    
    // Path bar
    float height = Helium::Configuration::getInstance().Formatting.GetLineHeight(Helium::Configuration::getInstance().Formatting.Paragraph) + 10;
    float barWidth = (modalRect.width - 2 * WINDOW_PADDING) * 0.75f;
    float buttonWidth = (modalRect.width - 2 * WINDOW_PADDING) * 0.25f - PATH_BAR_MARGIN;
    pathBarRect = {modalRect.x + WINDOW_PADDING, modalRect.y + TOP_BAR_HEIGHT + PATH_BAR_MARGIN, barWidth, height};
    parentFolderButtonRect = {modalRect.x + WINDOW_PADDING + barWidth + PATH_BAR_MARGIN, modalRect.y + TOP_BAR_HEIGHT + PATH_BAR_MARGIN, buttonWidth, height};
    DrawRectangleRounded(pathBarRect, 0.25, 8, Helium::Configuration::getInstance().ColorTheme.Foreground);
    DrawRectangleRounded(parentFolderButtonRect, 0.25, 8, Helium::Configuration::getInstance().ColorTheme.Foreground);
    Utils::DrawText(".. <", {parentFolderButtonRect.x, parentFolderButtonRect.y});
    
    // Path text rendering with ellipsis if it overflows
    std::string pathText = currentPath.string();
    int fontSize = Helium::Configuration::getInstance().Formatting.Paragraph;
    int pathWidth = MeasureText(pathText.c_str(), fontSize);
    
    if (pathWidth > barWidth) {
        std::string ellipsis = "...";
        int ellipsisWidth = MeasureText(ellipsis.c_str(), fontSize);

        for (size_t i = 0; i < pathText.size(); ++i) {
            std::string subPath = ellipsis + pathText.substr(i);
            if (MeasureText(subPath.c_str(), fontSize) <= barWidth) {
                pathText = subPath;
                break;
            }
        }
    }

    Utils::DrawText(pathText.c_str(), {modalRect.x + WINDOW_PADDING, modalRect.y + TOP_BAR_HEIGHT + PATH_BAR_MARGIN});

    // Files
}

