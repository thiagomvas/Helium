#ifndef UTILS_HPP
#define UTILS_HPP

#include "Configuration.hpp"
#include "raylib.h"
#include "tokenizer.hpp"
#include <memory>
#include <string>
#include <vector>
#include "Button.hpp"
#include <filesystem>
namespace Utils {
void WrapText(std::string text, std::shared_ptr<std::vector<std::string>> output);
std::vector<std::string> WrapText(const std::string& text, Font font, float fontSize, float charSpacing, int maxWidth); 
void WrapText(const std::string& text, std::shared_ptr<std::vector<std::string>> output, float fontSize);
int DrawInlineToken(const Helium::Token& it, int& x, int y, float fontSize);
int DrawInlineToken(const Helium::Token& it, int& x, int y);
int GetLineHeight(Font font, int fontSize);
void DrawText(const std::string& text, Vector2 pos);
void DrawText(const std::string& text, Vector2 pos, int fontSize);
void DrawText(Font font, const std::string& text, Vector2 pos, int fontSize);

bool IsFile(const std::string& path);
bool IsSupportedNoteFileType(const std::string& path);
Color ParseHexColor(const std::string& hexColor);
std::string ColorToHex(const Color& color);
std::string CleanseText(const std::string& text);

std::vector<UI::Button> LoadFilesInDirectoryAsButtons(const std::filesystem::path& currentPath, const std::string& fileFilter);

}

#endif

