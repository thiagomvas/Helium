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
	void MoveUp(const std::string& text);
	void MoveDown(const std::string& text);
	int GetPosition();
	std::string GetHighlightedText(const std::string& text);
	bool IsHighlighting();
	void BeginHighlight();
	void EndHighlight();
private:
	int _position = 0;
	bool _highlightMode = false;
	int _highlightStart = -1;
	int _highlightEnd = -1;
};

}

#endif
