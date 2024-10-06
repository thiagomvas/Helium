#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <string>
namespace Helium {

class Cursor {
public:
	Cursor();
	void MoveToStart();
	void MoveToEnd(const std::string& text);
	void MoveLeft(const std::string& text);
	void MoveRight(const std::string& text);
	int GetPosition();
private:
	int _position = 0;
};

}

#endif