#include "NoteArea.hpp"
#include "Configuration.hpp"
#include "InputHandler.hpp"
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

NoteArea::NoteArea(std::shared_ptr<Configuration> config, std::shared_ptr<Helium::InputHandler> input) : _config(config), _inputHandler(input) {
    
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

void NoteArea::SetMode(NoteMode mode) {
    _mode = mode;
    if(mode == NoteMode::READ) {
        _tokens = _tokenizer.tokenize(_rawText);
    }
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
                SetMode(NoteMode::WRITE);
                _inputHandler->SetMode(NoteMode::WRITE);
                break;
        }
    }


    if(IsKeyPressed(KEY_LEFT_ALT))
    {
        SetMode(NoteMode::DRAW);
        _inputHandler->SetMode(NoteMode::DRAW);
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

            if(IsKeyPressed(KEY_LEFT_SHIFT)) {
                _cursor.BeginHighlight();
            }

            if(IsKeyReleased(KEY_LEFT_SHIFT)) {
                _cursor.EndHighlight();
                std::cout << "Highlight text: \"" << _cursor.GetHighlightedText(_rawText) << "\"" << std::endl;
            }

            if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
                std::string clipboard(GetClipboardText());
                _rawText += clipboard;
                for(int i = 0; i < clipboard.length(); i++) {
                    _cursor.MoveRight(_rawText);
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

            if(IsKeyPressed(KEY_UP)) {
                _beginActionTime = 0;
                _cursor.MoveUp(_rawText);
            }
            if(IsKeyDown(KEY_UP)) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _cursor.MoveUp(_rawText);
                }
            }

            if(IsKeyPressed(KEY_DOWN)) {
                _beginActionTime = 0;
                _cursor.MoveDown(_rawText);
            }
            if(IsKeyDown(KEY_DOWN)) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _cursor.MoveDown(_rawText);
                }
            }

            if(IsKeyPressed(KEY_HOME))
                _cursor.MoveToStart();
            if(IsKeyPressed(KEY_END))
                _cursor.MoveToEnd(_rawText);

            if(IsKeyPressed(KEY_ENTER)) {
                _rawText.insert(_cursor.GetPosition(), 1, '\n');
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
                DrawCircleV({GetMouseX() - _rect.x, GetMouseY() - _rect.y}, _brushRadius, BLANK);
                DrawLineEx({_prevCursorPos.x - _rect.x, _prevCursorPos.y - _rect.y}, {GetMouseX() - _rect.x, GetMouseY() - _rect.y}, _brushRadius * 2, BLANK);
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

    std::istringstream stream(_rawText);
    std::string caretString;
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
    int highlightStart = _cursor.GetHighlightStart(), highlightEnd = _cursor.GetHighlightEnd();
    float currentY = _rect.y;
    float lineHeight = MeasureTextEx(_config->Formatting.DefaultFont, "A", _config->Formatting.Paragraph, _config->Formatting.CharSpacing).y;

    int textLength = _rawText.length();
    int cursorPos = _cursor.GetPosition();
    
    float currentX = _rect.x;
    std::string line;
    std::istringstream textStream(_rawText);

    int processedChars = 0;

    float caretX = _rect.x;
    float caretY = _rect.y;

    while (std::getline(textStream, line)) {
        float lineWidth = MeasureTextEx(_config->Formatting.DefaultFont, line.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;
        
        if (_cursor.IsHighlighting() && highlightEnd > processedChars && highlightStart < processedChars + line.size()) {
            int startInLine = std::max(0, highlightStart - processedChars);
            int endInLine = std::min(static_cast<int>(line.size()), highlightEnd - processedChars);
            
            std::string beforeHighlight = line.substr(0, startInLine);
            std::string highlightedText = line.substr(startInLine, endInLine - startInLine);
            
            float highlightStartX = currentX + MeasureTextEx(_config->Formatting.DefaultFont, beforeHighlight.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;
            float highlightWidth = MeasureTextEx(_config->Formatting.DefaultFont, highlightedText.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;

            DrawRectangle(highlightStartX, currentY, highlightWidth, lineHeight, _config->ColorTheme.TextHighlight );
        }

        DrawTextEx(_config->Formatting.DefaultFont, line.c_str(), {currentX, currentY}, _config->Formatting.Paragraph, _config->Formatting.CharSpacing, _config->ColorTheme.TextColor);

        if (cursorPos >= processedChars && cursorPos <= processedChars + line.size()) {
            std::string beforeCaret = line.substr(0, cursorPos - processedChars);
            caretX = currentX + MeasureTextEx(_config->Formatting.DefaultFont, beforeCaret.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;
            caretY = currentY;  // Caret should be aligned with the current line
        }

        currentY += lineHeight;
        processedChars += line.size() + 1;  // Include the newline character
    }

    // Draw caret only if it's visible (blinking)
    if (showCaret) {
        DrawRectangle(caretX, caretY, 2, lineHeight, _config->ColorTheme.TextColor);  // Thin vertical caret
    }

    break;
}


    }
    // Draw the texture
    DrawTextureRec(_texture.texture, { 0, 0, static_cast<float>(_texture.texture.width), -static_cast<float>(_texture.texture.height) }, { _rect.x, _rect.y }, WHITE);
    // Handle draw mode (if applicable)
    if (_mode == Helium::NoteMode::DRAW) {
        DrawCircleLines(GetMouseX(), GetMouseY(), _brushRadius, _config->ColorTheme.BrushBorder);
    }
}


void NoteArea::SetRect(Rectangle rect) {
    _rect = rect;
}
void NoteArea::Save() {
    Utils::SaveNote("C:/Users/Thiago/picasso.note", _rawText, _texture.texture);
}

std::string NoteArea::GetText() {
    return _rawText;
}

Cursor* NoteArea::GetCursor() {
    return &_cursor;
}
}
