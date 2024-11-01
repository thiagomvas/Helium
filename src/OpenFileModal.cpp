#include "OpenFileModal.hpp"
#include "Button.hpp"
#include "UiUtils.hpp" // Assuming UiUtils functions are in this file
#include "utils.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_set>

namespace fs = std::filesystem;

OpenFileModal::OpenFileModal(const Rectangle &rect)
    : modalRect(rect),
      isVisible(false),
      scrollOffset(0),
      parentFolderBtn(".. <", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().ColorTheme.TextColor, Helium::Configuration::getInstance().ColorTheme.Foreground), 
      selectFileBtn("Select", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().ColorTheme.TextColor, Helium::Configuration::getInstance().ColorTheme.Foreground), 
      closeModalBtn("X", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().ColorTheme.TextColor, BLANK) {
}

OpenFileModal::OpenFileModal(const Rectangle &rect, const std::string &fileFilter)
    : OpenFileModal(rect) {
    _fileFilter = fileFilter;
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

void OpenFileModal::SetFilter(const std::string &filter) {
    _fileFilter = filter;
}

std::string OpenFileModal::GetSelectedFile() const {
    return selectedFile;
}

void OpenFileModal::LoadFilesInDirectory() {
    fileList.clear();
    fileButtons.clear();

    // Parse fileFilter into a set of extensions if it is not empty
    std::unordered_set<std::string> allowedExtensions;
    if (!_fileFilter.empty()) {
        std::istringstream filterStream(_fileFilter);
        std::string extension;
        while (std::getline(filterStream, extension, ',')) {
            // Trim whitespace around extensions (optional, for robustness)
            extension.erase(0, extension.find_first_not_of(" \t"));
            extension.erase(extension.find_last_not_of(" \t") + 1);
            allowedExtensions.insert("." + extension); // Prefix with '.' for consistency
        }
    }

    if (fs::exists(currentPath) && fs::is_directory(currentPath)) {
        std::vector<UI::Button> directories;
        std::vector<UI::Button> files;

        for (const auto &entry : fs::directory_iterator(currentPath)) {
            std::string entryName = entry.path().filename().string();
            UI::Button button(
                entryName,
                Helium::Configuration::getInstance().Formatting.Paragraph,
                Helium::Configuration::getInstance().ColorTheme.TextColor,
                Helium::Configuration::getInstance().ColorTheme.Foreground);

            if (fs::is_directory(entry)) {
                button.SetText(entryName + "/"); // Add "/" to indicate a directory
                directories.push_back(button);
            } else {
                // If fileFilter is specified, check the extension
                if (allowedExtensions.empty() ||
                    allowedExtensions.count(entry.path().extension().string()) > 0) {
                    files.push_back(button);
                }
            }
        }

        // Add directories first, followed by files
        fileButtons.insert(fileButtons.end(), directories.begin(), directories.end());
        fileButtons.insert(fileButtons.end(), files.begin(), files.end());
    } else {
        std::cerr << "Directory does not exist: " << currentPath << std::endl;
    }
}

void OpenFileModal::Update() {
    if (!isVisible) return;
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

    // Handle scrolling input
    if (IsKeyPressed(KEY_DOWN) && scrollOffset < static_cast<int>(fileList.size()) - visibleItemCount) {
        scrollOffset++;
    }
    if (IsKeyPressed(KEY_UP && scrollOffset > 0)) {
        scrollOffset--;
    }

    if (CheckCollisionPointRec(GetMousePosition(), modalRect))
        scrollOffset -= std::clamp(static_cast<int>(GetMouseWheelMove()), -1, 1);

    scrollOffset = std::clamp(scrollOffset, 0, static_cast<int>(fileButtons.size()));

    float y = pathBarRect.y + pathBarRect.height;
    for (size_t i = scrollOffset; i < scrollOffset + visibleItemCount && i < fileButtons.size(); ++i) {
        UI::Button b = fileButtons[i];
        b.SetPosition({modalRect.x, y});
        if (b.IsClicked()) {
            std::string text = b.GetText();
            if (!text.empty() && text.back() == '/')
                text.pop_back();
            fs::path selected = currentPath / text;
            if (fs::is_directory(selected)) {
                currentPath = selected;
                LoadFilesInDirectory();
                scrollOffset = 0;
            } else {
                unconfirmedSelectedFile = selected.string();
            }
        }
        y += b.GetBounds().height;
    }

    if (selectFileBtn.IsClicked()) {
        selectedFile = unconfirmedSelectedFile;
        unconfirmedSelectedFile.clear();
        Hide();
    }

    if (closeModalBtn.IsClicked()) {
        unconfirmedSelectedFile.clear();
        selectedFile.clear();
        Hide();
    }

    if (parentFolderBtn.IsClicked()) {
        currentPath = currentPath.parent_path();
        LoadFilesInDirectory();
        scrollOffset = 0;
    }
}

void OpenFileModal::Draw() {
    if (!isVisible) return;
    // Modal
    DrawRectangleRounded(modalRect, 0.1, 8, Helium::Configuration::getInstance().ColorTheme.Background);                                                                            // Modal Background
    DrawRectangleRounded({modalRect.x, modalRect.y, modalRect.width, TOP_BAR_HEIGHT}, 1, 8, Helium::Configuration::getInstance().ColorTheme.AccentBackground);                      // Modal top bar
    DrawRectangleRec({modalRect.x, modalRect.y + TOP_BAR_HEIGHT * 0.5f, modalRect.width, TOP_BAR_HEIGHT * 0.5f}, Helium::Configuration::getInstance().ColorTheme.AccentBackground); // Modal Top bar (straight bottom)
    DrawRectangleRoundedLinesEx(modalRect, 0.1, 8, 2, Helium::Configuration::getInstance().ColorTheme.AccentBackground);                                                            // Modal border
    Utils::DrawText("    Open File...", {modalRect.x, modalRect.y});                                                                                                                // Modal title
    closeModalBtn.SetBounds({modalRect.x + modalRect.width - WINDOW_PADDING - 25, modalRect.y, 25, 25});
    closeModalBtn.Draw();

    // Path bar
    float height = Helium::Configuration::getInstance().Formatting.GetLineHeight(Helium::Configuration::getInstance().Formatting.Paragraph) + 10;
    float barWidth = (modalRect.width - 2 * WINDOW_PADDING) * 0.75f;
    float buttonWidth = (modalRect.width - 2 * WINDOW_PADDING) * 0.25f - PATH_BAR_MARGIN;
    pathBarRect = {modalRect.x + WINDOW_PADDING, modalRect.y + TOP_BAR_HEIGHT + PATH_BAR_MARGIN, barWidth, height};
    parentFolderButtonRect = {modalRect.x + WINDOW_PADDING + barWidth + PATH_BAR_MARGIN, modalRect.y + TOP_BAR_HEIGHT + PATH_BAR_MARGIN, buttonWidth, height};
    parentFolderBtn.SetBounds(parentFolderButtonRect);
    parentFolderBtn.Draw();

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

    // Bottom bar
    selectedFileNameRect = {modalRect.x + WINDOW_PADDING + PATH_BAR_MARGIN, modalRect.y + modalRect.height - WINDOW_PADDING - height - PATH_BAR_MARGIN, barWidth, height};
    DrawRectangleRounded(selectedFileNameRect, 0.25, 8, Helium::Configuration::getInstance().ColorTheme.Foreground);
    Utils::DrawText(unconfirmedSelectedFile, {selectedFileNameRect.x + WINDOW_PADDING, selectedFileNameRect.y + WINDOW_PADDING});
    confirmSelectionButtonRect = {modalRect.x + WINDOW_PADDING + barWidth + PATH_BAR_MARGIN + PATH_BAR_MARGIN, modalRect.y + modalRect.height - WINDOW_PADDING - height - PATH_BAR_MARGIN, buttonWidth - WINDOW_PADDING, height};
    selectFileBtn.SetBounds(confirmSelectionButtonRect);
    selectFileBtn.Draw();

    fileListRect = {modalRect.x + WINDOW_PADDING, pathBarRect.y + pathBarRect.height + PATH_BAR_MARGIN, modalRect.width - 2 * WINDOW_PADDING, selectedFileNameRect.y - (pathBarRect.y + pathBarRect.height + 2 * PATH_BAR_MARGIN)};

    visibleItemCount = static_cast<int>(fileListRect.height) / FILE_ITEM_HEIGHT;

    for (size_t i = scrollOffset; i < scrollOffset + visibleItemCount && i < fileButtons.size(); ++i) {
        Rectangle itemRect = {fileListRect.x, fileListRect.y + (i - scrollOffset) * FILE_ITEM_HEIGHT, fileListRect.width, FILE_ITEM_HEIGHT};

        fileButtons[i].SetBounds(itemRect);
        fileButtons[i].Draw();
    }
}
