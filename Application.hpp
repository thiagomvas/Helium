#ifndef APPLICATION_H
#define APPLICATION_H

#include "raylib.h"

namespace Helium {
	class Application {
		public: 
			Application();
			~Application();

			void Start();
			void Stop();
		private:
			bool isRunning;
			RenderTexture2D target;
	};
}

#endif
