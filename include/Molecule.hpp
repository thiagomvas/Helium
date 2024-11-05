#include <memory>
#include <string>
#include <vector>
#ifndef MOLECULE_HPP
#define MOLECULE_HPP

class Atom;
class Bond;

class Molecule {
    public:
    Molecule(std::string name);

    void AddAtom(std::shared_ptr<Atom> atom);
    void AddBond(std::shared_ptr<Bond> bond);
    const std::vector<std::shared_ptr<Atom>> &GetAtoms() const;
    const std::vector<std::shared_ptr<Bond>> &GetBonds() const;

    private:
    std::string _name;
    std::vector<std::shared_ptr<Atom>> _atoms;
    std::vector<std::shared_ptr<Bond>> _bonds;
};

#endif