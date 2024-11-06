#include <string>
#ifndef SIMULATIONSETTINGS_HPP
#define SIMULATIONSETTINGS_HPP

class SimulationSettings {
    public:
    float TargetDistance = 100.0f;
    float AttractionStrength = 0.05f;
    float RepulsionStrength = 0.5f;
    float Damping = 0.95f;
    float MaxForce = 5.0f;
    float CollisionDistance = 20.0f;
    float BaseSize = 10.0f;
    float GrowthFactor = 5.0f;


    float CalculateAtomSize(int numBonds);
    std::string serialize();
    void deserialize(const std::string& data);
};

#endif