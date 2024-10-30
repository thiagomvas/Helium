#include "Application.hpp"
#include "Configuration.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <filesystem>
#include <fstream>
#include "DataPath.hpp"

int main() {
    std::string appDataPath = GetAppDataPath();
    std::filesystem::path heliumFolderPath = appDataPath + "/Helium";

    if (!std::filesystem::exists(heliumFolderPath)) {
        std::filesystem::create_directories(heliumFolderPath);
    }

    std::filesystem::path filePath = heliumFolderPath / "config.txt";

    if (std::filesystem::exists(filePath)) {
        std::ifstream configFile(filePath);
        std::string configData((std::istreambuf_iterator<char>(configFile)),
                               std::istreambuf_iterator<char>());
        configFile.close();

        Helium::Configuration::getInstance().deserialize(configData);
        std::cout << "Configuration loaded from config.txt." << std::endl;
    } else {
        std::string defaultConfig = Helium::Configuration::getInstance().serialize();

        std::ofstream configFile(filePath);
        configFile << defaultConfig;
        configFile.close();

        std::cout << "Default configuration saved to config.txt." << std::endl;
    }

    Helium::Application app;
    app.Start();
    app.Stop();

    return 0;
}
