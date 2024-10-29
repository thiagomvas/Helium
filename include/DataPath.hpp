#ifndef DATAPATH_HPP
#define DATAPATH_HPP

#include <string>
#include <cstdlib>

std::string GetAppDataPath() {
#ifdef _WIN32
    // Windows: Use the APPDATA environment variable (C:\Users\Username\AppData\Roaming)
    char* appDataPath = std::getenv("APPDATA");
    return appDataPath ? std::string(appDataPath) : "";
#elif __APPLE__
    // macOS: Use ~/Library/Application Support/
    char* homePath = std::getenv("HOME");
    return homePath ? std::string(homePath) + "/Library/Application Support" : "";
#elif __linux__
    // Linux: Use the XDG_DATA_HOME environment variable or default to ~/.local/share/
    char* dataHome = std::getenv("XDG_DATA_HOME");
    if (dataHome) {
        return std::string(dataHome);
    }
    char* homePath = std::getenv("HOME");
    return homePath ? std::string(homePath) + "/.local/share" : "";
#else
    return ""; // Unsupported system
#endif
}


#endif