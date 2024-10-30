#include "InputHandler.hpp"

namespace Helium {
InputHandler::InputHandler() {
}

void InputHandler::Update() {
    switch (_currentMode) {
    case NoteMode::READ:
        CheckInputs(ReadModeActions);
        break;
    case NoteMode::WRITE:
        CheckInputs(WriteModeActions);
        break;
    case NoteMode::DRAW:
        CheckInputs(DrawModeActions);
        break;
    }
    CheckInputs(GlobalActions);
}

void InputHandler::SetMode(NoteMode mode) {
    _currentMode = mode;
}

void InputHandler::AddGlobalAction(InputCombo combo, Action action) {
    GlobalActions[combo] = action;
}

void InputHandler::AddAction(NoteMode mode, InputCombo combo, Action action) {
    switch (mode) {
    case NoteMode::READ:
        ReadModeActions[combo] = action;
        break;
    case NoteMode::WRITE:
        WriteModeActions[combo] = action;
        break;
    case NoteMode::DRAW:
        DrawModeActions[combo] = action;
        break;
    }
}

void InputHandler::CheckInputs(const std::unordered_map<InputCombo, Action> &actions) {
    bool lctrl = IsKeyDown(KEY_LEFT_CONTROL);
    bool rctrl = IsKeyDown(KEY_RIGHT_CONTROL);
    bool lshift = IsKeyDown(KEY_LEFT_SHIFT);
    bool rshift = IsKeyDown(KEY_RIGHT_SHIFT);

    float frameTime = GetFrameTime(); // Time since last frame

    if (activeAction) {
        // Accumulate time if action is active
        elapsedTime += frameTime;

        // Trigger the action if enough time has passed
        if (IsKeyDown(activeCombo.key)) {
            if (elapsedTime >= Helium::Configuration::getInstance().ActionRepeatInitialDelaySeconds) {
                activeAction();
                elapsedTime -= Helium::Configuration::getInstance().ActionRepeatDelaySeconds;
            }
        } else {
            // Reset if key is released
            activeAction = nullptr;
            elapsedTime = 0.0f;
        }
        return;
    }

    // Find a new action if none is active
    for (const auto &[combo, action] : actions) {
        bool modifiersMatch = (combo.modifiers == 0 ||
                               ((combo.modifiers & KEY_LEFT_CONTROL) && lctrl) ||
                               ((combo.modifiers & KEY_RIGHT_CONTROL) && rctrl) ||
                               ((combo.modifiers & KEY_LEFT_SHIFT) && lshift) ||
                               ((combo.modifiers & KEY_RIGHT_SHIFT) && rshift));

        if (modifiersMatch) {
            switch (combo.inputEvent) {
            case InputEvent::Press:
                if (IsKeyPressed(combo.key)) {
                    action();
                }
                break;
            case InputEvent::Release:
                if (IsKeyReleased(combo.key)) {
                    action();
                }
                break;
            case InputEvent::Down:
                if (IsKeyDown(combo.key)) {
                    activeCombo = combo;
                    activeAction = action;
                    elapsedTime = 0.0f; // Reset accumulated time when action starts
                    activeAction();
                }
                break;
            }
            if (activeAction) break; // Stop if an action is found
        }
    }
}
}
