#ifndef COLORTHEME_HPP
#define COLORTHEME_HPP

#include "raylib.h"
#include "Serializer.hpp"
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>
namespace Helium {
class ColorTheme {
public: 
	Color Background = { 12, 12, 12, 255 };
	Color AccentBackground = { 50, 50, 50, 255 };
	Color Foreground = { 20, 20, 20, 255 };
	Color AccentForeground = { 200, 100, 100, 255 };
	Color TextColor = { 200, 200, 200, 255 };
	Color SecondaryTextColor = { 150, 150, 150, 255};
	Color BrushBorder = { 200, 200, 200, 255 };
	Color TextHighlight = { 37, 150, 190, 128 };
	Color CodeTextColor = { 150, 150, 150, 255 };
	Color CodeBackgroundColor = { 40, 40, 40, 255 };

	std::string serialize() {
	std::ostringstream oss;
		oss << "Background:" << Serializer::Serialize(Background) << '\n';
        oss << "AccentBackground:" << Serializer::Serialize(AccentBackground) << '\n';
		oss << "Foreground:" << Serializer::Serialize(Foreground) << '\n';
        oss << "AccentForeground:" << Serializer::Serialize(AccentForeground) << '\n';
		oss << "TextColor:" << Serializer::Serialize(TextColor) << '\n';
		oss << "BrushBorder:" << Serializer::Serialize(BrushBorder) << '\n';
		oss << "SecondaryTextColor:" << Serializer::Serialize(SecondaryTextColor) << '\n';
		oss << "TextHighlight:" << Serializer::Serialize(TextHighlight) << '\n';
		oss << "CodeTextColor" << Serializer::Serialize(CodeTextColor) << '\n';
		oss << "CodeBackgroundColor" << Serializer::Serialize(CodeBackgroundColor) << '\n';

		return oss.str();
	}


	void deserialize(const std::string& data) {
	    std::istringstream iss(data);
	    std::string line;
	    std::unordered_map<std::string, std::function<void(std::string&)>> handlers{
		{"Background", [this](std::string& value) { Background = Serializer::DeserializeRaylibColor(value); }},
		{"AccentBackground", [this](std::string& value) { AccentBackground = Serializer::DeserializeRaylibColor(value); }},
		{"Foreground", [this](std::string& value) { Foreground = Serializer::DeserializeRaylibColor(value); }},
		{"AccentForeground", [this](std::string& value) { AccentForeground = Serializer::DeserializeRaylibColor(value); }},
		{"TextColor", [this](std::string& value) { TextColor = Serializer::DeserializeRaylibColor(value); }},
		{"BrushBorder", [this](std::string& value) { BrushBorder = Serializer::DeserializeRaylibColor(value); }}, 
		{"SecondaryTextColor", [this](std::string& value) {SecondaryTextColor = Serializer::DeserializeRaylibColor(value); }},
		{"TextHighlight", [this](std::string& value) {TextHighlight = Serializer::DeserializeRaylibColor(value); }},
		{"CodeTextColor", [this](std::string& value) {CodeTextColor = Serializer::DeserializeRaylibColor(value); }},
		{"CodeBackgroundColor", [this](std::string& value) {CodeBackgroundColor = Serializer::DeserializeRaylibColor(value); }},
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

