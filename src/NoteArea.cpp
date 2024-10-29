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
#include <fstream>
#include <filesystem>


void safeErase(std::shared_ptr<std::string> text, int start, int end) {
    if (!text || start < 0 || end < 0 || start >= text->length() || end > text->length() || start > end) {
        std::cerr << "Invalid indices. No changes made." << std::endl;
        return;
    }
    text->erase(start, std::min(static_cast<int>(end - start + 1), static_cast<int>(text->length() - start)));
}





namespace Helium {

NoteArea::NoteArea( std::shared_ptr<Helium::InputHandler> input) : _inputHandler(input), _rawText(std::make_shared<std::string>()), wrappedLines(std::make_shared<std::vector<std::string>>()), readModeLines(std::make_shared<std::vector<std::string>>()) {
    
}


void NoteArea::Initialize(int heightOffset) {
    Texture2D temp;
    _texture = LoadRenderTexture(Helium::Configuration::getInstance().MaxNoteWidth, GetScreenHeight());

    rlSetBlendFactorsSeparate(0x0302, 0x0303, 1, 0x0303, 0x8006, 0x8006); // Required configuration to be able to erase on the texture!
    _rect.width = Helium::Configuration::getInstance().MaxNoteWidth;
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
        std::vector<Helium::Token> temp;
        for (const Token& t : _tokens) {
            switch (t.type) {
                case Helium::TokenType::HEADER: {
                    int headerFontSize = Helium::Configuration::getInstance().Formatting.GetFontSizeForHeader(std::stoi(t.attributes.at("level")));
                    Utils::WrapText(t.value, readModeLines, headerFontSize);
                    for(std::string line : *readModeLines) {
                        Helium::Token token(t.type, t.value, _tokenizer.tokenizeInline(line));
                        token.attributes[ATTRIBUTE_HEADER_LEVEL] = t.attributes.at(ATTRIBUTE_HEADER_LEVEL);
                        temp.push_back(token);
                    }
                    break;
                }
                // MULTILINE TOKENS
                case Helium::TokenType::QUOTE:
                case Helium::TokenType::CODE: {
                    temp.push_back(t);
                    break;
                }
                
                default:
                    Utils::WrapText(t.value, readModeLines, Helium::Configuration::getInstance().Formatting.Paragraph);
                    for(const std::string& line : *readModeLines) {
                        Helium::Token token(t.type, t.value, _tokenizer.tokenizeInline(line));
                        temp.push_back(token);
                    }
                    break;
            }
        }
        _tokens = std::move(temp);
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
        case Helium::NoteMode::WRITE: {
            
            
            if (IsKeyPressed(KEY_LEFT_SHIFT)) {
                _cursor.BeginHighlight();
            }

            if (IsKeyReleased(KEY_LEFT_SHIFT)) {
                _cursor.EndHighlight();
                std::cout << "Highlight text: \"" << _cursor.GetHighlightedText() << "\"" << std::endl;
            }

            // Handle Backspace key
            if (IsKeyPressed(KEY_BACKSPACE) && !_rawText->empty()) {
                _beginActionTime = 0;
                if (_cursor.IsHighlighting()) {
                    // Erase highlighted text
                    safeErase(_rawText, _cursor.GetHighlightStart(), _cursor.GetHighlightEnd());
                    _cursor.Deselect();
                } else {
                    // Erase one character to the left of the cursor
                    safeErase(_rawText, _cursor.GetPosition() - 1, _cursor.GetPosition() - 1);
                    _cursor.MoveLeft();
                }
                isDirty = true;
            }

            // Handle repeating backspace action
            if (IsKeyDown(KEY_BACKSPACE) && !_rawText->empty()) {
                _beginActionTime += GetFrameTime();
                if (_beginActionTime >= Helium::Configuration::getInstance().ActionRepeatDelaySeconds) {
                    safeErase(_rawText, _cursor.GetPosition() - 1, _cursor.GetPosition() - 1);
                    _cursor.MoveLeft();
                    isDirty = true;
                }
            }

            // Handle Control key for debug output
            if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
                std::cout << "Cursor - (" << _cursor.GetCurrentLineColumn() << "," << _cursor.GetCurrentLineIndex() << ")[" << _cursor.GetPosition() << "]" << std::endl;
                int index = 0;

                for (auto line : *wrappedLines) {
                    std::cout << index << "(" << line.length() <<  ") : " << line << std::endl;
                    index++;
                }
            }

            // Handle paste operation
            if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
                if (_cursor.IsHighlighting()) {
                    safeErase(_rawText, _cursor.GetHighlightStart(), _cursor.GetHighlightEnd());
                    _cursor.Deselect();
                }
                std::string clipboard(GetClipboardText());
                _rawText->insert(_cursor.GetPosition(), clipboard);
                isDirty = true;
                _cursor.Goto(_cursor.GetPosition() + clipboard.length());
            }

