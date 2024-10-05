#ifndef COLORTHEME_HPP
#define COLORTHEME_HPP

#include "raylib.h"
#include "utils.hpp"
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>
namespace Helium {
class ColorTheme {
public: 
	Color Background = { 12, 12, 12, 255 };
	Color Foreground = { 20, 20, 20, 255 };
	Color TextColor = { 200, 200, 200, 255 };
	Color BrushBorder = { 200, 200, 200, 255 };

	std::string serialize() {
	std::ostringstream oss;
		oss << "Background:" << Utils::Serialize(Background) << '\n';
		oss << "Foreground:" << Utils::Serialize(Foreground) << '\n';
		oss << "TextColor:" << Utils::Serialize(TextColor) << '\n';
		oss << "BrushBorder:" << Utils::Serialize(BrushBorder) << '\n';

		return oss.str();
	}


	void deserialize(const std::string& data) {
	    std::istringstream iss(data);
	    std::string line;
	    std::unordered_map<std::string, std::function<void(std::string&)>> handlers{
		{"Background", [this](std::string& value) { Background = Utils::DeserializeRaylibColor(value); }},
		{"Foreground", [this](std::string& value) { Foreground = Utils::DeserializeRaylibColor(value); }},
		{"TextColor", [this](std::string& value) { TextColor = Utils::DeserializeRaylibColor(value); }},
		{"BrushBorder", [this](std::string& value) { BrushBorder = Utils::DeserializeRaylibColor(value); }}
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
};
}

#endif

