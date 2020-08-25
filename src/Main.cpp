#include "Application.h"
#define FPS 60
#undef main

Application* app = new Application();

int main(int argc, char* argv[])
{
	constexpr int ms_perFrame = static_cast<int>((1000 / FPS) + 0.5f);
	Uint32 frameBegin;
	int dt;
	
	app->init("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 800, SDL_WINDOW_OPENGL);

	while( app->isRunning() )
	{
		frameBegin = SDL_GetTicks();

		app->handleEvents();
		app->render();
		app->update(ms_perFrame);

		dt = SDL_GetTicks() - frameBegin;
		if (dt < ms_perFrame) SDL_Delay(ms_perFrame - dt);
		else std::cout << "fdt > fps\n";
	}
	app->clean();

	return 0;
}