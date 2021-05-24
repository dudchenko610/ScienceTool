#pragma once

#include "../window/Window.h"

#include "array-list/ArrayList.h"
#include "../looper/Looper.h"

namespace core
{
	class Application
	{
	public:
		Application(const Application&) = delete;

		static Application& Get();
	private:
		Application();
		~Application();
	public:
		void Loop();
	private:
		friend class core::Window;
		void OpenWindow(Window* window);
		void CloseWindow(Window* window);
	public:
		Looper& GetLooper();
	private:
		collection::ArrayList<Window*> windows;
		friend class core::Window;
		Looper looper;
	};

}



