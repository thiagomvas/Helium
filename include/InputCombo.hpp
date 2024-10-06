#ifndef INPUTCOMBO_HPP
#define INPUTCOMBO_HPP
#include <functional>
namespace Helium {
struct InputCombo {
	int key = 0;
	int modifiers = 0;

	InputCombo(int key, int modifiers = 0) : key(key), modifiers(modifiers) {}

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

