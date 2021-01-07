#include "Application.h"
constexpr int _WIDTH = 1000;
constexpr int _HEIGHT = 800;
constexpr int fps = 60;
#undef main

Application* app = new Application();

int main(int argc, char* argv[])
{

	constexpr int ms_perFrame = static_cast<int>((1000 / fps) + 0.5f);
	Uint32 frameBegin;
	int dt;
	
	app->init(argv[0], SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _WIDTH, _HEIGHT, SDL_WINDOW_OPENGL);

	while( app->isRunning() )
	{
		frameBegin = SDL_GetTicks();

		app->handleEvents();
		app->render();
		app->update(ms_perFrame);

		dt = SDL_GetTicks() - frameBegin;
		if (dt < ms_perFrame) SDL_Delay(ms_perFrame - dt);
		//else std::cout << "fdt > fps\n";
	}
	app->clean();

	return 0;
}