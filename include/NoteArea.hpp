#ifndef NOTEAREA_HPP
#define NOTEAREA_HPP

#include "NoteMode.hpp"
#include "InputHandler.hpp"
#include "cursor.hpp"
#include "raylib.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "tokenizer.hpp"
#include "constants.h"
#include "Configuration.hpp"

void SaveNote(std::string fileName, std::string text, Texture2D texture);

bool LoadNote(std::string fileName, std::string& text, Texture2D& texture);
namespace Helium {
class NoteArea {
public:
    NoteArea(std::shared_ptr<Configuration> config, std::shared_ptr<InputHandler> input);
    void Initialize(int heightOffset);
    void Update();
    void Save();
    void Draw();
    std::string GetFormattedText();
    void SetRect(Rectangle rect);
    void SetMode(NoteMode mode);
private:
    RenderTexture2D _texture;
    std::string _rawText;
    std::vector<Token> _tokens;
    Rectangle _rect;
    Tokenizer _tokenizer;
    float _beginActionTime;
    int _brushRadius = 5;
    NoteMode _mode = Helium::NoteMode::READ;
    Cursor _cursor;
    std::shared_ptr<Configuration> _config;
    std::shared_ptr<InputHandler> _inputHandler;
    Vector2 _prevCursorPos;
}; 


}

#endif // NOTEAREA_HPP
