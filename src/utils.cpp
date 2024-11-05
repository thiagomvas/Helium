#include "utils.hpp"
#include "raylib.h"
#include "tokenizer.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include "Button.hpp"
#include <unordered_set>

namespace fs = std::filesystem;

namespace Utils {

void WrapText(std::string text, std::shared_ptr<std::vector<std::string>> output) {
    std::string line;
    std::istringstream textStream(text);
    output->clear();
    
    // Process the raw text line by line
    while (std::getline(textStream, line)) {
        // Check if the entire line fits without wrapping
        if (MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, line.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x < Helium::Configuration::getInstance().GetScaledNoteWidth()) {
            output->push_back(line);  // Line fits without wrapping
            continue;
        }

        std::string currLine;
        int currWidth = 0;
        size_t start = 0;

        while (start < line.length()) {
            // Find the next space or the end of the string
            size_t end = line.find_first_of(' ', start);

            // If no more spaces are found, take the rest of the line
            if (end == std::string::npos) {
                end = line.length();
            }

            // Extract the word including spaces
            std::string segment = line.substr(start, end - start);
            // Measure width including the segment's width and a space if currLine is not empty
            int width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, segment.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;

            // If currLine is not empty, we need to consider the space before adding the segment
            if (!currLine.empty()) {
                width += MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, " ", Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x; // Width of a space
            }

            // If the segment doesn't fit, wrap to the next line
            if (currWidth + width > Helium::Configuration::getInstance().GetScaledNoteWidth()) {
                if (!currLine.empty()) {
                    output->push_back(currLine);  // Only push if there's content in currLine
                }
                currLine = segment;  // Start a new line with the current segment
                currWidth = width;  // Update width to the current segment
            } else {
                // Append the segment to currLine
                if (!currLine.empty()) {
                    currLine.append(" ");  // Add a space before the next segment if currLine is not empty
                }
                currLine.append(segment);  // Add the segment to the current line
                currWidth += width;  // Update the width
            }

            // Move to the next character after the space
            start = end + 1;  // Skip the space character
        }

        // Push any remaining line after processing all segments
        if (!currLine.empty()) {
            output->push_back(currLine);
        }
    }
}
void WrapText(const std::string& text, std::shared_ptr<std::vector<std::string>> output, float fontSize) {
    std::string line;
    std::istringstream textStream(text);
    output->clear();
    
    // Process the raw text line by line
    while (std::getline(textStream, line)) {
        // Check if the entire line fits without wrapping
        if (MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, line.c_str(), fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing).x < Helium::Configuration::getInstance().GetScaledNoteWidth()) {
            output->push_back(line);  // Line fits without wrapping
            continue;
        }

        std::string currLine;
        int currWidth = 0;
        size_t start = 0;

        while (start < line.length()) {
            // Find the next space or the end of the string
            size_t end = line.find_first_of(' ', start);

            // If no more spaces are found, take the rest of the line
            if (end == std::string::npos) {
                end = line.length();
            }

            // Extract the word including spaces
            std::string segment = line.substr(start, end - start);
            // Measure width including the segment's width and a space if currLine is not empty
            int width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, segment.c_str(), fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing).x;

            // If currLine is not empty, we need to consider the space before adding the segment
            if (!currLine.empty()) {
                width += MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, " ", fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing).x; // Width of a space
            }

            // If the segment doesn't fit, wrap to the next line
            if (currWidth + width > Helium::Configuration::getInstance().GetScaledNoteWidth()) {
                if (!currLine.empty()) {
                    output->push_back(currLine);  // Only push if there's content in currLine
                }
                currLine = segment;  // Start a new line with the current segment
                currWidth = width;  // Update width to the current segment
            } else {
                // Append the segment to currLine
                if (!currLine.empty()) {
                    currLine.append(" ");  // Add a space before the next segment if currLine is not empty
                }
                currLine.append(segment);  // Add the segment to the current line
                currWidth += width;  // Update the width
            }

