#include "NoteArea.hpp"
#include "Configuration.hpp"
#include "InputHandler.hpp"
#include "raylib.h"
#include "rlgl.h"
#include "constants.h"
#include "tokenizer.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>





namespace Helium {

NoteArea::NoteArea(std::shared_ptr<Configuration> config, std::shared_ptr<Helium::InputHandler> input) : _config(config), _inputHandler(input), _rawText(std::make_shared<std::string>()), wrappedLines(std::make_shared<std::vector<std::string>>()) {
    
}


void NoteArea::Initialize(int heightOffset) {
    Texture2D temp;
    if(Serializer::LoadNote("C:/Users/Thiago/picasso.note", *_rawText, temp))
    {
        _tokens = _tokenizer.tokenize(*_rawText);
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
    _cursor.SetTextPter(_rawText, wrappedLines);
   _cursor.MoveToEnd();
}

void NoteArea::SetMode(NoteMode mode) {
    _mode = mode;
    if(mode == NoteMode::READ) {
        _tokens = _tokenizer.tokenize(*_rawText);
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
            if(IsKeyPressed(KEY_BACKSPACE) && !_rawText->empty())
            {
                _beginActionTime = 0;
                _cursor.MoveLeft();
                _rawText->erase(_cursor.GetPosition(), 1);
                isDirty = true;
            }

            
        if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
            std::cout << "Cursor - (" << _cursor.GetCurrentLineColumn() << "," << _cursor.GetCurrentLineIndex() << ")[" << _cursor.GetPosition() << "]" << std::endl;
            int index = 0;

            for (auto line : *wrappedLines) {

                std::cout << index << "(" << line.length() <<  ") : " << line << std::endl;
                index++;
            }
        }


            if(IsKeyDown(KEY_BACKSPACE) && !_rawText->empty()) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _cursor.MoveLeft();
                    _rawText->erase(_cursor.GetPosition(), 1);
                    isDirty = true;
                }
            }

            if(IsKeyPressed(KEY_LEFT_SHIFT)) {
                _cursor.BeginHighlight();
            }

            if(IsKeyReleased(KEY_LEFT_SHIFT)) {
                _cursor.EndHighlight();
                std::cout << "Highlight text: \"" << _cursor.GetHighlightedText() << "\"" << std::endl;
            }

