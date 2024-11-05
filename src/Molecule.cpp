#include "Atom.hpp"
#include "Bond.hpp"
#include "Molecule.hpp"
#include <vector>
#include <memory>
#include <string>

Molecule::Molecule(std::string name) : _name(name) {
}

void Molecule::AddAtom(std::shared_ptr<Atom> atom) {
    _atoms.push_back(atom);
}

void Molecule::AddBond(std::shared_ptr<Bond> bond) {
    _bonds.push_back(bond);
}

const std::vector<std::shared_ptr<Atom>> &Molecule::GetAtoms() const {
    return _atoms;
}

const std::vector<std::shared_ptr<Bond>> &Molecule::GetBonds() const {
    return _bonds;
}
