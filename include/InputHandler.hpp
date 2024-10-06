#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include "InputCombo.hpp"
#include "NoteMode.hpp"
#include "raylib.h"
#include <functional>
#include <unordered_map>
namespace Helium {
class InputHandler {
public:
	using Action = std::function<void()>;

	std::unordered_map<InputCombo, Action> ReadModeActions;
	std::unordered_map<InputCombo, Action> WriteModeActions;
	std::unordered_map<InputCombo, Action> DrawModeActions;
	std::unordered_map<InputCombo, Action> GlobalActions;
	void Update() {
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
	void SetMode(NoteMode mode) {
		_currentMode = mode;
	}
	void AddGlobalAction(InputCombo combo, Action action) {
		GlobalActions[combo] = action;
	}
    void AddAction(NoteMode mode, InputCombo combo, Action action) {
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
private:
	NoteMode _currentMode;
    void CheckInputs(const std::unordered_map<InputCombo, Action>& actions) {
		bool lctrl = IsKeyDown(KEY_LEFT_CONTROL);
		bool rctrl = IsKeyDown(KEY_RIGHT_CONTROL);
		bool lshift = IsKeyDown(KEY_LEFT_SHIFT);
		bool rshift = IsKeyDown(KEY_RIGHT_SHIFT);
        for (const auto& [combo, action] : actions) {
            bool modifiersMatch = (combo.modifiers == 0 ||
                                   ((combo.modifiers & KEY_LEFT_CONTROL) && lctrl) ||
                                   ((combo.modifiers & KEY_RIGHT_CONTROL) && rctrl) ||
                                   ((combo.modifiers & KEY_LEFT_SHIFT) && lshift) ||
                                   ((combo.modifiers & KEY_RIGHT_SHIFT) && rshift));

            if (modifiersMatch && IsKeyPressed(combo.key)) {
                action();
            }
        }
    }
};
}

#endif

