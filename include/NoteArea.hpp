#ifndef NOTEAREA_HPP
#define NOTEAREA_HPP

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

enum class NoteMode {
	READ,
	WRITE,
	DRAW,
};
class NoteArea {
public:
    NoteArea(std::shared_ptr<Configuration> config);
    void Initialize();
    void Update();
    void UpdateText();
    void Draw();
    std::string GetFormattedText();
    void SetRect(Rectangle rect);
private:
    RenderTexture2D _texture;
    std::string _rawText;
    std::vector<Token> _tokens;
    Rectangle _rect;
    Tokenizer _tokenizer;
    float _beginActionTime;
    int _brushRadius = 5;
    Helium::NoteMode _mode = Helium::NoteMode::READ;
    Font _defaultFont;
    Font _boldFont;
    Font _italicFont;
    Cursor _cursor;
    std::shared_ptr<Configuration> _config;
    Vector2 _prevCursorPos;
}; 


}

#endif // NOTEAREA_HPP
