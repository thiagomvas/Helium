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
    NoteArea(std::shared_ptr<InputHandler> input);
    void Initialize(int heightOffset);
    void Update();
    void Save();
    void Draw();
    void RenderMarkdown(int y);
    std::string GetText();
    std::string GetPath();
    void SetPath(const std::string& path);
    void SetRect(Rectangle rect);
    void SetMode(NoteMode mode);
	Cursor* GetCursor();
    void TryLoadNote(const std::string& path);
    void SetDirty();
private:
    std::string path;
    RenderTexture2D _texture;
    std::shared_ptr<std::string> _rawText;
    std::vector<Token> _tokens;
    Rectangle _rect;
    Tokenizer _tokenizer;
    float _beginActionTime;
    int _brushRadius = 5;
    NoteMode _mode = Helium::NoteMode::READ;
    Cursor _cursor;
    std::shared_ptr<InputHandler> _inputHandler;
    Vector2 _prevCursorPos;
    std::shared_ptr<std::vector<std::string>> wrappedLines;
    std::shared_ptr<std::vector<std::string>> readModeLines;
    bool isDirty;
}; 


}

#endif // NOTEAREA_HPP
