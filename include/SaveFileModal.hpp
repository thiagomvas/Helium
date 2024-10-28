#ifndef SAVEFILEMODAL_H
#define SAVEFILEMODAL_H

#include "raylib.h"
#include <memory>
#include <string>
#include "Configuration.hpp"

class SaveFileModal {
public:
    SaveFileModal(const Rectangle& rect, std::shared_ptr<Helium::Configuration> config);

    void Show();             // Show the modal
    void Hide();             // Hide the modal
    bool IsVisible() const;  // Check if the modal is visible
    std::string GetFilePath() const; // Get the saved file path
    bool HasClosed();
    void Update();           // Update modal logic
    void Draw();             // Draw the modal

private:
    Rectangle modalRect;
    std::string filePath;    // Variable to hold the user-entered file path
    bool isVisible;    
    bool hasClosed;      
    std::shared_ptr<Helium::Configuration> config;

    void HandleInput();      // Handle user input for the file path
};

#endif // SAVEFILEMODAL_H
