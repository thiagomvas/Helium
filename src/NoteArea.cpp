#include "NoteArea.hpp"
#include "raylib.h"
#include "rlgl.h"
#include "constants.h"
#include "tokenizer.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <sstream>



void SaveNote(std::string fileName, std::string text, Texture2D texture) {
    // Open file in binary mode
    std::ofstream file(fileName, std::ios::binary);
    
    // Save string length followed by the string data
    size_t stringLength = text.size();
    file.write(reinterpret_cast<char*>(&stringLength), sizeof(size_t));
    file.write(text.c_str(), stringLength);
    
    // Save texture metadata: width, height, and format
    file.write(reinterpret_cast<char*>(&texture.width), sizeof(int));
    file.write(reinterpret_cast<char*>(&texture.height), sizeof(int));
    file.write(reinterpret_cast<char*>(&texture.format), sizeof(int));

    // Save texture pixel data
    Image image = LoadImageFromTexture(texture); // Extract the texture data as an Image
    ImageFlipVertical(&image);
    file.write(reinterpret_cast<char*>(image.data), image.width * image.height * 4); // Assuming RGBA format
    
    // Clean up
    UnloadImage(image);
    file.close();
    std::cout << "Saved note: " << fileName << std::endl;
}

bool LoadNote(std::string fileName, std::string& text, Texture2D& texture) {
    // Try to open the file in binary mode
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return false;
    }

    // Read string length and string data
    size_t stringLength;
    file.read(reinterpret_cast<char*>(&stringLength), sizeof(size_t));
    if (file.fail()) {
        std::cerr << "Failed to read string length." << std::endl;
        return false;
    }
    
    char* buffer = new char[stringLength + 1];
    file.read(buffer, stringLength);
    if (file.fail()) {
        std::cerr << "Failed to read string data." << std::endl;
        delete[] buffer;
        return false;
    }
    buffer[stringLength] = '\0';
    text = std::string(buffer);
    delete[] buffer;

    // Read texture metadata (width, height, format)
    int width, height, format;
    file.read(reinterpret_cast<char*>(&width), sizeof(int));
    file.read(reinterpret_cast<char*>(&height), sizeof(int));
    file.read(reinterpret_cast<char*>(&format), sizeof(int));
    if (file.fail()) {
        std::cerr << "Failed to read texture metadata." << std::endl;
        return false;
    }

    // Allocate memory for the pixel data and read it
    unsigned char* pixelData = new unsigned char[width * height * 4]; // Assuming RGBA format
    file.read(reinterpret_cast<char*>(pixelData), width * height * 4);
    if (file.fail()) {
        std::cerr << "Failed to read pixel data." << std::endl;
        delete[] pixelData;
        return false;
    }

    // Create the texture from the pixel data
    Image image = { pixelData, width, height, 1, format };
    texture = LoadTextureFromImage(image);

    // Clean up
    delete[] pixelData;
    file.close();

    return true;
}


