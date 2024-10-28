#include "utils.hpp"
#include "raylib.h"
#include "tokenizer.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace Utils {

void WrapText(std::string text, std::shared_ptr<std::vector<std::string>> output) {
    std::string line;
    std::istringstream textStream(text);
    output->clear();
    
    // Process the raw text line by line
    while (std::getline(textStream, line)) {
        // Check if the entire line fits without wrapping
        if (MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, line.c_str(), Helium::Configuration::getInstance().Formatting.Paragraph, Helium::Configuration::getInstance().Formatting.CharSpacing).x < Helium::Configuration::getInstance().MaxNoteWidth) {
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
            if (currWidth + width > Helium::Configuration::getInstance().MaxNoteWidth) {
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
        if (MeasureTextEx(Helium::Configuration::getInstance().Formatting.DefaultFont, line.c_str(), fontSize, Helium::Configuration::getInstance().Formatting.CharSpacing).x < Helium::Configuration::getInstance().MaxNoteWidth) {
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
            if (currWidth + width > Helium::Configuration::getInstance().MaxNoteWidth) {
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
bool IsFile(const std::string& path) {
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
}
