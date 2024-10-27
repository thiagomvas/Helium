#include "utils.hpp"
#include "raylib.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace Utils {

void WrapText(std::string text, std::shared_ptr<std::vector<std::string>> output, std::shared_ptr<Helium::Configuration> _config) {
    std::string line;
    std::istringstream textStream(text);
    output->clear();
    
    // Process the raw text line by line
    while (std::getline(textStream, line)) {
        // Check if the entire line fits without wrapping
        if (MeasureTextEx(_config->Formatting.DefaultFont, line.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x < _config->MaxNoteWidth) {
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
            int width = MeasureTextEx(_config->Formatting.DefaultFont, segment.c_str(), _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x;

            // If currLine is not empty, we need to consider the space before adding the segment
            if (!currLine.empty()) {
                width += MeasureTextEx(_config->Formatting.DefaultFont, " ", _config->Formatting.Paragraph, _config->Formatting.CharSpacing).x; // Width of a space
            }

            // If the segment doesn't fit, wrap to the next line
            if (currWidth + width > _config->MaxNoteWidth) {
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


}
