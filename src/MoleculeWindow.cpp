#include "MoleculeWindow.hpp"
#include "Configuration.hpp"
#include "SimulationSettings.hpp"
#include "Atom.hpp"
#include "Bond.hpp"
#include "Molecule.hpp"
#include "raylib.h"
#include "raymath.h"
#include <memory>

MoleculeWindow::MoleculeWindow(std::shared_ptr<Molecule> molecule)
    : _loadedMolecule(molecule) {
    _camera.target = {0.0f, 0.0f};
    _camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    _camera.rotation = 0.0f;
    _camera.zoom = 1.0f;
    InitializeAtomPositions();
}

void MoleculeWindow::Update() {
    SimulationSettings& _settings = Helium::Configuration::getInstance().Simulation;
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), _camera);

    // Handle dragging and panning logic as before
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (const auto &atom : _loadedMolecule->GetAtoms()) {
            Vector2 atomPos = atomPositions[atom];
            if (CheckCollisionPointCircle(mousePos, atomPos, 20)) {
                _selectedAtom = atom;
                _isDragginAtom = true;
                break;
            }
        }
    }

    if (_isDragginAtom && _selectedAtom != nullptr) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            atomPositions[_selectedAtom] = mousePos;
            atomVelocities[_selectedAtom] = {0, 0}; // Reset velocity while dragging
        } else {
            _isDragginAtom = false;
            _selectedAtom = nullptr;
        }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        if (!_isPanningCamera) {
            _isPanningCamera = true;
        }
        Vector2 delta = GetMouseDelta();
        _camera.target = Vector2Subtract(_camera.target, Vector2Scale(delta, 1.0f / _camera.zoom));
    } else {
        _isPanningCamera = false;
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        Vector2 mouseWorldPosBeforeZoom = GetScreenToWorld2D(GetMousePosition(), _camera);

        _camera.zoom += wheel * 0.1f;
        if (_camera.zoom < 0.1f) _camera.zoom = 0.1f;

        Vector2 mouseWorldPosAfterZoom = GetScreenToWorld2D(GetMousePosition(), _camera);

        _camera.target = Vector2Add(_camera.target, Vector2Subtract(mouseWorldPosBeforeZoom, mouseWorldPosAfterZoom));
    }

    // Apply bond attraction forces
    for (const auto &bond : _loadedMolecule->GetBonds()) {
        auto atom1 = bond->GetAtom1();
        auto atom2 = bond->GetAtom2();

        Vector2 pos1 = atomPositions[atom1];
        Vector2 pos2 = atomPositions[atom2];

        Vector2 direction = Vector2Subtract(pos2, pos1);
        float distance = Vector2Length(direction);
        Vector2 normalizedDirection = Vector2Scale(direction, 1.0f / distance);

        float displacement = distance - _settings.TargetDistance;
        Vector2 force = Vector2Scale(normalizedDirection, _settings.AttractionStrength * displacement);

        // Cap force
        if (Vector2Length(force) > _settings.MaxForce) {
            force = Vector2Scale(Vector2Normalize(force), _settings.MaxForce);
        }

        atomVelocities[atom1] = Vector2Add(atomVelocities[atom1], force);
        atomVelocities[atom2] = Vector2Subtract(atomVelocities[atom2], force);
    }

    // Apply repulsive forces to prevent atom overlap (collision)
    for (const auto &atom1 : _loadedMolecule->GetAtoms()) {
        for (const auto &atom2 : _loadedMolecule->GetAtoms()) {
            if (atom1 == atom2) continue;

            Vector2 pos1 = atomPositions[atom1];
            Vector2 pos2 = atomPositions[atom2];

            Vector2 direction = Vector2Subtract(pos2, pos1);
            float distance = Vector2Length(direction);

            // If atoms are closer than CollisionDistance, apply a repulsive force
            if (distance < _settings.CollisionDistance) {
                Vector2 normalizedDirection = Vector2Scale(direction, 1.0f / distance);
                float overlap = _settings.CollisionDistance - distance;
                Vector2 repulsionForce = Vector2Scale(normalizedDirection, -_settings.RepulsionStrength * overlap);

                // Cap repulsive force
                if (Vector2Length(repulsionForce) > _settings.MaxForce) {
                    repulsionForce = Vector2Scale(Vector2Normalize(repulsionForce), _settings.MaxForce);
                }

                atomVelocities[atom1] = Vector2Add(atomVelocities[atom1], repulsionForce);
                atomVelocities[atom2] = Vector2Subtract(atomVelocities[atom2], repulsionForce);
            }
        }
    }

    // Update positions based on velocity and apply damping
    for (const auto &atom : _loadedMolecule->GetAtoms()) {
        if (atom != _selectedAtom) {
            Vector2 velocity = atomVelocities[atom];
            velocity = Vector2Scale(velocity, _settings.Damping);
            atomVelocities[atom] = velocity;

            atomPositions[atom] = Vector2Add(atomPositions[atom], velocity);
        }
    }
}


void MoleculeWindow::Draw() {
    BeginMode2D(_camera);

    for (const auto &bond : _loadedMolecule->GetBonds()) {
        auto atom1 = bond->GetAtom1();
        auto atom2 = bond->GetAtom2();
        Vector2 pos1 = atomPositions[atom1];
        Vector2 pos2 = atomPositions[atom2];
        DrawLineV(pos1, pos2, WHITE);
    }

    for (const auto &atom : _loadedMolecule->GetAtoms()) {
        Vector2 pos = atomPositions[atom];
        DrawCircleV(pos, 20, BLUE);
        int textWidth = MeasureText(atom->GetTitle().c_str(), 20);
        DrawText(atom->GetTitle().c_str(), pos.x - textWidth / 2, pos.y - 30, 20, WHITE);
    }

    EndMode2D();
}

void MoleculeWindow::InitializeAtomPositions() {
    for (const auto &atom : _loadedMolecule->GetAtoms()) {
        atomPositions[atom] = Vector2{static_cast<float>(GetRandomValue(-200, 200)), static_cast<float>(GetRandomValue(-200, 200))};
    }
}
