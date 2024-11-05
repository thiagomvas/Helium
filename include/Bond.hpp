#include <memory>
#ifndef BOND_HPP
#define BOND_HPP

class Atom;

class Bond {
    public:
    Bond(std::shared_ptr<Atom> atom1, std::shared_ptr<Atom> atom2);

    std::shared_ptr<Atom> GetAtom1() const;
    std::shared_ptr<Atom> GetAtom2() const;

    std::shared_ptr<Atom> GetConnectedAtom(std::shared_ptr<Atom> atom) const;

    private:
    std::shared_ptr<Atom> _atom1;
    std::shared_ptr<Atom> _atom2;

};

#endif