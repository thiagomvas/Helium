#include "SaveFileModal.hpp"
#include "Button.hpp"
#include "DataPath.hpp"
#include "UiUtils.hpp" // Assuming UiUtils functions are in this file
#include "constants.h"
#include "utils.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_set>
namespace fs = std::filesystem;

SaveFileModal::SaveFileModal(const Rectangle &rect)
    : modalRect(rect),
      isVisible(false),
      hasClosed(false),
      scrollOffset(0),
      parentFolderBtn(".. <", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().ColorTheme.TextColor, Helium::Configuration::getInstance().ColorTheme.Foreground),
      saveFileBtn("Save", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().ColorTheme.TextColor, Helium::Configuration::getInstance().ColorTheme.Foreground),
      closeModalBtn("X", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().ColorTheme.TextColor, BLANK),
      saveFileInputBox("File name...") {
        saveFileInputBox.SetColor(Helium::Configuration::getInstance().ColorTheme.Foreground);
}
SaveFileModal::SaveFileModal(const Rectangle &rect, const std::string &fileFilter)
    : SaveFileModal(rect) {
    _fileFilter = fileFilter;
}
void SaveFileModal::Show() {
    currentPath = GetUserRootPath();
    isVisible = true;
    hasClosed = false; // Reset closed state when showing
    fileButtons = Utils::LoadFilesInDirectoryAsButtons(currentPath, _fileFilter);
}

void SaveFileModal::Hide() {
    isVisible = false;
    hasClosed = true; // Set closed state to true when hiding
}

bool SaveFileModal::IsVisible() const {
    return isVisible;
}

std::string SaveFileModal::GetFilePath() const {
    return selectedFile;
}

bool SaveFileModal::HasClosed() {
    bool closedState = hasClosed; // Store current state
    hasClosed = false;            // Reset for next frame
    return closedState;           // Return the previous state
}

void SaveFileModal::Update() {
    if (!isVisible) return;

    saveFileInputBox.Update();

    float offset = GetMouseY() - modalRect.y;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), modalRect) && offset > 0 && offset < Constants::TOP_BAR_MENU_HEIGHT) {
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

    if (IsKeyPressed(KEY_DOWN) && scrollOffset < static_cast<int>(fileList.size()) - visibleItemCount) {
        scrollOffset++;
    }
    if (IsKeyPressed(KEY_UP && scrollOffset > 0)) {
        scrollOffset--;
    }

    if (CheckCollisionPointRec(GetMousePosition(), modalRect))
        scrollOffset -= std::clamp(static_cast<int>(GetMouseWheelMove()), -1, 1);

    scrollOffset = std::clamp(scrollOffset, 0, static_cast<int>(fileButtons.size()));

    for (size_t i = scrollOffset; i < scrollOffset + visibleItemCount && i < fileButtons.size(); ++i) {
        UI::Button b = fileButtons[i];
        if (b.IsClicked()) {
            std::string text = b.GetText();
            if (!text.empty() && text.back() == '/')
                text.pop_back();
            fs::path selected = currentPath / text;
            if (fs::is_directory(selected)) {
                currentPath = selected;
                fileButtons = Utils::LoadFilesInDirectoryAsButtons(currentPath, _fileFilter);
                scrollOffset = 0;
            } else {
                saveFileInputBox.SetText(selected.string());
            }
        }
    }

    if (saveFileBtn.IsClicked()) {
        selectedFile = saveFileInputBox.GetText();

        fs::path filePath(selectedFile);
        if (filePath.is_relative()) {
            selectedFile = (currentPath / filePath).string();
        }

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
        fileButtons = Utils::LoadFilesInDirectoryAsButtons(currentPath, _fileFilter);
        scrollOffset = 0;
    }
}