            // Handle cursor movement
            if (IsKeyPressed(KEY_LEFT)) {
                _beginActionTime = 0;
                _cursor.MoveLeft();
            }
            if (IsKeyDown(KEY_LEFT)) {
                _beginActionTime += GetFrameTime();
                if (_beginActionTime >= Helium::Configuration::getInstance().ActionRepeatDelaySeconds) {
                    _cursor.MoveLeft();
                }
            }
            if (IsKeyPressed(KEY_RIGHT)) {
                _beginActionTime = 0;
                _cursor.MoveRight();
            }
            if (IsKeyDown(KEY_RIGHT)) {
                _beginActionTime += GetFrameTime();
                if (_beginActionTime >= Helium::Configuration::getInstance().ActionRepeatDelaySeconds) {
                    _cursor.MoveRight();
                }
            }
            if (IsKeyPressed(KEY_UP)) {
                _beginActionTime = 0;
                _cursor.MoveUp();
            }
            if (IsKeyDown(KEY_UP)) {
                _beginActionTime += GetFrameTime();
                if (_beginActionTime >= Helium::Configuration::getInstance().ActionRepeatDelaySeconds) {
                    _cursor.MoveUp();
                }
            }
            if (IsKeyPressed(KEY_DOWN)) {
                _beginActionTime = 0;
                _cursor.MoveDown();
            }
            if (IsKeyDown(KEY_DOWN)) {
                _beginActionTime += GetFrameTime();
                if (_beginActionTime >= Helium::Configuration::getInstance().ActionRepeatDelaySeconds) {
                    _cursor.MoveDown();
                }
            }

            // Handle Home and End keys
            if (IsKeyPressed(KEY_HOME))
                _cursor.MoveToStart();
            if (IsKeyPressed(KEY_END))
                _cursor.MoveToEnd();

            // Handle Enter key
            if (IsKeyPressed(KEY_ENTER)) {
                if (_cursor.IsHighlighting()) {
                    safeErase(_rawText, _cursor.GetHighlightStart(), _cursor.GetHighlightEnd());
                    _cursor.Deselect(); // Deselect after erasing
                }
                _rawText->insert(_cursor.GetPosition(), 1, '\n');
                _cursor.MoveRight();
                isDirty = true;
            }