namespace Helium {

void NoteArea::Initialize() {
    Texture2D temp;
    if(LoadNote("C:/Users/Thiago/picasso.note", _rawText, temp))
    {
        _tokens = _tokenizer.tokenize(_rawText);
        _texture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        BeginTextureMode(_texture);
        DrawTexture(temp, 0, 0, WHITE);
        EndTextureMode();
        UnloadTexture(temp);
    }
    else
        _texture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    rlSetBlendFactorsSeparate(0x0302, 0x0303, 1, 0x0303, 0x8006, 0x8006);
}

void NoteArea::Update() {
    int wheel;
    if (CheckCollisionPointRec(GetMousePosition(), _rect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        switch(_mode) {
            case Helium::NoteMode::READ:
                _mode = Helium::NoteMode::WRITE;
                break;
            case Helium::NoteMode::WRITE:
                _mode = Helium::NoteMode::READ;
                _tokens = _tokenizer.tokenize(_rawText);
            default:
                break;
        }
    }
    if((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_S)) {
        SaveNote("C:/Users/Thiago/picasso.note", _rawText, _texture.texture);
    }
    if(IsKeyPressed(KEY_LEFT_ALT))
    {
        if(_mode == Helium::NoteMode::DRAW)
            _mode = Helium::NoteMode::READ;
        else
            _mode = Helium::NoteMode::DRAW;
    }

    switch(_mode) {
        case Helium::NoteMode::READ:
            break;
        case Helium::NoteMode::WRITE:
            if(IsKeyPressed(KEY_BACKSPACE))
            {
                _beginDeleteTime = 0;
                _rawText.pop_back();
            }

            if(IsKeyDown(KEY_BACKSPACE) && !_rawText.empty()) {
                _beginDeleteTime += GetFrameTime();
                if(_beginDeleteTime >= 0.5) {
                    _rawText.pop_back();
                }
            }

            if(IsKeyPressed(KEY_ENTER)) {
                _rawText += '\n';
            }
            // Read characters instead of individual key presses
            int key;
            while ((key = GetCharPressed()) > 0) {
                if (key >= 32) {
                    _rawText += (char)key;
                }
            }
    
            break;
        case Helium::NoteMode::DRAW:
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                BeginTextureMode(_texture);
                DrawCircle(GetMouseX(), GetMouseY(), _brushRadius, PINK);
                EndTextureMode();
            }
            if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                BeginTextureMode(_texture);
                BeginBlendMode(BLEND_CUSTOM);
                DrawCircle(GetMouseX(), GetMouseY(), _brushRadius, BLANK);
                EndBlendMode();
                EndTextureMode();
            }
            
            wheel = GetMouseWheelMove();
            if(wheel != 0)
            {
                _brushRadius -= wheel;
            }

        default:
            break;
    }
}


void NoteArea::Draw() {
    DrawRectangleRec(_rect, Colors::FOREGROUND);

    int y = 0;
    const int fontSize = 16;  // Fixed font size for all lines

    std::istringstream stream(_rawText);
    std::string line;
    int caretX = 0, caretY = 0;  // Caret position

    static bool showCaret = true;         // Flag for caret visibility (blinking)
    static float caretBlinkTimer = 0.0f;  // Timer to control caret blinking

    const float caretBlinkInterval = 0.5f;  // Blink interval for the caret

    caretBlinkTimer += GetFrameTime();
    if (caretBlinkTimer >= caretBlinkInterval) {
        showCaret = !showCaret;  // Toggle visibility
        caretBlinkTimer = 0.0f;  // Reset timer
    }

    switch (_mode) {
        default:
        case Helium::NoteMode::READ:
            for (const Token& t : _tokens) {
                switch (t.type) {
                    case Helium::TokenType::HEADER:
                        DrawText(t.value.c_str(), 0, y, Formatting::GetFontSizeForHeader(stoi(t.attributes.at("level"))), BLACK);
                        y += Formatting::GetFontSizeForHeader(stoi(t.attributes.at("level")));
                        break;
                    default:
                        DrawText(t.value.c_str(), 0, y, Formatting::PARAGRAPH, BLACK);
                        y += Formatting::PARAGRAPH;
                        break;
                }
            }
            break;

        case Helium::NoteMode::WRITE: {
            bool lastCharWasNewline = false;

            // Loop through each line of text and draw it
            while (std::getline(stream, line)) {
                DrawText(line.c_str(), 0, y, fontSize, BLACK);

                // Set caret position to the end of the last line
                caretX = MeasureText(line.c_str(), fontSize);  // Calculate width of the text for X
                caretY = y;  // Y is the current line

                y += fontSize;  // Move to next line position
            }

            // Handle case where the last character is a newline
            if (!_rawText.empty() && _rawText.back() == '\n') {
                caretX = 0;  // Start at the new line
                caretY += fontSize;  // Move caret down
                lastCharWasNewline = true;
            }

            // Draw caret only if it's visible (blinking)
            if (showCaret) {
                DrawRectangle(caretX, caretY, 2, fontSize, BLACK);  // Thin vertical caret
            }
            break;
        }
    }

    // Draw the texture
    DrawTextureRec(_texture.texture, { 0, 0, static_cast<float>(_texture.texture.width), -static_cast<float>(_texture.texture.height) }, { 0.0f, 0.0f }, WHITE);

    // Handle draw mode (if applicable)
    if (_mode == Helium::NoteMode::DRAW) {
        DrawCircleLines(GetMouseX(), GetMouseY(), _brushRadius, BLACK);
    }
}



void NoteArea::SetRect(Rectangle rect) {
    _rect = rect;
}
void NoteArea::UpdateText() {}
} // namespace Helium
