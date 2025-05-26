#define SDL_MAIN_HANDLED 1

#include "Application.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{

	auto& app = VORTEK_EDITOR::Application::GetInstance();
	app.Run();

	return 0;
}