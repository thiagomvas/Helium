#ifndef APPLICATION_H
#define APPLICATION_H

#include "InputHandler.hpp"
#include "NoteArea.hpp"
#include "raylib.h"
#include "Configuration.hpp"
#include <memory>
namespace Helium {
class Application {
public:
	Application();
	~Application();

	void Start();
	void Save();
	void Stop();
private:
	bool isRunning;
	std::shared_ptr<InputHandler> _inputHandler;
	std::unique_ptr<NoteArea> _noteArea;
};
} // namespace Helium

#endif
