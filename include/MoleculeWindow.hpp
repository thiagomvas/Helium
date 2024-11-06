#ifndef MOLECULEWINDOW_HPP
#define MOLECULEWINDOW_HPP

#include "Application.hpp"
#include <memory>
#include "Window.hpp"
#include "Molecule.hpp"
#include <unordered_map>
#include "raylib.h"

class MoleculeWindow : public Window {
    public:
    MoleculeWindow(std::shared_ptr<Molecule> molecule);
    void Update() override;
    void Draw() override;

    private:
    std::shared_ptr<Molecule> _loadedMolecule;
    std::unordered_map<std::shared_ptr<Atom>, Vector2> atomPositions;
    std::unordered_map<std::shared_ptr<Atom>, Vector2> atomVelocities;
    Camera2D _camera;
    bool _isDragginAtom;
    bool _isPanningCamera;
    std::shared_ptr<Atom> _selectedAtom;
    void InitializeAtomPositions();
};

#endif