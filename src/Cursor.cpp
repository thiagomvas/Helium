
#include "Cursor.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <vector>

namespace Helium {

Cursor::Cursor() : _position(0), _highlightMode(false), _highlightActive(false) {}

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

void Cursor::MoveUp() {
    size_t currLineStart = text->rfind('\n', _position - 1);

    if (currLineStart == std::string::npos) {
        currLineStart = 0;
    } else {
        currLineStart++;
    }

    size_t prevLineStart = text->rfind('\n', currLineStart - 2);
    if (prevLineStart == std::string::npos) {
        prevLineStart = 0;
    } else {
        prevLineStart++;
    }

    size_t currColumn = _position - currLineStart;
    size_t prevLineEnd = text->find('\n', prevLineStart);
    if (prevLineEnd == std::string::npos) {
        prevLineEnd = text->length();
    }

    _position = prevLineStart + std::min(currColumn, prevLineEnd - prevLineStart);

    if (_highlightMode) {
        _highlightEnd = _position;
    } else {
        _highlightActive = false;
    }
}

void Cursor::MoveDown() {
    size_t currLineEnd = text->find('\n', _position);
    if (currLineEnd == std::string::npos) {
        _position = text->length();
        return;
    }

    size_t nextLineStart = currLineEnd + 1;
    size_t nextLineEnd = text->find('\n', nextLineStart);
    if (nextLineEnd == std::string::npos) {
        nextLineEnd = text->length();
    }

    size_t currColumn = _position - (text->rfind('\n', _position - 1) + 1);

    _position = nextLineStart + std::min(currColumn, nextLineEnd - nextLineStart);

    if (_highlightMode) {
        _highlightEnd = _position;
    } else {
        _highlightActive = false;
    }
}

void Cursor::BeginHighlight() {
    _highlightStart = _position;
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

void Cursor::SetTextPter(std::string* text, std::vector<std::string>* wrappedLines) {
    this->text = text;
    this->wrappedLines = wrappedLines;
}
} // namespace Helium

