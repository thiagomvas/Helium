#include <string>
#include <vector>
#include <memory>
#ifndef ATOM_HPP
#define ATOM_HPP

class Bond;

class Atom {
    public:
    Atom(std::string title, std::string content);
    const std::string& GetTitle() const;
    const std::string& GetContent() const;
    const std::vector<std::shared_ptr<Bond>>& GetBonds() const;

    void SetTitle(const std::string& title);
    void SetContent(const std::string& content);

    void AddBond(std::shared_ptr<Bond> bond);

    private:
    std::string _title;
    std::string _content;
    std::vector<std::shared_ptr<Bond>> _bonds;
};

#endif