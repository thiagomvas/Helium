#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include "InputCombo.hpp"
#include "NoteMode.hpp"
#include "Configuration.hpp"
#include "raylib.h"
#include <functional>
#include <unordered_map>
#include <chrono>

namespace Helium {
class InputHandler {
public:
    InputHandler();

    using Action = std::function<void()>;

    std::unordered_map<InputCombo, Action> ReadModeActions;
    std::unordered_map<InputCombo, Action> WriteModeActions;
    std::unordered_map<InputCombo, Action> DrawModeActions;
    std::unordered_map<InputCombo, Action> GlobalActions;

    void Update();
    void SetMode(NoteMode mode);
    void AddGlobalAction(InputCombo combo, Action action);
    void AddAction(NoteMode mode, InputCombo combo, Action action);

private:
    NoteMode _currentMode;
    const float initialDelay = 0.5f;
    const float repeatDelay = 0.1f;
    InputCombo activeCombo;
    Action activeAction = nullptr;
    float elapsedTime = 0.0f; // Time accumulator for active action

    void CheckInputs(const std::unordered_map<InputCombo, Action>& actions);
};
}

#endif
