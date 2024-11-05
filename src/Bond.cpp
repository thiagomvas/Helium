#include "Bond.hpp"
#include <memory>
#include "Atom.hpp"

Bond::Bond(std::shared_ptr<Atom> atom1, std::shared_ptr<Atom> atom2) : _atom1(atom1), _atom2(atom2) {}

std::shared_ptr<Atom> Bond::GetAtom1() const {
    return _atom1;
}

std::shared_ptr<Atom> Bond::GetAtom2() const {
    return _atom2;
}

std::shared_ptr<Atom> Bond::GetConnectedAtom(std::shared_ptr<Atom> atom) const {
    if(atom == _atom1) {
        return _atom2;
    } else if (atom == _atom2) {
        return _atom1;
    } else {
        return nullptr;
    }
}
