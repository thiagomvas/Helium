#include "Atom.hpp"
#include <memory>
#include <string>
#include <vector>

Atom::Atom(std::string title, std::string content)
    : _title(title), _content(content) {
}

const std::string &Atom::GetTitle() const {
    return _title;
}

const std::string &Atom::GetContent() const {
    return _content;
}

const std::vector<std::shared_ptr<Bond>> &Atom::GetBonds() const {
    return _bonds;
}

void Atom::SetTitle(const std::string &title) {
    _title = title;
}

void Atom::SetContent(const std::string &content) {

    _content = content;
}
void Atom::AddBond(std::shared_ptr<Bond> bond) {
    _bonds.push_back(bond);
}