            if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
                std::string clipboard(GetClipboardText());
                _rawText->insert(_cursor.GetPosition(), clipboard);
                isDirty = true;
                for(int i = 0; i < clipboard.length(); i++) {
                    _cursor.MoveRight();
                }
            }

            if(IsKeyPressed(KEY_LEFT)) {
                _beginActionTime = 0;
                _cursor.MoveLeft();
            }
            if(IsKeyDown(KEY_LEFT)) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _cursor.MoveLeft();
                }
            }
            if(IsKeyPressed(KEY_RIGHT)) {
                _beginActionTime = 0;
                _cursor.MoveRight();
            }

            if(IsKeyDown(KEY_RIGHT)) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _cursor.MoveRight();
                }
            }

            if(IsKeyPressed(KEY_UP)) {
                _beginActionTime = 0;
                _cursor.MoveUp();
            }
            if(IsKeyDown(KEY_UP)) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _cursor.MoveUp();
                }
            }

            if(IsKeyPressed(KEY_DOWN)) {
                _beginActionTime = 0;
                _cursor.MoveDown();
            }
            if(IsKeyDown(KEY_DOWN)) {
                _beginActionTime += GetFrameTime();
                if(_beginActionTime >= _config->ActionRepeatDelaySeconds) {
                    _cursor.MoveDown();
                }
            }

            if(IsKeyPressed(KEY_HOME))
                _cursor.MoveToStart();
            if(IsKeyPressed(KEY_END))
                _cursor.MoveToEnd();

            if(IsKeyPressed(KEY_ENTER)) {
                _rawText->insert(_cursor.GetPosition(), 1, '\n');
                _cursor.MoveRight();
                isDirty = true;
            }
            // Read characters instead of individual key presses
            int key;
            while ((key = GetCharPressed()) > 0) {
                if (key >= 32) {
                    _rawText->insert(_cursor.GetPosition(), 1, (char) key);
                    _cursor.MoveRight();
                    isDirty = true;
                }
            }

            if(isDirty) {
                isDirty = false;
                Utils::WrapText(*_rawText, wrappedLines, _config);
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

    std::istringstream stream(*_rawText);
    std::string caretString;
    std::istringstream caretStream;
    if(_cursor.GetPosition() == _rawText->length()) {
        caretStream = std::istringstream(*_rawText);
    } else {
        caretStream = std::istringstream(_rawText->substr(0, _cursor.GetPosition()));
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
            int highlightStart = _cursor.GetHighlightStart(), highlightEnd = _cursor.GetHighlightEnd();
            float currentY = _rect.y;

            float lineHeight = MeasureTextEx(_config->Formatting.DefaultFont, "A", _config->Formatting.Paragraph, _config->Formatting.CharSpacing).y;

            int cursorPos = _cursor.GetPosition();
            float currentX = _rect.x;
            int totalChars = 0;  // Tracks total characters processed in all lines
            bool caretPositioned = false;  // Ensures caret is set only once
            std::istringstream textStream(*_rawText);

            int lineIndex = 0;
            for (const std::string& wrappedLine : *wrappedLines) {
                int lineLength = wrappedLine.length();
                int lineEnd = totalChars + lineLength;
                int lineWidth = MeasureTextEx(_config->Formatting.DefaultFont, wrappedLine.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;
                
                if(_cursor.IsHighlighting()) {
                    
int hlStart = _cursor.GetHighlightStart(), hlEnd = _cursor.GetHighlightEnd();
int hlsc = _cursor.GetColumn(hlStart), hlsl = _cursor.GetLine(hlStart);
int hlec = _cursor.GetColumn(hlEnd), hlel = _cursor.GetLine(hlEnd);

// Case 1: Highlighting begins before line and ends after line
if (hlsl < lineIndex && hlel > lineIndex) {
    DrawRectangle(_rect.x, currentY, lineWidth, lineHeight, _config->ColorTheme.TextHighlight);
}
// Case 2: Highlighting starts in this line but ends after line
else if (hlsl == lineIndex && hlel > lineIndex && hlsc >= 0 && hlsc < lineLength) {
    // Calculate the offset of the start of highlighting in this line
    int offset = MeasureTextEx(_config->Formatting.DefaultFont, wrappedLine.substr(0, hlsc).c_str(), 
                                _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;

    // Draw rectangle from offset to end of line
    DrawRectangle(_rect.x + offset, currentY, lineWidth - offset, lineHeight, _config->ColorTheme.TextHighlight);
}
// Case 3: Highlighting starts before this line but ends in this line
else if (hlsl < lineIndex && hlel == lineIndex && hlec <= lineLength) {
    // Calculate the offset to the end of the highlight in this line
    int offset = MeasureTextEx(_config->Formatting.DefaultFont, wrappedLine.substr(0, hlec).c_str(), 
                                _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;

    // Draw rectangle from the beginning of the line to the highlight end
    DrawRectangle(_rect.x, currentY, offset, lineHeight, _config->ColorTheme.TextHighlight);
}
// Case 4: Highlighting starts and ends in the same line
else if (hlsl == hlel && hlsl == lineIndex && hlsc >= 0 && hlsc < lineLength && hlec > hlsc && hlec <= lineLength) {
    // Get the highlighted text from the wrapped line
    std::string highlightedText = wrappedLine.substr(hlsc, hlec - hlsc);
    
    if (!highlightedText.empty()) {
        // Calculate the start position of the highlight
        float highlightStartX = MeasureTextEx(_config->Formatting.DefaultFont, wrappedLine.substr(0, hlsc).c_str(), 
                                               _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;

        // Calculate the width of the highlighted text
        float highlightWidth = MeasureTextEx(_config->Formatting.DefaultFont, highlightedText.c_str(), 
                                              _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;

        // Draw rectangle for the highlighted portion
        DrawRectangle(_rect.x + highlightStartX, currentY, highlightWidth, lineHeight, _config->ColorTheme.TextHighlight);
    }
}
                }

                
                // Render the wrapped line
                DrawTextEx(_config->Formatting.DefaultFont, wrappedLine.c_str(), 
                           {currentX, currentY}, 
                           _config->Formatting.Paragraph, 
                           _config->Formatting.CharSpacing, 
                           _config->ColorTheme.TextColor);

                currentY += lineHeight;  // Move down to the next line
                totalChars += wrappedLine.length();
                lineIndex++;
            }            
            if(_cursor.GetCurrentLineColumn() == 0) 
                caretX = _rect.x;
            else
                caretX = MeasureTextEx(_config->Formatting.DefaultFont,
                                       wrappedLines->at(_cursor.GetCurrentLineIndex()).substr(0, _cursor.GetCurrentLineColumn()).c_str(),
                                       _config->Formatting.Paragraph,
                                       _config->Formatting.CharSpacing).x + _rect.x ;

            caretY = _rect.y + lineHeight * _cursor.GetCurrentLineIndex();
            // Draw the caret if it’s visible
            if (showCaret) {
                DrawRectangle(caretX, caretY, 2, lineHeight, _config->ColorTheme.TextColor);
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
    Serializer::SaveNote("C:/Users/Thiago/picasso.note", *_rawText, _texture.texture);
}

std::string NoteArea::GetText() {
    return *_rawText;
}

Cursor* NoteArea::GetCursor() {
    return &_cursor;
}
}
