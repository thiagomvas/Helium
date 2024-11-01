#ifndef FILEOPENMODAL_H
#define FILEOPENMODAL_H

#include "raylib.h"
#include "Button.hpp"
#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include "Button.hpp"
#include "Configuration.hpp"
class OpenFileModal {
public:
    OpenFileModal(const Rectangle& rect );
    OpenFileModal(const Rectangle& rect, const std::string& fileFilter );

    const float PATH_BAR_MARGIN = 5;
    const float TOP_BAR_HEIGHT = 25;
    const float WINDOW_PADDING = 5;
    const float FILE_ITEM_HEIGHT = 25;

    // Methods to show, hide, and render the modal
    void Show(const std::string& rootPath);
    void Hide();
    void Update();
    void Draw();
    // Get the selected file
    std::string GetSelectedFile() const;

    // Check if the modal is currently visible
    bool IsVisible() const;

    void SetFilter(const std::string& filter);

private:
    Rectangle modalRect;
    Rectangle pathBarRect;
    Rectangle parentFolderButtonRect;
    Rectangle fileListRect;
    Rectangle selectedFileNameRect;
    Rectangle confirmSelectionButtonRect;
    std::vector<std::string> fileList;
    std::vector<UI::Button> fileButtons;
    std::filesystem::path currentPath;
    std::string selectedFile;
    std::string unconfirmedSelectedFile;
    UI::Button selectFileBtn;
    UI::Button parentFolderBtn;
    UI::Button closeModalBtn;
    std::string _fileFilter;
    bool isVisible;
    int scrollOffset;
    int visibleItemCount;
    void LoadFilesInDirectory();
    bool isDragging = false;
};

#endif // FILEOPENMODAL_H
