#ifndef NOTEAREA_HPP
#define NOTEAREA_HPP

#include "cursor.hpp"
#include "raylib.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.hpp"
#include "constants.h"

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
    Rectangle _rect = { 0, 0, Config::EDITAREA_WIDTH, 0 };
    Tokenizer _tokenizer;
    float _beginDeleteTime;
    int _brushRadius = 5;
    Helium::NoteMode _mode = Helium::NoteMode::READ;
    Font _defaultFont;
    Font _boldFont;
    Font _italicFont;
    Cursor _cursor;
}; 


}

#endif // NOTEAREA_HPP
