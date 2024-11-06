#include "SimulationSettings.hpp"
#include <functional>
#include <math.h>
#include <sstream>
#include <string>
#include <unordered_map>

float SimulationSettings::CalculateAtomSize(int numBonds) {
    return BaseSize + GrowthFactor * log2f(numBonds + 1);
}

std::string SimulationSettings::serialize() {

    std::ostringstream oss;

    oss << "TargetDistance:" << TargetDistance << '\n';
    oss << "AttractionStrength:" << AttractionStrength << '\n';
    oss << "RepulsionStrength:" << RepulsionStrength << '\n';
    oss << "Damping:" << Damping << '\n';
    oss << "MaxForce:" << MaxForce << '\n';
    oss << "CollisionDistance:" << CollisionDistance << '\n';
    oss << "BaseSize:" << BaseSize << '\n';
    oss << "GrowthFactor:" << GrowthFactor << '\n';

    return oss.str();
}

void SimulationSettings::deserialize(const std::string &data) {
    std::istringstream iss(data);
    std::string line;

    std::unordered_map<std::string, std::function<void(const std::string &)>> handlers{
        {"TargetDistance", [this](const std::string &value) { TargetDistance = std::stof(value); }},
        {"AttractionStrength", [this](const std::string &value) { AttractionStrength = std::stof(value); }},
        {"RepulsionStrength", [this](const std::string &value) { RepulsionStrength = std::stof(value); }},
        {"Damping", [this](const std::string &value) { Damping = std::stof(value); }},
        {"MaxForce", [this](const std::string &value) { MaxForce = std::stof(value); }},
        {"CollisionDistance", [this](const std::string &value) { CollisionDistance = std::stof(value); }},
        {"BaseSize", [this](const std::string &value) { BaseSize = std::stof(value); }},
        {"GrowthFactor", [this](const std::string &value) { GrowthFactor = std::stof(value); }}};

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
