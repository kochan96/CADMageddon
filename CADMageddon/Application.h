#pragma once
#include "WindowFactory.h"
#include "OpenGLManager.h"

namespace CADMageddon
{
	class Application
	{
	public:
		Application();
		~Application() {}

		void Init();
		int Run();

	private:
		std::unique_ptr<Window> m_window;
		std::shared_ptr<OpenGLManager> m_openGlManager;
	};
}