#include "NoteArea.hpp"
#include "Configuration.hpp"
#include "raylib.h"
#include "rlgl.h"
#include "constants.h"
#include "tokenizer.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>





namespace Helium {

NoteArea::NoteArea(std::shared_ptr<Configuration> config) {
    _config = config;
}


void NoteArea::Initialize(int heightOffset) {
    Texture2D temp;
    if(Utils::LoadNote("C:/Users/Thiago/picasso.note", _rawText, temp))
    {
        _tokens = _tokenizer.tokenize(_rawText);
        _texture = LoadRenderTexture(temp.width, temp.height);
        BeginTextureMode(_texture);
        DrawTextureV(temp, {0, 0}, WHITE);
        EndTextureMode();
        UnloadTexture(temp);
    }
    else
        _texture = LoadRenderTexture(_config->MaxNoteWidth, GetScreenHeight());

    rlSetBlendFactorsSeparate(0x0302, 0x0303, 1, 0x0303, 0x8006, 0x8006); // Required configuration to be able to erase on the texture!
    _rect.width = _config->MaxNoteWidth;
    _rect.height = GetScreenHeight();
    _rect.x = (GetScreenWidth() - _rect.width) / 2;
    _rect.y = heightOffset;

   _cursor.MoveToEnd(_rawText);
}

void NoteArea::Update() {
    
    // Change height to fit screen
    if(IsWindowResized()) {
        _rect.height = GetScreenHeight();
        _rect.x = (GetScreenWidth() - _rect.width) / 2;
    }


    int wheel;
    if (CheckCollisionPointRec(GetMousePosition(), _rect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        switch(_mode) {
            case Helium::NoteMode::READ:
                _mode = Helium::NoteMode::WRITE;
                break;
            default:
                break;
        }
    }

    if((IsKeyPressed(KEY_ESCAPE) && _mode == NoteMode::WRITE)) {
        _mode = NoteMode::READ;
        _tokens = _tokenizer.tokenize(_rawText);
    }

    if((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_S)) {
        Utils::SaveNote("C:/Users/Thiago/picasso.note", _rawText, _texture.texture);
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
            if(IsKeyPressed(KEY_BACKSPACE) && !_rawText.empty())
            {
                _beginActionTime = 0;
                _rawText.pop_back();
                _cursor.MoveLeft(_rawText);
            }

            if(IsKeyDown(KEY_BACKSPACE) && !_rawText.empty()) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _rawText.pop_back();
                    _cursor.MoveLeft(_rawText);
                }
            }

            if(IsKeyPressed(KEY_LEFT)) {
                _beginActionTime = 0;
                _cursor.MoveLeft(_rawText);
            }
            if(IsKeyDown(KEY_LEFT)) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _cursor.MoveLeft(_rawText);
                }
            }
            if(IsKeyPressed(KEY_RIGHT)) {
                _beginActionTime = 0;
                _cursor.MoveRight(_rawText);
            }

            if(IsKeyDown(KEY_RIGHT)) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _cursor.MoveRight(_rawText);
                }
            }
            if(IsKeyPressed(KEY_ENTER)) {
                _rawText += '\n';
                _cursor.MoveRight(_rawText);
            }
            // Read characters instead of individual key presses
            int key;
            while ((key = GetCharPressed()) > 0) {
                if (key >= 32) {
                    _rawText.insert(_cursor.GetPosition(), 1, (char) key);
                    _cursor.MoveRight(_rawText);
                    std::cout << _cursor.GetPosition() << std::endl;
                }
            }
    
            break;
        case Helium::NoteMode::DRAW:
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                _prevCursorPos = GetMousePosition();
            }
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                int reqHeight = std::max(_texture.texture.height, GetMouseY());
                if( _texture.texture.height < reqHeight) {
                    RenderTexture2D newTexture = LoadRenderTexture(_config->MaxNoteWidth, reqHeight);
                    BeginTextureMode(newTexture);
                    BeginBlendMode(BLEND_CUSTOM);
                    ClearBackground(BLANK);
                    EndBlendMode();
                    DrawTextureRec(_texture.texture, {0, 0, (float)_texture.texture.width, (float)-_texture.texture.height }, {0, 0}, WHITE);
                    EndTextureMode();
                    UnloadRenderTexture(_texture);
                    _texture = newTexture;
                }
            }
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                BeginTextureMode(_texture);
                DrawCircleV({GetMouseX() - _rect.x, GetMouseY() - _rect.y}, _brushRadius, PINK);
                DrawLineEx({_prevCursorPos.x - _rect.x, _prevCursorPos.y - _rect.y}, {GetMouseX() - _rect.x, GetMouseY() - _rect.y}, _brushRadius * 2, PINK);
                EndTextureMode();
                _prevCursorPos = GetMousePosition();
            }
            if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                BeginTextureMode(_texture);
                BeginBlendMode(BLEND_CUSTOM);
                DrawCircle(GetMouseX() + _rect.x, GetMouseY() + _rect.y, _brushRadius, BLANK);
                DrawLineEx(_prevCursorPos, GetMousePosition(), _brushRadius * 2, PINK);
                EndBlendMode();
                EndTextureMode();
                _prevCursorPos = GetMousePosition();
            }
            
            _brushRadius = std::clamp(_brushRadius + GetMouseWheelMove(), 1.0f, 99.0f);
            break;
            default:
                break;
    }
}