            // Move to the next character after the space
            start = end + 1;  // Skip the space character
        }

        // Push any remaining line after processing all segments
        if (!currLine.empty()) {
            output->push_back(currLine);
        }
    }
}
std::vector<std::string> WrapText(const std::string& text, Font font, float fontSize, float charSpacing, int maxWidth) {
    std::vector<std::string> output;
    std::string line;
    std::istringstream textStream(text);
    
    while (std::getline(textStream, line)) {
        if (MeasureTextEx(font, line.c_str(), fontSize, charSpacing).x < maxWidth) {
            output.push_back(line);
            continue;
        }

        std::string currLine;
        int currWidth = 0;
        size_t start = 0;

        while (start < line.length()) {
            size_t end = line.find_first_of(' ', start);
            if (end == std::string::npos) {
                end = line.length();
            }

            std::string segment = line.substr(start, end - start);
            int width = MeasureTextEx(font, segment.c_str(), fontSize, charSpacing).x;

            if (!currLine.empty()) {
                width += MeasureTextEx(font, " ", fontSize, charSpacing).x;
            }

            if (currWidth + width > maxWidth) {
                if (!currLine.empty()) {
                    output.push_back(currLine);
                }
                currLine = segment;
                currWidth = width;
            } else {
                if (!currLine.empty()) {
                    currLine.append(" ");
                }
                currLine.append(segment);
                currWidth += width;
            }

            start = end + 1;
        }

        if (!currLine.empty()) {
            output.push_back(currLine);
        }
    }

    return output;
}
int DrawInlineToken(const Helium::Token& it, int& x, int y, float fontSize) {
    int width;
    switch (it.type) {
        case Helium::TokenType::BOLD: {
            DrawTextEx(Helium::Configuration::getInstance().Formatting.BoldFont, it.value.c_str(), { (float)x, (float)y }, fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
            width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.BoldFont, it.value.c_str(), fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            break;
        }

        case Helium::TokenType::ITALIC: {
            DrawTextEx(Helium::Configuration::getInstance().Formatting.ItalicFont, it.value.c_str(), { (float)x, (float)y }, fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
            width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.ItalicFont, it.value.c_str(), fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            break;
        }

        case Helium::TokenType::STRIKETHROUGH: {
            DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), { (float)x, (float)y }, fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
            int xOffset = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            DrawLineEx({ (float)x, (float)y + fontSize * 0.5f }, { (float)(x + xOffset), (float)y + fontSize * 0.5f }, Helium::Configuration::getInstance().Formatting.StrikethroughWidth, Helium::Configuration::getInstance().ColorTheme.TextColor);
            width = xOffset;
            break;
        }

        default: {
            DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), { (float)x, (float)y }, fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
            width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            break;
        }
    }
    return width;
}

int DrawInlineToken(const Helium::Token& it, int& x, int y) {
    int width;
    switch (it.type) {
        case Helium::TokenType::COLORCHIP: {
            int lineHeight = Helium::Configuration::getInstance().Formatting.GetLineHeight(Helium::Configuration::getInstance().Formatting.Paragraph);
            width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.CodeFont, it.value.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            DrawRectangle(x - 2, y - 2, width + 4 + lineHeight, Helium::Configuration::getInstance().Formatting.Paragraph + 4, Helium::Configuration::getInstance().ColorTheme.CodeBackgroundColor);
            DrawCircle(x + width +  lineHeight * 0.5f, y + lineHeight * 0.5f, lineHeight * 0.25f, ParseHexColor(it.attributes.at(Helium::ATTRIBUTE_COLORCHIP_COLOR)));
            // Draw code text
            DrawTextEx(Helium::Configuration::getInstance().Formatting.CodeFont, it.value.c_str(), { (float)x, (float)y }, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.CodeTextColor);
            width += 4 + lineHeight;
            break;
        }
        case Helium::TokenType::BOLD: {
            DrawTextEx(Helium::Configuration::getInstance().Formatting.BoldFont, it.value.c_str(), { (float)x, (float)y }, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
            width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.BoldFont, it.value.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            break;
        }

        case Helium::TokenType::ITALIC: {
            DrawTextEx(Helium::Configuration::getInstance().Formatting.ItalicFont, it.value.c_str(), { (float)x, (float)y }, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
            width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.ItalicFont, it.value.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            break;
        }

        case Helium::TokenType::STRIKETHROUGH: {
            DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), { (float)x, (float)y }, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
            int xOffset = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            DrawLineEx({ (float)x, (float)y + Helium::Configuration::getInstance().Formatting.Paragraph * 0.5f }, { (float)(x + xOffset), (float)y + Helium::Configuration::getInstance().Formatting.Paragraph * 0.5f }, Helium::Configuration::getInstance().Formatting.StrikethroughWidth, Helium::Configuration::getInstance().ColorTheme.TextColor);
            width = xOffset;
            break;
        }

        case Helium::TokenType::INLINECODE: { 
            width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.CodeFont, it.value.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            DrawRectangle(x - 2, y - 2, width + 4, Helium::Configuration::getInstance().Formatting.Paragraph + 4, Helium::Configuration::getInstance().ColorTheme.CodeBackgroundColor);

            // Draw code text
            DrawTextEx(Helium::Configuration::getInstance().Formatting.CodeFont, it.value.c_str(), { (float)x, (float)y }, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.CodeTextColor);
            break;
        }

        case Helium::TokenType::COLOREDTEXT: {
            DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), { (float)x, (float)y }, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, Utils::ParseHexColor(it.attributes.at(Helium::ATTRIBUTE_COLOREDTEXT_COLOR)));
            width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            break;
        }
        default: {
            DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), { (float)x, (float)y }, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
            width = MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, it.value.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x;
            break;
        }
    }
    return width;
}
int GetLineHeight(Font font, int fontSize) {
	return MeasureTextEx(font, "A", fontSize, 1).y;
}
void DrawText(const std::string &text, Vector2 pos) {
    DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, text.c_str(), pos, Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
}
void DrawText(const std::string &text, Vector2 pos, int fontSize) {
    DrawTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, text.c_str(), pos, fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
}
void DrawText(Font font, const std::string &text, Vector2 pos, int fontSize) {
    DrawTextEx(font, text.c_str(), pos, fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing, Helium::Configuration::getInstance().ColorTheme.TextColor);
}
bool IsFile(const std::string &path)
{
    fs::path filePath(path); // Convert the string to a filesystem path
    return fs::exists(filePath) && fs::is_regular_file(filePath); // Check if it exists and is a regular file
}

