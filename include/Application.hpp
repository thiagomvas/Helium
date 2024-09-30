#ifndef APPLICATION_H
#define APPLICATION_H

#include "raylib.h"

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
	};
}

#endif
