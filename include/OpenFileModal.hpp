#ifndef FILEOPENMODAL_H
#define FILEOPENMODAL_H

#include "raylib.h"
#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include "Configuration.hpp"
class OpenFileModal {
public:
    OpenFileModal(const Rectangle& rect );

    const float PATH_BAR_MARGIN = 5;
    const float TOP_BAR_HEIGHT = 25;
    const float WINDOW_PADDING = 5;

    // Methods to show, hide, and render the modal
    void Show(const std::string& rootPath);
    void Hide();
    void Update();
    void Draw();
    // Get the selected file
    std::string GetSelectedFile() const;

    // Check if the modal is currently visible
    bool IsVisible() const;

private:
    Rectangle modalRect;
    Rectangle pathBarRect;
    Rectangle parentFolderButtonRect;
    std::vector<std::string> fileList;
    std::filesystem::path currentPath;
    std::string selectedFile;
    bool isVisible;
    int scrollOffset;
    int visibleItemCount;
    void LoadFilesInDirectory();
    bool isDragging = false;
};

#endif // FILEOPENMODAL_H