            // Read characters instead of individual key presses
            int key;
            while ((key = GetCharPressed()) > 0) {
                if (key >= 32) { // Valid character range
                    if (_cursor.IsHighlighting()) {
                        safeErase(_rawText, _cursor.GetHighlightStart(), _cursor.GetHighlightEnd());
                        _cursor.Deselect();
                    }
                    _rawText->insert(_cursor.GetPosition(), 1, (char)key);
                    _cursor.MoveRight(); // Move the cursor after inserting
                    isDirty = true;
                }
            }

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), _rect)) {
                Vector2 mousePosition = GetMousePosition();
                
                int lineIndex = (mousePosition.y - _rect.y) / Helium::Configuration::getInstance().Formatting.GetLineHeight(Helium::Configuration::getInstance().Formatting.Paragraph);
                
                if (lineIndex < wrappedLines->size()) {
                    float currentY = _rect.y; 
                    int totalChars = 0;
                    int width = mousePosition.x - _rect.x;

                    for(int i = 0; i < lineIndex; i++)
                        totalChars += wrappedLines->at(i).size() + 1;
                    std::string line = wrappedLines->at(lineIndex); 
                    bool moved = false;
                    for (size_t i = 0; i < line.size(); i++) {
                        char c = line[i];
                        int charWidth = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, &c, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;

                        if (width <= charWidth) {
                            _cursor.Goto(totalChars + i); 
                            moved = true;
                            break;
                        }
                        
                        width -= charWidth + Helium::Configuration::getInstance().Formatting.CharSpacing;
                    }
                    if(!moved) {
                        _cursor.Goto(totalChars + line.size());
                    }
                }
                else {
                    _cursor.MoveToEnd();
                }
            }
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                _cursor.BeginHighlight();
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                if(_cursor.GetHighlightStart() == _cursor.GetHighlightEnd())
                    _cursor.Deselect();
                else
                    _cursor.EndHighlight();
            }


            if(isDirty) {
                isDirty = false;
                Utils::WrapText(*_rawText, wrappedLines);
            }
    
            break;
        }
        case Helium::NoteMode::DRAW: {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                _prevCursorPos = GetMousePosition();
            }
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                int reqHeight = std::max(_texture.texture.height, GetMouseY());
                if( _texture.texture.height < reqHeight) {
                    RenderTexture2D newTexture = LoadRenderTexture(Helium::Configuration::getInstance().MaxNoteWidth, reqHeight);
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
                DrawCircleV({GetMouseX() - _rect.x, GetMouseY() - _rect.y}, _brushRadius, RED);
                DrawLineEx({_prevCursorPos.x - _rect.x, _prevCursorPos.y - _rect.y}, {GetMouseX() - _rect.x, GetMouseY() - _rect.y}, _brushRadius * 2, RED);
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
        }
            default:
                break;
    }
}


