#include "NoteArea.hpp"
#include "raylib.h"
#include "rlgl.h"
#include "constants.h"
#include "tokenizer.hpp"
#include <iostream>
#include <string>
#include <vector>
namespace Helium {

void NoteArea::Initialize() {
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
    DrawRectangleRec(_rect, _mode == Helium::NoteMode::READ ? GREEN : (_mode == Helium::NoteMode::DRAW) ? BLUE : RED);
    int y = 0, fontSize = 0;
    switch(_mode) {
        case Helium::NoteMode::DRAW:
            DrawCircleLines(GetMouseX(), GetMouseY(), _brushRadius, BLACK);
        default:
        case Helium::NoteMode::READ:
            for(Token t : _tokens) {
                switch (t.type) {
                    case Helium::TokenType::HEADER:
                        fontSize = Formatting::GetFontSizeForHeader(stoi(t.attributes["level"]));
                        DrawText(t.value.c_str(), 0, y, fontSize, BLACK);
                        y += fontSize;
                        break;
                    default:
                        DrawText(t.value.c_str(), 0, y, Formatting::PARAGRAPH, BLACK);
                        y += Formatting::PARAGRAPH;
                        break;
                }
            }
            break;
        case Helium::NoteMode::WRITE:
            DrawText(_rawText.c_str(), 0, 0, 16, BLACK);
            break;
    }
    DrawTextureRec(_texture.texture, { 0, 0, static_cast<float>(_texture.texture.width), -static_cast<float>(_texture.texture.height) }, { 0.0f, 0.0f }, WHITE);
}

void NoteArea::SetRect(Rectangle rect) {
    _rect = rect;
}
void NoteArea::UpdateText() {}
} // namespace Helium
