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
	int GetHighlightStart();
	int GetHighlightEnd();
	std::string GetHighlightedText(const std::string& text);
	bool IsHighlighting();
	void BeginHighlight();
	void EndHighlight();
	void Deselect();
private:
	int _position = 0;
	bool _highlightMode = false;
	bool _highlightActive = false;
	int _highlightStart = -1;
	int _highlightEnd = -1;
};

}

#endif
