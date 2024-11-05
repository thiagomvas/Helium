#ifndef NOTEWINDOW_HPP
#define NOTEWINDOW_HPP
#include "Window.hpp"
#include <memory>
#include "InputHandler.hpp"
#include "NoteArea.hpp"
#include "SaveFileModal.hpp"
#include "Application.hpp"
#include "Molecule.hpp"
#include "raylib.h"
class NoteWindow : public Window {
    public:
    NoteWindow();
    void Update() override;
    void Draw() override;
    private:
	std::unique_ptr<Helium::InputHandler> _inputHandler;
	std::unique_ptr<Helium::NoteArea> _noteArea;
    Camera2D camera;
    Vector2 scroll;
    std::shared_ptr<Helium::Application> _app;

};

#endif