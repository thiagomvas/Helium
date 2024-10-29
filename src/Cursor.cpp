
#include "Cursor.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

namespace Helium {

Cursor::Cursor() : _position(0), _highlightMode(false), _highlightActive(false) {}

void Cursor::Goto(int position) {
    _position = position;
    if(_highlightMode) _highlightEnd = _position;
}

std::string Cursor::GetWordAtCursor() const {
    if (!text || text->empty()) return ""; // Return empty if text is null or empty

    int start = _position;
    int end = _position;
    int textSize = text->size();

    // Move start backwards until it reaches the beginning of the word
    while (start > 0 && !std::isspace((*text)[start - 1]) && !std::ispunct((*text)[start - 1])) {
        --start;
    }

    // Move end forwards until it reaches the end of the word
    while (end < textSize && !std::isspace((*text)[end]) && !std::ispunct((*text)[end])) {
        ++end;
    }

    // Extract and return the word
    return text->substr(start, end - start);
}

void Cursor::ReplaceWordWithMacro(const std::unordered_map<std::string, std::string>& macros) {
    std::string word = GetWordAtCursor();
    if (macros.find(word) != macros.end()) {
        std::string replacement = macros.at(word);
        int start = _position - word.length(); 
        int end = _position; 

        (*text).replace(start, end - start, replacement);
        _position = start + replacement.length();
    }
}

void Cursor::MoveToStart() {
    _position = 0;
    if (!_highlightMode) _highlightActive = false;  // Stop highlighting if not in highlight mode
}

void Cursor::MoveToEnd() {
    _position = text->length();
    if (!_highlightMode) _highlightActive = false;  // Stop highlighting if not in highlight mode
}

void Cursor::MoveLeft() {
    if (_position > 0)
        _position--;
    
    if (_highlightMode) {
        _highlightEnd = _position;  // Update highlight range
    } else {
        _highlightActive = false;   // Stop highlighting after movement if not in highlight mode
    }
}

bool Cursor::IsHighlighting() {
    return _highlightMode || _highlightActive;
}

void Cursor::MoveRight() {
    if (_position < text->length())
        _position++;
    
    if (_highlightMode) {
        _highlightEnd = _position;  // Update highlight range
    } else {
        _highlightActive = false;   // Stop highlighting after movement if not in highlight mode
    }
}




int Cursor::GetCurrentLineIndex() const {
    int totalChars = 0;

    if (wrappedLines->empty())
        return 0;

    for (int lineIndex = 0; lineIndex < wrappedLines->size(); lineIndex++) {
        int lineLength = wrappedLines->at(lineIndex).size();
        // Calculate the end of the current line with the newline character
        int lineEndChar = totalChars + lineLength + 1; // Add 1 for the newline

        if (_position >= totalChars && _position < lineEndChar) {
            return lineIndex; // Use < to avoid including the newline in the last line
        }

        totalChars += lineLength + 1; // Increment totalChars by lineLength + 1 (for newline)
    }
    return wrappedLines->size() - 1; // If position is out of bounds, return last line index
}
int Cursor::GetLine(int position) const {
    int totalChars = 0;

    if (wrappedLines->empty())
        return 0;

    for (int lineIndex = 0; lineIndex < wrappedLines->size(); lineIndex++) {
        int lineLength = wrappedLines->at(lineIndex).size();
        int lineEndChar = totalChars + lineLength + 1; 

        if (_position >= totalChars && position < lineEndChar) {
            return lineIndex; 
        }

        totalChars += lineLength + 1; 
    }
    return wrappedLines->size() - 1; 
}




int Cursor::GetCurrentLineColumn() const {
    if (wrappedLines->empty())
        return 0;

    int totalChars = 0;

    for (const std::string& wrappedLine : *wrappedLines) {
        int lineLength = wrappedLine.length();
        int lineEndChar = totalChars + lineLength + 1; // Add 1 for the newline

        if (_position >= totalChars && _position < lineEndChar) {
            return _position - totalChars; // Calculate the column correctly
        }

        totalChars += lineLength + 1; // Increment totalChars by lineLength + 1 (for newline)
    }

    // If position is out of bounds, return the length of the last line
    return wrappedLines->back().length();
}
int Cursor::GetColumn(int position) const {
    if (wrappedLines->empty())
        return 0;

    int totalChars = 0;

    for (const std::string& wrappedLine : *wrappedLines) {
        int lineLength = wrappedLine.length();
        int lineEndChar = totalChars + lineLength + 1; 

        if (position >= totalChars && position < lineEndChar) {
            return position - totalChars; 
        }

        totalChars += lineLength + 1; 
    }

    return wrappedLines->back().length();
}




void Cursor::MoveUp() {
    int lineIndex = GetCurrentLineIndex(); // Get the current line index
    if (lineIndex > 0) {
        // Get the length of the current line
        size_t currLineLength = wrappedLines->at(lineIndex).size();
        size_t currColumn = _position - (lineIndex > 0 ? std::accumulate(wrappedLines->begin(), wrappedLines->begin() + lineIndex, 0, 
            [](size_t sum, const std::string& line) { return sum + line.size(); }) : 0);

        // Move to the previous line
        --lineIndex;

        // Get the length of the previous line
        size_t prevLineLength = wrappedLines->at(lineIndex).size();
        
        // Update cursor position
        _position = std::min(currColumn, prevLineLength);
        _position += std::accumulate(wrappedLines->begin(), wrappedLines->begin() + lineIndex, 0, 
            [](size_t sum, const std::string& line) { return sum + line.size(); });
    }

    // Update highlighting
    if (_highlightMode) {
        _highlightEnd = _position;
    } else {
        _highlightActive = false;
    }
}

void Cursor::MoveDown() {
    int lineIndex = GetCurrentLineIndex(); // Get the current line index
    if (lineIndex < wrappedLines->size() - 1) {
        // Get the length of the current line
        size_t currLineLength = wrappedLines->at(lineIndex).size();
        size_t currColumn = _position - (lineIndex > 0 ? std::accumulate(wrappedLines->begin(), wrappedLines->begin() + lineIndex, 0, 
            [](size_t sum, const std::string& line) { return sum + line.size(); }) : 0);

        // Move to the next line
        ++lineIndex;

        // Update cursor position
        _position = std::min(currColumn, wrappedLines->at(lineIndex).size());
        _position += std::accumulate(wrappedLines->begin(), wrappedLines->begin() + lineIndex, 0, 
            [](size_t sum, const std::string& line) { return sum + line.size(); });
    }

    // Update highlighting
    if (_highlightMode) {
        _highlightEnd = _position;
    } else {
        _highlightActive = false;
    }
}
void Cursor::BeginHighlight() {
    _highlightStart = _position;
    _highlightEnd = _position;
    _highlightMode = true;
    _highlightActive = true;  // Start highlighting actively
}

void Cursor::EndHighlight() {
    _highlightEnd = _position;
    _highlightMode = false;
    // Keep the highlight active after ending the highlight mode until movement occurs
}

std::string Cursor::GetHighlightedText() {
    if (_highlightStart < 0 || _highlightEnd < 0) return "";
    if (_highlightStart > _highlightEnd)
        return text->substr(_highlightEnd, _highlightStart - _highlightEnd);
    else
        return text->substr(_highlightStart, _highlightEnd - _highlightStart);
}

int Cursor::GetPosition() {
    return _position;
}

int Cursor::GetHighlightStart() {
    if (_highlightStart < _highlightEnd)
        return _highlightStart;
    else return _highlightEnd;
}

int Cursor::GetHighlightEnd() {
    if (_highlightEnd < _highlightStart)
        return _highlightStart;
    else return _highlightEnd;
}

void Cursor::Deselect() {
    _highlightStart = -1;
    _highlightEnd = -1;
    _highlightActive = false;
    _highlightMode = false;
}

void Cursor::SetTextPter(std::shared_ptr<std::string> text, std::shared_ptr<std::vector<std::string>> wrappedLines) {
    this->text = text;
    this->wrappedLines = wrappedLines;
}
} // namespace Helium

