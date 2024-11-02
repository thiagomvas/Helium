#ifndef SAVEFILEMODAL_H
#define SAVEFILEMODAL_H

#include "raylib.h"
#include "Button.hpp"
#include "InputBox.hpp"
#include <memory>
#include <filesystem>
#include <string>
#include "Configuration.hpp"

class SaveFileModal {
public:
    SaveFileModal(const Rectangle& rect );
    SaveFileModal(const Rectangle& rect, const std::string& fileFilter );

    void Show();             // Show the modal
    void Hide();             // Hide the modal
    bool IsVisible() const;  // Check if the modal is visible
    std::string GetFilePath() const; // Get the saved file path
    bool HasClosed();
    void Update();           // Update modal logic
    void Draw();             // Draw the modal

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
    UI::Button saveFileBtn;
    UI::Button parentFolderBtn;
    UI::Button closeModalBtn;
    UI::InputBox saveFileInputBox;
    std::string _fileFilter;
    int scrollOffset;
    bool isVisible;    
    int visibleItemCount;
    bool hasClosed;      
    bool isDragging;
     

    void HandleInput();      // Handle user input for the file path
};

#endif // SAVEFILEMODAL_H