void SaveFileModal::Draw() {
    if (!isVisible) return;

    DrawRectangleRounded(modalRect, 0.1, 8, Helium::Configuration::getInstance().ColorTheme.Background);                                                                                                            // Modal Background
    DrawRectangleRounded({modalRect.x, modalRect.y, modalRect.width, Constants::TOP_BAR_MENU_HEIGHT}, 1, 8, Helium::Configuration::getInstance().ColorTheme.AccentBackground);                                      // Modal top bar
    DrawRectangleRec({modalRect.x, modalRect.y + Constants::TOP_BAR_MENU_HEIGHT * 0.5f, modalRect.width, Constants::TOP_BAR_MENU_HEIGHT * 0.5f}, Helium::Configuration::getInstance().ColorTheme.AccentBackground); // Modal Top bar (straight bottom)
    DrawRectangleRoundedLinesEx(modalRect, 0.1, 8, 2, Helium::Configuration::getInstance().ColorTheme.AccentBackground);                                                                                            // Modal border
    Utils::DrawText("    Save File...", {modalRect.x, modalRect.y});                                                                                                                                                // Modal title
    closeModalBtn.SetBounds({modalRect.x + modalRect.width - Constants::MODAL_PADDING - 25, modalRect.y, 25, 25});
    closeModalBtn.Draw();

    float height = Helium::Configuration::getInstance().Formatting.GetLineHeight(Helium::Configuration::getInstance().Formatting.Paragraph) + 10;
    float barWidth = (modalRect.width - 2 * Constants::MODAL_PADDING) * 0.75f;
    float buttonWidth = (modalRect.width - 2 * Constants::MODAL_PADDING) * 0.25f - Constants::TOP_BAR_MENU_HEIGHT;
    pathBarRect = {modalRect.x + Constants::MODAL_PADDING, modalRect.y + Constants::TOP_BAR_MENU_HEIGHT + Constants::TOP_BAR_MENU_HEIGHT, barWidth, height};
    parentFolderButtonRect = {modalRect.x + Constants::MODAL_PADDING + barWidth + Constants::PATH_BAR_MARGIN, modalRect.y + Constants::TOP_BAR_MENU_HEIGHT + Constants::PATH_BAR_MARGIN, buttonWidth, height};
    parentFolderBtn.SetBounds(parentFolderButtonRect);
    parentFolderBtn.Draw();
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

    Utils::DrawText(pathText.c_str(), {modalRect.x + Constants::MODAL_PADDING, modalRect.y + Constants::TOP_BAR_MENU_HEIGHT + Constants::PATH_BAR_MARGIN});

    // Bottom bar
    selectedFileNameRect = {modalRect.x + Constants::MODAL_PADDING + Constants::PATH_BAR_MARGIN, modalRect.y + modalRect.height - Constants::MODAL_PADDING - height - Constants::PATH_BAR_MARGIN, barWidth, height};
    saveFileInputBox.SetBounds(selectedFileNameRect);
    saveFileInputBox.Draw();
    Utils::DrawText(unconfirmedSelectedFile, {selectedFileNameRect.x + Constants::MODAL_PADDING, selectedFileNameRect.y + Constants::MODAL_PADDING});
    confirmSelectionButtonRect = {modalRect.x + Constants::MODAL_PADDING + barWidth + Constants::PATH_BAR_MARGIN + Constants::PATH_BAR_MARGIN, modalRect.y + modalRect.height - Constants::MODAL_PADDING - height - Constants::PATH_BAR_MARGIN, modalRect.width - selectedFileNameRect.width - 2 * Constants::MODAL_PADDING - 2 * Constants::PATH_BAR_MARGIN, height};
    saveFileBtn.SetBounds(confirmSelectionButtonRect);
    saveFileBtn.Draw();

    fileListRect = {modalRect.x + Constants::MODAL_PADDING, pathBarRect.y + pathBarRect.height + Constants::PATH_BAR_MARGIN, modalRect.width - 2 * Constants::MODAL_PADDING, selectedFileNameRect.y - (pathBarRect.y + pathBarRect.height + 2 * Constants::PATH_BAR_MARGIN)};

    visibleItemCount = static_cast<int>(fileListRect.height / Constants::FILE_ITEM_HEIGHT);

    for (size_t i = scrollOffset; i < scrollOffset + visibleItemCount && i < fileButtons.size(); ++i) {
        Rectangle itemRect = {fileListRect.x, fileListRect.y + (i - scrollOffset) * Constants::FILE_ITEM_HEIGHT, fileListRect.width, Constants::FILE_ITEM_HEIGHT};

        fileButtons[i].SetBounds(itemRect);
        fileButtons[i].Draw();
    }
}

void SaveFileModal::SetBounds(const Rectangle &rect) {
    modalRect = rect;
}
