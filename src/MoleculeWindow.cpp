#include "MoleculeWindow.hpp"
#include "Molecule.hpp"
#include "Bond.hpp"
#include "Atom.hpp"
#include "raylib.h"
#include <memory>

MoleculeWindow::MoleculeWindow(std::shared_ptr<Molecule> molecule)
    : _loadedMolecule(molecule) {
        InitializeAtomPositions();
}

void MoleculeWindow::Update() {
}

void MoleculeWindow::Draw() {
    for (const auto &bond : _loadedMolecule->GetBonds()) {
        auto atom1 = bond->GetAtom1();
        auto atom2 = bond->GetAtom2();

        Vector2 pos1 = atomPositions[atom1];
        Vector2 pos2 = atomPositions[atom2];

        DrawLineV(pos1, pos2, WHITE); // Draw line between connected atoms
    }

    // Draw atoms as circles with titles
    for (const auto &atom : _loadedMolecule->GetAtoms()) {
        Vector2 pos = atomPositions[atom];
        DrawCircleV(pos, 20, BLUE); // Draw atom as a blue circle

        // Draw atom's title above the circle
        int textWidth = MeasureText(atom->GetTitle().c_str(), 20);
        DrawText(atom->GetTitle().c_str(), pos.x - textWidth / 2, pos.y - 30, 20, WHITE);
    }
}

void MoleculeWindow::InitializeAtomPositions() {
    srand(time(nullptr)); // Seed random position generation

    int screenWidth = GetScreenWidth() * 0.5f;
    int screenHeight = GetScreenHeight() * 0.5f;

    for (const auto &atom : _loadedMolecule->GetAtoms()) {
        Vector2 position = {
            static_cast<float>(rand() % screenWidth + screenWidth * 0.5f),
            static_cast<float>(rand() % screenHeight + screenWidth * 0.5f)};
        atomPositions[atom] = position;
    }
}
