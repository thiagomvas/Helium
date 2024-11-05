#ifndef APPLICATION_H
#define APPLICATION_H

#include "InputHandler.hpp"
#include "NoteArea.hpp"
#include "raylib.h"
#include "Configuration.hpp"
#include "SaveFileModal.hpp"
#include "Molecule.hpp"
#include <memory>
namespace Helium {
class Application : public std::enable_shared_from_this<Application> {
public:
	Application();
	~Application();

	void Start();
	void Save();
	void Stop();

	void OpenSaveModal();
	std::shared_ptr<Molecule> GetLoadedMolecule();
private:
	bool isRunning;
	std::shared_ptr<InputHandler> _inputHandler;
	std::unique_ptr<NoteArea> _noteArea;
	SaveFileModal _saveModal;
	std::shared_ptr<Molecule> _loadedMolecule;
};
} // namespace Helium

#endif