void NoteArea::Draw() {
    DrawRectangleRec(_rect, _config->ColorTheme.Foreground);

    int y = _rect.y;
    const int fontSize = 16;  // Fixed font size for all lines

    std::istringstream stream(_rawText);
    std::istringstream caretStream;
    if(_cursor.GetPosition() == _rawText.length()) {
        caretStream = std::istringstream(_rawText);
    } else {
        caretStream = std::istringstream(_rawText.substr(0, _cursor.GetPosition()));
    }
    std::string line;
    int caretX = _rect.x, caretY = 0;  // Caret position

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
                        DrawTextEx(_config->Formatting.DefaultFont, t.value.c_str(), { (float) _rect.x, (float) y }, _config->Formatting.GetFontSizeForHeader(stoi(t.attributes.at("level"))), 1, _config->ColorTheme.TextColor);
                        y += _config->Formatting.GetFontSizeForHeader(stoi(t.attributes.at("level")));
                        break;
                    default:
                        int x = _rect.x;
                        std::vector<Token> inlines = _tokenizer.tokenizeInline(t.value);
                        for(Token it : inlines) {
                            switch(it.type) {
                                case TokenType::BOLD:
                                    DrawTextEx(_config->Formatting.BoldFont, it.value.c_str(), { (float) x, (float) y }, _config->Formatting.Paragraph, _config->Formatting.CharSpacing, _config->ColorTheme.TextColor);
                                    x += MeasureTextEx(_config->Formatting.BoldFont, it.value.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;
                                    break;
                                case TokenType::ITALIC:
                                    DrawTextEx(_config->Formatting.ItalicFont, it.value.c_str(), { (float) x, (float) y }, _config->Formatting.Paragraph, _config->Formatting.CharSpacing, _config->ColorTheme.TextColor);
                                    x += MeasureTextEx(_config->Formatting.ItalicFont, it.value.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;
                                    break;
                                case TokenType::STRIKETHROUGH:
                                    DrawTextEx(_config->Formatting.DefaultFont, it.value.c_str(), { (float) x, (float) y }, _config->Formatting.Paragraph, _config->Formatting.CharSpacing, _config->ColorTheme.TextColor);
                                    int xOffset;
                                    xOffset = MeasureTextEx(_config->Formatting.BoldFont, it.value.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;
                                    DrawLineEx({ (float) x, (float) y + _config->Formatting.Paragraph * 0.5f }, { (float) (x + xOffset), (float) y + _config->Formatting.Paragraph * 0.5f}, _config->Formatting.StrikethroughWidth, _config->ColorTheme.TextColor);
                                    x += xOffset;
                                    break;
                                default:
                                    DrawTextEx(_config->Formatting.DefaultFont, it.value.c_str(), { (float) x, (float) y }, _config->Formatting.Paragraph, _config->Formatting.CharSpacing, _config->ColorTheme.TextColor);
                                     x += MeasureTextEx(_config->Formatting.DefaultFont, it.value.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;
                                    break;
                            }
                        }

                        y += _config->Formatting.Paragraph;
                        break;
                }
            }
            break;

        case Helium::NoteMode::WRITE: {
            bool lastCharWasNewline = false;
            // Loop through each line of text and draw it
            while (std::getline(stream, line)) {
                DrawText(line.c_str(), _rect.x, y, fontSize, _config->ColorTheme.TextColor);
                y += fontSize;  // Move to next line position
            }
            y = _rect.y;
            while(std::getline(caretStream, line)) {
                // Set caret position to the end of the last line
                caretX = MeasureText(line.c_str(), fontSize) + _rect.x;  // Calculate width of the text for X
                caretY = y;  // Y is the current line

                y += fontSize;  // Move to next line position
            }

            
            // Handle case where the last character(s) are newlines
            if (!_rawText.empty()) {
                int newlineCount = 0;

                // Count the number of consecutive trailing newlines
                for (int i = _rawText.length() - 1; i >= 0 && _rawText[i] == '\n'; --i) {
                    newlineCount++;
                }

                if (newlineCount > 0) {
                    caretX = _rect.x;  // Move caret to the start of the line
                    caretY += fontSize * newlineCount;  // Move caret down by the number of newlines
                    lastCharWasNewline = true;
                }
            }

            // Draw caret only if it's visible (blinking)
            if (showCaret) {
                DrawRectangle(caretX, caretY, 2, fontSize, _config->ColorTheme.TextColor);  // Thin vertical caret
            }
            break;
        }
    }
    // Draw the texture
    DrawTextureRec(_texture.texture, { 0, 0, static_cast<float>(_texture.texture.width), -static_cast<float>(_texture.texture.height) }, { _rect.x, _rect.y }, WHITE);
    DrawRectangleLinesEx({_rect.x, _rect.y, static_cast<float>(_texture.texture.width), static_cast<float>(_texture.texture.height)}, 3, RED);
    // Handle draw mode (if applicable)
    if (_mode == Helium::NoteMode::DRAW) {
        DrawCircleLines(GetMouseX(), GetMouseY(), _brushRadius, _config->ColorTheme.BrushBorder);
    }
}


void NoteArea::SetRect(Rectangle rect) {
    _rect = rect;
}
void NoteArea::UpdateText() {}
}
