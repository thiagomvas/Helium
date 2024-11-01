#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <functional>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>

#include "ColorTheme.hpp"
#include "Formatting.hpp"

namespace Helium {
class Configuration {
    public:
    static Configuration &getInstance();

    const std::string SUPPORTED_NOTE_FILE_TYPE = "txt, note, md";

    Configuration(const Configuration &) = delete;
    Configuration &operator=(const Configuration &) = delete;

    const std::string AppName = "Helium";
    const int TopMenuBarHeight = 25;
    std::unordered_map<std::string, std::string> Macros;
    int MaxNoteWidth = 600;
    float ActionRepeatInitialDelaySeconds = 0.5f;
    float ActionRepeatDelaySeconds = 0.05f;
    int ScrollLineCount = 4;
    Formatting Formatting;
    ColorTheme ColorTheme;

    std::string serialize();

    void deserialize(const std::string &data);

    void unloadResources();

    private:
    Configuration();
};
} // namespace Helium

#endif
