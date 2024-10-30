#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
namespace Helium {

class Cursor {
public:
	Cursor();
	void MoveToStart();
	void MoveToEnd();
	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();
	void MoveToEndOfLine();
	void MoveToStartOfLine();
	int GetPosition();
	int GetHighlightStart();
	int GetHighlightEnd();
	std::string GetHighlightedText();
	bool IsHighlighting();
	void BeginHighlight();
	void EndHighlight();
	void Deselect();
	void SetTextPter(std::shared_ptr<std::string> text, std::shared_ptr<std::vector<std::string>> wrappedLines);
	void Goto(int position);
	std::string GetWordAtCursor() const;
	void ReplaceWordWithMacro(const std::unordered_map<std::string, std::string>& macros);
	int GetCurrentLineIndex() const;
	int GetCurrentLineColumn() const;
	int GetColumn(int position) const;
	int GetLine(int position) const;
private:
	std::shared_ptr<std::string> text;
	std::shared_ptr<std::vector<std::string>> wrappedLines;
	int _position = 0;
	bool _highlightMode = false;
	bool _highlightActive = false;
	int _highlightStart = -1;
	int _highlightEnd = -1;
};

}

#endif
