#include "Cursor.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>


namespace Helium {

Cursor::Cursor() : _position(0) {}

void Cursor::MoveToStart() {
    _position = 0;
}

void Cursor::MoveToEnd(const std::string& text) {
    _position = text.length();
}

void Cursor::MoveLeft(const std::string& text) {
    if(_position > 0)
        _position--;
}

bool Cursor::IsHighlighting() {
    return _highlightMode;
}

void Cursor::MoveRight(const std::string& text) {
    if(_position < text.length())
        _position++;
}

void Cursor::MoveUp(const std::string& text) {
    size_t currLineStart = text.rfind('\n', _position - 1);

    if(currLineStart == std::string::npos) {
        currLineStart = 0;
    } else {
        currLineStart++;
    }

    size_t prevLineStart = text.rfind('\n', currLineStart - 2);
    if(prevLineStart == std::string::npos) {
        prevLineStart = 0;
    } else {
        prevLineStart++;
    }

    size_t currColumn = _position - currLineStart;
    size_t prevLineEnd = text.find('\n', prevLineStart);
    if(prevLineEnd == std::string::npos) {
        prevLineEnd = text.length();
    }

    _position = prevLineStart + std::min(currColumn, prevLineEnd - prevLineStart);
}

void Cursor::MoveDown(const std::string& text) {
    size_t currLineEnd = text.find('\n', _position);
    if (currLineEnd == std::string::npos) {
        _position = text.length();
        return;
    }

    size_t nextLineStart = currLineEnd + 1;
    size_t nextLineEnd = text.find('\n', nextLineStart);
    if (nextLineEnd == std::string::npos) {
        nextLineEnd = text.length();
    }

    size_t currColumn = _position - (text.rfind('\n', _position - 1) + 1);

    _position = nextLineStart + std::min(currColumn, nextLineEnd - nextLineStart);
}

void Cursor::BeginHighlight() {
    _highlightStart = _position;
    _highlightMode = true;
}

void Cursor::EndHighlight() {
    _highlightEnd = _position;
    _highlightMode = false;
}

std::string Cursor::GetHighlightedText(const std::string& text) {
    if(_highlightStart < 0 || _highlightEnd < 0) return "";
    if(_highlightStart > _highlightEnd)
        return text.substr(_highlightEnd, _highlightStart - _highlightEnd);
    else
        return text.substr(_highlightStart, _highlightEnd - _highlightStart);
}
int Cursor::GetPosition() {
    return _position;
}



}