void NoteArea::Draw() {
    DrawRectangleRec(_rect, Helium::Configuration::getInstance().ColorTheme.Foreground);

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
                int x = _rect.x;
                switch (t.type) {
                    case Helium::TokenType::HEADER: {
                        int headerFontSize = Helium::Configuration::getInstance().Formatting.GetFontSizeForHeader(std::stoi(t.attributes.at(ATTRIBUTE_HEADER_LEVEL)));
                        for(const Token& it : t.children) {
                            x += Utils::DrawInlineToken(it, x, y, headerFontSize);
                        }
                        x = _rect.x;
                        y += Helium::Configuration::getInstance().Formatting.GetLineHeight(headerFontSize);
                        break;
                    }
                    case Helium::TokenType::CODE: {
                        int codeFontSize = Helium::Configuration::getInstance().Formatting.Paragraph; // Assume a config entry for code font size
                        int maxWidth = 0; // Track maximum width for the rectangle
                        int lineHeight = Helium::Configuration::getInstance().Formatting.GetLineHeight(codeFontSize);
                        
                        // Calculate the width for all child tokens to set the rectangle dimensions
                        for (const Token& it : t.children) {
                            float width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.CodeFont, it.value.c_str(), codeFontSize, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
                            maxWidth = std::max(maxWidth, static_cast<int>(width));
                        }

                        // Draw the background rectangle for the code block
                        DrawRectangle(x - 2, y - 2, maxWidth + 4, (lineHeight * t.children.size()) + 4, Helium::Configuration::getInstance().ColorTheme.CodeBackgroundColor);

                        // Draw each line of code
                        for (const Token& it : t.children) {
                            DrawTextEx(Configuration::getInstance().Formatting.CodeFont, 
                                        it.value.c_str(), 
                                        {static_cast<float>(x), static_cast<float>(y)}, 
                                        Configuration::getInstance().Formatting.Paragraph, 
                                        Configuration::getInstance().Formatting.CharSpacing, 
                                        Configuration::getInstance().ColorTheme.CodeTextColor);
                            y += lineHeight; // Move down after rendering each line of code
                        }
                        
                        x = _rect.x; 
                        break;
                    }
                    case Helium::TokenType::QUOTE: {
                        int fontSize = Helium::Configuration::getInstance().Formatting.Paragraph;
                        int lineHeight = Helium::Configuration::getInstance().Formatting.GetLineHeight(fontSize);
                        float totalHeight = t.children.size() * lineHeight;
                        DrawRectangle(x, y, 5, totalHeight, Configuration::getInstance().ColorTheme.getQuoteColor(t.attributes.at(ATTRIBUTE_QUOTE_TYPE)));
                        x += 25;
                        
                        for(const Token& child : t.children) {
                            for(const Token& it : child.children) {
                                x += Utils::DrawInlineToken(it, x, y);
                            }
                            x = _rect.x + 25;
                            y += lineHeight;
                        }
                        break;
                    }

                    default:
                        for(const Token& it : t.children) {
                            x += Utils::DrawInlineToken(it, x, y);
                        }
                        x = _rect.x;
                        y += Helium::Configuration::getInstance().Formatting.GetLineHeight(Helium::Configuration::getInstance().Formatting.Paragraph);
                        
                        break;
                }
            }
            break;
        case Helium::NoteMode::WRITE: {
            int highlightStart = _cursor.GetHighlightStart(), highlightEnd = _cursor.GetHighlightEnd();
            float currentY = _rect.y;

            float lineHeight = Helium::Configuration::getInstance().Formatting.GetLineHeight(Helium::Configuration::getInstance().Formatting.Paragraph);

            int cursorPos = _cursor.GetPosition();
            float currentX = _rect.x;
            int totalChars = 0;  // Tracks total characters processed in all lines
            std::istringstream textStream(*_rawText);

            int lineIndex = 0;
            for (const std::string& wrappedLine : *wrappedLines) {
                int lineLength = wrappedLine.length();
                int lineEnd = totalChars + lineLength;
                int lineWidth = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, wrappedLine.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
                
                if(_cursor.IsHighlighting()) {
                    int hlStart = _cursor.GetHighlightStart(), hlEnd = _cursor.GetHighlightEnd();
                    int hlsc = _cursor.GetColumn(hlStart), hlsl = _cursor.GetLine(hlStart);
                    int hlec = _cursor.GetColumn(hlEnd), hlel = _cursor.GetLine(hlEnd);

                    // Case 1: Highlighting begins before line and ends after line
                    if (hlsl < lineIndex && hlel > lineIndex) {
                        DrawRectangle(_rect.x, currentY, lineWidth, lineHeight, Helium::Configuration::getInstance().ColorTheme.TextHighlight);
                    }
                    // Case 2: Highlighting starts in this line but ends after line
                    else if (hlsl == lineIndex && hlel > lineIndex && hlsc >= 0 && hlsc < lineLength) {
                        int offset = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, wrappedLine.substr(0, hlsc).c_str(), 
                                                    Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;

                        DrawRectangle(_rect.x + offset, currentY, lineWidth - offset, lineHeight, Helium::Configuration::getInstance().ColorTheme.TextHighlight);
                    }
                    // Case 3: Highlighting starts before this line but ends in this line
                    else if (hlsl < lineIndex && hlel == lineIndex && hlec <= lineLength) {
                        int offset = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, wrappedLine.substr(0, hlec).c_str(), 
                                                    Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;

                        DrawRectangle(_rect.x, currentY, offset, lineHeight, Helium::Configuration::getInstance().ColorTheme.TextHighlight);
                    }
                    // Case 4: Highlighting starts and ends in the same line
                    else if (hlsl == hlel && hlsl == lineIndex && hlsc >= 0 && hlsc < lineLength && hlec > hlsc && hlec <= lineLength) {
                        std::string highlightedText = wrappedLine.substr(hlsc, hlec - hlsc);
                        
                        if (!highlightedText.empty()) {
                            float highlightStartX = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, wrappedLine.substr(0, hlsc).c_str(), 
                                                                   Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;

                            float highlightWidth = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, highlightedText.c_str(), 
                                                                  Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;

                            DrawRectangle(_rect.x + highlightStartX, currentY, highlightWidth, lineHeight, Helium::Configuration::getInstance().ColorTheme.TextHighlight);
                        }
                    }
                }

                
                // Render the wrapped line
                DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, wrappedLine.c_str(), 
                           {currentX, currentY}, 
                           Helium::Configuration::getInstance().Formatting.Paragraph, 
                           Helium::Configuration::getInstance().Formatting.CharSpacing, 
                           Helium::Configuration::getInstance().ColorTheme.TextColor);

                currentY += lineHeight;  // Move down to the next line
                totalChars += wrappedLine.length();
                lineIndex++;
            }            
            if(_cursor.GetCurrentLineColumn() == 0) 
                caretX = _rect.x;
            else
                caretX = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont,
                                       wrappedLines->at(_cursor.GetCurrentLineIndex()).substr(0, _cursor.GetCurrentLineColumn()).c_str(),
                                       Helium::Configuration::getInstance().Formatting.Paragraph,
                                       Helium::Configuration::getInstance().Formatting.CharSpacing).x + _rect.x ;

            caretY = _rect.y + lineHeight * _cursor.GetCurrentLineIndex();
            // Draw the caret if it’s visible
            if (showCaret) {
                DrawRectangle(caretX, caretY, 2, lineHeight, Helium::Configuration::getInstance().ColorTheme.TextColor);
            }

            break;
        }
    }
    // Draw the texture
    DrawTextureRec(_texture.texture, { 0, 0, static_cast<float>(_texture.texture.width), -static_cast<float>(_texture.texture.height) }, { _rect.x, _rect.y }, WHITE);
    // Handle draw mode (if applicable)
    if (_mode == Helium::NoteMode::DRAW) {
        DrawCircleLines(GetMouseX(), GetMouseY(), _brushRadius, Helium::Configuration::getInstance().ColorTheme.BrushBorder);
    }
}