bool IsSupportedNoteFileType(const std::string& path) {
    fs::path filePath(path); 

    if (!fs::exists(filePath) || !fs::is_regular_file(filePath)) {
        return false; 
    }

    std::string extension = filePath.extension().string();
    return (extension == ".txt" || extension == ".note" || extension == ".md");
}
Color ParseHexColor(const std::string& hexColor) {
    // Ensure the color string starts with '#' and is followed by exactly 6 hex digits
    if (hexColor.length() != 7 || hexColor[0] != '#') {
        return BLANK;
    }

    // Convert hex color components to integers
    int r = std::stoi(hexColor.substr(1, 2), nullptr, 16);
    int g = std::stoi(hexColor.substr(3, 2), nullptr, 16);
    int b = std::stoi(hexColor.substr(5, 2), nullptr, 16);

    // Return the Raylib Color with alpha set to 255 (fully opaque)
    return { static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b), 255 };
}
std::string ColorToHex(const Color &color) {
    std::ostringstream oss;
    oss << "#"
        << std::hex << std::setw(2) << std::setfill('0') << (color.r & 0xFF)
        << std::setw(2) << std::setfill('0') << (color.g & 0xFF)
        << std::setw(2) << std::setfill('0') << (color.b & 0xFF);
    return oss.str();
}
std::string CleanseText(const std::string &text) {
    std::string cleansedText = text; 

    for (size_t i = 0; i < cleansedText.length(); ) {
        if (cleansedText[i] == '\r') {
            cleansedText.erase(i, 1);
        } else {
            ++i;
        }
    }

    return cleansedText;
}

std::vector<UI::Button> LoadFilesInDirectoryAsButtons(const std::filesystem::path& currentPath, const std::string& fileFilter)
{
    std::vector<UI::Button> fileButtons;

    // Parse fileFilter into a set of extensions if it is not empty
    std::unordered_set<std::string> allowedExtensions;
    if (!fileFilter.empty()) {
        std::istringstream filterStream(fileFilter);
        std::string extension;
        while (std::getline(filterStream, extension, ',')) {
            extension.erase(0, extension.find_first_not_of(" \t"));
            extension.erase(extension.find_last_not_of(" \t") + 1);
            allowedExtensions.insert("." + extension); // Prefix with '.' for consistency
        }
    }

    if (fs::exists(currentPath) && fs::is_directory(currentPath)) {
        std::vector<UI::Button> directories;
        std::vector<UI::Button> files;

        for (const auto& entry : fs::directory_iterator(currentPath)) {
            std::string entryName = entry.path().filename().string();
            UI::Button button(
                entryName,
                Helium::Configuration::getInstance().Formatting.Paragraph,
                Helium::Configuration::getInstance().ColorTheme.TextColor,
                Helium::Configuration::getInstance().ColorTheme.Foreground);

            if (fs::is_directory(entry)) {
                button.SetText(entryName + "/"); // Add "/" to indicate a directory
                directories.push_back(button);
            } else {
                // If fileFilter is specified, check the extension
                if (allowedExtensions.empty() ||
                    allowedExtensions.count(entry.path().extension().string()) > 0) {
                    files.push_back(button);
                }
            }
        }

        // Add directories first, followed by files
        fileButtons.insert(fileButtons.end(), directories.begin(), directories.end());
        fileButtons.insert(fileButtons.end(), files.begin(), files.end());
    } else {
        std::cerr << "Directory does not exist: " << currentPath << std::endl;
    }

    return fileButtons;
}
}
