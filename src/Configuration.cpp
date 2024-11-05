#include "Configuration.hpp"
#include "ColorTheme.hpp"
#include "Formatting.hpp"
#include <functional>
#include <vector>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <iostream>

namespace Helium {

Configuration::Configuration() {
}
Configuration &Configuration::getInstance() {
    static Configuration instance; // Singleton instance
    return instance;
}

int Configuration::GetScaledNoteWidth() {
    return std::min(GetScreenWidth(), MaxNoteWidth);
}
std::string Configuration::serialize() {
    std::ostringstream oss;
    oss << "MaxNoteWidth:" << MaxNoteWidth << '\n';
    oss << "ActionRepeatInitialDelaySeconds:" << ActionRepeatInitialDelaySeconds << '\n';
    oss << "ActionRepeatDelaySeconds:" << ActionRepeatDelaySeconds << '\n';
    oss << "ScrollLineCount:" << ScrollLineCount << '\n';
    oss << "HoverAnimationDurationSeconds:" << HoverAnimationDurationSeconds << '\n';

    oss << "FORMATTING\n"
        << Formatting.serialize() << '\n';
    oss << "COLORTHEME\n"
        << ColorTheme.serialize() << '\n';
    oss << "MACROS\n";

    for (const auto &[key, value] : Macros) {
        if (value.find('\n') != std::string::npos) { // Multi-line macro
            oss << key << ":<<\n"
                << value << "\n>>\n";
        } else {
            oss << key << ":" << value << '\n';
        }
    }
    return oss.str();
}

std::vector<std::pair<std::string, std::string>> Configuration::serializeIntoMap() {
    std::vector<std::pair<std::string, std::string>> configVector;
    std::string serialized = serialize();
    std::istringstream configStream(serialized);
    std::string line;

    while (std::getline(configStream, line)) {
        auto delimiterPos = line.find(':');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            configVector.emplace_back(key, value); // Add key-value pair to vector
        } else {
            configVector.emplace_back(line, ""); // Add key with empty value
        }
    }

    return configVector; // Return vector of pairs
}


void Configuration::deserialize(const std::string &data) {
    std::istringstream iss(data);
    std::string line;
    bool inMacrosSection = false;
    
    std::unordered_map<std::string, std::function<void(const std::string &)>> handlers{
        {"MaxNoteWidth", [this](const std::string &value) { MaxNoteWidth = std::stoi(value); }},
        {"ActionRepeatInitialDelaySeconds", [this](const std::string &value) { ActionRepeatInitialDelaySeconds = std::stof(value); }},
        {"ActionRepeatDelaySeconds", [this](const std::string &value) { ActionRepeatDelaySeconds = std::stof(value); }},
        {"ScrollLineCount", [this](const std::string &value) { ScrollLineCount = std::stoi(value); }},
        {"HoverAnimationDurationSeconds", [this](const std::string &value) { HoverAnimationDurationSeconds = std::stof(value); }},
    };

    std::string formattingData;
    std::string colorThemeData;

    while (std::getline(iss, line)) {
        if (line == "MACROS") {
            inMacrosSection = true;
            continue;
        }

        if (inMacrosSection) {
            if (line.empty()) {
                inMacrosSection = false; // Exit MACROS section on blank line
            } else {
                auto delimiterPos = line.find(':');
                if (delimiterPos != std::string::npos) {
                    std::string key = line.substr(0, delimiterPos);
                    std::string value = line.substr(delimiterPos + 1);

                    // Check for multi-line macro value
                    if (value == "<<") {
                        value.clear();
                        // Read until the end marker '>>' is found
                        while (std::getline(iss, line) && line != ">>") {
                            value += line + "\n"; // Include empty lines
                        }
                        // Remove the trailing newline character if necessary
                        if (!value.empty() && value.back() == '\n') {
                            value.pop_back();
                        }
                    }

                    Macros[key] = value;
                }
            }
        } else if (line == "FORMATTING") {
            formattingData.clear();
            while (std::getline(iss, line) && !line.empty() && line != "COLORTHEME") {
                formattingData += line + "\n";
            }
            Formatting.deserialize(formattingData);
        } else if (line == "COLORTHEME") {
            colorThemeData.clear();
            while (std::getline(iss, line) && !line.empty()) {
                colorThemeData += line + "\n";
            }
            ColorTheme.deserialize(colorThemeData);
        } else {
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
    std::cout << serialize() << std::endl;
}
void Configuration::deserialize(const std::vector<std::pair<std::string, std::string>> &data) {
    std::string serializedData;
    
    for (const auto& [key, value] : data) {
        if(value == "") {
            serializedData += key + '\n';
        } else {
            serializedData += key + ":" + value + "\n";
        }
    }
    
    deserialize(serializedData);
}

void Configuration::unloadResources() {
    Formatting.unloadResources();
}
}