void NoteArea::SetRect(Rectangle rect) {
    _rect = rect;
}
std::string NoteArea::GetPath() {
    return path;
}
void NoteArea::SetPath(const std::string &path) {
    this->path = path;
}
void NoteArea::Save()
{
    if(path.empty())
        std::cerr << "No valid path to save note" << std::endl;
    else
        Serializer::SaveNote(path, *_rawText, _texture.texture);
}

std::string NoteArea::GetText() {
    return *_rawText;
}

Cursor* NoteArea::GetCursor() {
    return &_cursor;
}
void NoteArea::TryLoadNote(const std::string &path) {
    if (!Utils::IsSupportedNoteFileType(path)) {
        std::cerr << "Unsupported file type: " << path << std::endl;
        return; 
    }

    std::string extension = std::filesystem::path(path).extension().string();

    if(extension == ".note") {
        Texture2D temp;
        if(Serializer::LoadNote(path, *_rawText, temp))
        {
            _tokens = _tokenizer.tokenize(*_rawText);
            _texture = LoadRenderTexture(temp.width, temp.height);
            BeginTextureMode(_texture);
            // Clear texture
            BeginBlendMode(BLEND_CUSTOM);
            DrawRectangleRec({_rect.x, _rect.x, static_cast<float>(_texture.texture.width), static_cast<float>(_texture.texture.height)}, BLANK);
            EndBlendMode();

            DrawTextureV(temp, {0, 0}, WHITE);
            EndTextureMode();
            UnloadTexture(temp);
            isDirty = true;
            std::cout << "Note file loaded successfully from: " << path << std::endl;
        } else {
            std::cerr << "Failed to open .note file: " << path << std::endl;
        }
    } else {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open text file: " << path << std::endl;
            return; 
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        _rawText = std::make_shared<std::string>(content); 
        float height = std::max(_rect.height, static_cast<float>(GetScreenHeight()));
        UnloadRenderTexture(_texture);
        _texture = LoadRenderTexture(_rect.width, GetScreenHeight());
        // Clear texture
        BeginDrawing();
        BeginTextureMode(_texture);
        BeginBlendMode(BLEND_CUSTOM);
        DrawRectangleRec({_rect.x, _rect.x, static_cast<float>(_texture.texture.width), height}, BLANK);
        EndBlendMode();
        EndTextureMode();
        BeginDrawing();

        file.close();
        isDirty = true;
        std::cout << "Raw text loaded successfully as a note from: " << path << std::endl;
    }
    SetMode(Helium::NoteMode::READ);
    this->path = path;
}
}
