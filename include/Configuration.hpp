#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <mutex> // For thread safety
#include "ColorTheme.hpp"
#include "Formatting.hpp"

namespace Helium {
class Configuration {
public:
    // Static method to access the singleton instance
    static Configuration& getInstance() {
        static Configuration instance; // Guaranteed to be destroyed and instantiated on first use
        return instance;
    }

    // Delete copy constructor and assignment operator
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    // Public members
    const std::string AppName = "Helium";
    const int TopMenuBarHeight = 25;

    int MaxNoteWidth = 600;
    float ActionRepeatDelaySeconds = 0.5f;
    int ScrollLineCount = 4;
    Formatting Formatting;
    ColorTheme ColorTheme;

    std::string serialize() {
        std::ostringstream oss;

        oss << "MaxNoteWidth:" << std::to_string(MaxNoteWidth) << '\n';
        oss << "ActionRepeatDelaySeconds:" << std::to_string(ActionRepeatDelaySeconds) << '\n';
        oss << "ScrollLineCount:" << std::to_string(ScrollLineCount) << '\n';

        oss << "FORMATTING" << '\n';
        oss << Formatting.serialize() << '\n';

        oss << "COLORTHEME" << '\n';
        oss << ColorTheme.serialize() << '\n';

        return oss.str();
    }

    void deserialize(const std::string& data) {
        std::istringstream iss(data);
        std::string line;
        // Map of handlers for each key
        std::unordered_map<std::string, std::function<void(const std::string&)>> handlers{
            {"MaxNoteWidth", [this](const std::string& value) { MaxNoteWidth = std::stoi(value); }},
            {"ActionRepeatDelaySeconds", [this](const std::string& value) { ActionRepeatDelaySeconds = std::stof(value); }},
            {"ScrollLineCount", [this](const std::string& value) { ScrollLineCount = std::stoi(value); }}
        };

        // Variables to hold serialized blocks of Formatting and ColorTheme
        std::string formattingData;
        std::string colorThemeData;

        while (std::getline(iss, line)) {
            // Handle formatting section
            if (line == "FORMATTING") {
                formattingData.clear();
                while (std::getline(iss, line) && !line.empty() && line != "COLORTHEME") {
                    formattingData += line + "\n";
                }
                Formatting.deserialize(formattingData);
            }
            // Handle color theme section
            else if (line == "COLORTHEME") {
                colorThemeData.clear();
                while (std::getline(iss, line) && !line.empty()) {
                    colorThemeData += line + "\n";
                }
                ColorTheme.deserialize(colorThemeData);
            } 
            // Handle individual properties using the map
            else {
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
    }

    void unloadResources() {
        Formatting.unloadResources();
    }

private:
    // Private constructor to prevent instantiation
    Configuration() {}
};
}

#endif
