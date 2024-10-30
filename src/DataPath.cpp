#include "DataPath.hpp"

#include <string>
#include <cstdlib>

std::string GetUserRootPath() {
#ifdef _WIN32
    // Windows: Use the USERPROFILE environment variable (C:\Users\Username)
    char* userProfilePath = std::getenv("USERPROFILE");
    return userProfilePath ? std::string(userProfilePath) : "";
#elif __APPLE__ || __linux__
    // macOS and Linux: Use the HOME environment variable
    char* homePath = std::getenv("HOME");
    return homePath ? std::string(homePath) : "";
#else
    return ""; // Unsupported system
#endif
}

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