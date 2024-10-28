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
    OpenFileModal(const Rectangle& rect, std::shared_ptr<Helium::Configuration> config);

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
    std::vector<std::string> fileList;
    std::filesystem::path currentPath;
    std::string selectedFile;
    bool isVisible;
    std::shared_ptr<Helium::Configuration> config;
    void LoadFilesInDirectory();
};

#endif // FILEOPENMODAL_H
