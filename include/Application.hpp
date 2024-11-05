#ifndef APPLICATION_H
#define APPLICATION_H

#include "InputHandler.hpp"
#include "NoteArea.hpp"
#include "raylib.h"
#include "Configuration.hpp"
#include "SaveFileModal.hpp"
#include <memory>
namespace Helium {
class Application {
public:
	Application();
	~Application();

	void Start();
	void Save();
	void Stop();

	void OpenSaveModal();
private:
	bool isRunning;
	std::shared_ptr<InputHandler> _inputHandler;
	std::unique_ptr<NoteArea> _noteArea;
	SaveFileModal _saveModal;
};
} // namespace Helium

#endif
