#ifndef INPUTCOMBO_HPP
#define INPUTCOMBO_HPP
#include <functional>
namespace Helium {
enum class InputEvent {
	Press,
	Down,
	Release
};

struct InputCombo {
	int key = 0;
	int modifiers = 0;
	InputEvent inputEvent = InputEvent::Press;

	InputCombo() = default;
	InputCombo(int key) : key(key) {}
	InputCombo(int key, int modifiers) : key(key), modifiers(modifiers) {}
	InputCombo(int key, int modifiers, InputEvent event) : key(key), modifiers(modifiers), inputEvent(event) {}

	bool operator==(const InputCombo &other) const {
		return key == other.key && modifiers == other.modifiers;
	}
};
}

namespace std {
template <>
struct hash<Helium::InputCombo> {
std::size_t operator()(const Helium::InputCombo &combo) const {
		return hash<int>()(combo.key) ^ hash<int>()(combo.modifiers);
	}
};
}

#endif

