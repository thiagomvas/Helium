#ifndef FORMATTING_HPP
#define FORMATTING_HPP

#include "raylib.h"
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>
namespace Helium {
class Formatting {
public:
	Formatting() {}
	int H1 = 48;
	int H2 = 40;
	int H3 = 32;
	int H4 = 26;
	int H5 = 22;
	int Paragraph = 20;
	int StrikethroughWidth = 2;
	std::string DefaultFontPath = "resources/Roboto-Bold.ttf";
	std::string BoldFontPath = "resources/Roboto-Black.ttf";
	std::string ItalicFontPath = "resources/Roboto-BoldItalic.ttf";
	float CharSpacing = 1;

	// Loaded during runtime
	// !! DO NOT SERIALIZE !!
	Font DefaultFont;
	Font BoldFont;
	Font ItalicFont;
	
	int GetLineHeight(int fontSize) {
		auto it = lineHeightMap.find(fontSize);
		if(it != lineHeightMap.end()) {
			return it->second;
		} else {
			int height = MeasureTextEx(DefaultFont, "|", fontSize, 1).y;
			lineHeightMap[fontSize] = height;
			return height;
		}
	}
	

	int GetFontSizeForHeader(int level) {
		switch(level) {
			case 1: return H1;
			case 2: return H2;
			case 3: return H3;
			case 4: return H4;
			case 5: return H5;
			default: return Paragraph;
		}
	}
	
	void loadFonts() {
		DefaultFont = LoadFontEx(DefaultFontPath.c_str(), 64, 0, 250);
		BoldFont = LoadFontEx(BoldFontPath.c_str(), 64, 0, 250);
		ItalicFont = LoadFontEx(ItalicFontPath.c_str(), 64, 0, 250);
	}

	std::string serialize() const {
		std::ostringstream oss;

		oss << "H1:" << std::to_string(H1) << '\n';
		oss << "H2:" << std::to_string(H2) << '\n';
		oss << "H3:" << std::to_string(H3) << '\n';
		oss << "H4:" << std::to_string(H4) << '\n';
		oss << "H5:" << std::to_string(H5) << '\n';
		oss << "Paragraph:" << std::to_string(Paragraph) << '\n';
		oss << "StrikethroughWidth:" << std::to_string(StrikethroughWidth) << '\n';
		oss << "CharSpacing:" << std::to_string(CharSpacing) << '\n';
		oss << "DefaultFontPath:" << DefaultFontPath << '\n';
		oss << "BoldFontPath:" << BoldFontPath << '\n';
		oss << "ItalicFontPath:" << ItalicFontPath << '\n';

		return oss.str();
	}
	
	void deserialize(const std::string& data) {
		std::istringstream iss(data);
		std::string line;

		std::unordered_map<std::string, std::function<void(const std::string&)>> handlers{
			{"H1", [this](const std::string& value) { H1 = std::stoi(value); }},
			{"H2", [this](const std::string& value) { H2 = std::stoi(value); }},
			{"H3", [this](const std::string& value) { H3 = std::stoi(value); }},
			{"H4", [this](const std::string& value) { H4 = std::stoi(value); }},
			{"H5", [this](const std::string& value) { H5 = std::stoi(value); }},
			{"Paragraph", [this](const std::string& value) { Paragraph = std::stoi(value); }},
			{"StrikethroughWidth", [this](const std::string& value) { StrikethroughWidth = std::stoi(value); }},
			{"CharSpacing", [this](const std::string&value) {CharSpacing = std::stoi(value); }},
			{"DefaultFontPath", [this](const std::string& value) { DefaultFontPath = value; }},
			{"BoldFontPath", [this](const std::string& value) {BoldFontPath = value;}},
			{"ItalicFontPath", [this](const std::string& value) {ItalicFontPath = value;}}
		};

		while (std::getline(iss, line)) {
			auto delimiterPos = line.find(':');
			if (delimiterPos != std::string::npos) {
				std::string key = line.substr(0, delimiterPos);
				std::string value = line.substr(delimiterPos + 1);

				auto handler = handlers.find(key);
				if (handler != handlers.end()) {
					handler->second(value);
				}
			}
		}
	}
	void unloadResources() {
		UnloadFont(DefaultFont);
		UnloadFont(BoldFont);
		UnloadFont(ItalicFont);
	}

private:
	std::unordered_map<int, int> lineHeightMap;

}; 
}

#endif
