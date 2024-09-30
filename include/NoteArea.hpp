#ifndef NOTEAREA_HPP
#define NOTEAREA_HPP

#include "raylib.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.hpp"

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
    Rectangle _rect;
    Tokenizer _tokenizer;
    float _beginDeleteTime;
	int _brushRadius = 5;
	Helium::NoteMode _mode = Helium::NoteMode::READ;
}; 


}

#endif // NOTEAREA_HPP
