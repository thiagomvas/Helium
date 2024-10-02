#include "Cursor.hpp"
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

void Cursor::MoveRight(const std::string& text) {
    if(_position < text.length())
        _position++;
}

int Cursor::GetPosition() {
    return _position;
}



}
