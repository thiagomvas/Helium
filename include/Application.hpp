#ifndef APPLICATION_H
#define APPLICATION_H

#include "raylib.h"
#include "Configuration.hpp"
#include <memory>

namespace Helium {
class Application {
public:
	Application(std::shared_ptr<Configuration> config);
	~Application();

	void Start();
	void Save();
	void Stop();
private:
	bool isRunning;
	std::shared_ptr<Configuration> _config;
};
} // namespace Helium

#endif
