#pragma once
#define PY_SSIZE_T_CLEAN
#include <python.h>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "PyIn.h"
#include "Camera.h"
#include "Shader.h"
#include "Vertex.h"
#include "SceneObjects.h"
#include "Debug.h"

class Application
{
	
	SDL_Window* window;
	SDL_GLContext context;
	SDL_Event ev;
	const Uint8* keyStates;
	Mix_Music* music;
	bool running = true;

public:
	
	Scene::Manager m;
	Camera::Viewport* cam;
	Shader::Program* phong_shader;
	Shader::Program* ps2;
	Shader::Program* graphlines_shader;
	Shader::Program* function_shader;


	void init(const char* title, int x, int y, int w, int h, int fullscreen)
	{
		startSDL();
		init_window(title, x, y, w, h, fullscreen);
		link_GLAPI();
		init_camera(w, h);
		prep_scene();
	}
	void handleEvents()
	{
		poll_events();
		get_keystates();
	}
	void update( float t )
	{
		m.update(t);
	}
	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m.render();
		SDL_GL_SwapWindow(window);
	}
	void clean()
	{
		cpython::finalize();
		if (music != nullptr) {
			Mix_FreeMusic(music);
			Mix_CloseAudio();
		}
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		IMG_Quit();
		SDL_Quit();
	}
	bool isRunning() { return running; }
	
private:

	void prep_scene()
	{
		cpython::initialize(); // needed when using a shader object that utilzes embedded python interpretter
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//phong_shader = new Shader::Program("src/Phong.glsl");
		//set_phong_uniforms(*phong_shader);
		//m.new_object<Sphere>(10.0f, 10.0f, 0.0f, *cam, *phong_shader);

		//ps2 = new Shader::Program("src/Phong.glsl");
		//set_phong_uniforms(*ps2);
		//m.new_object<Cube>(2.0f, 2.0f, 1.0f, *cam, *ps2);
		
		graphlines_shader = new Shader::Program("src/GraphLines.glsl");
		m.new_object<Plane>(30.0f, *cam, *graphlines_shader);

		function_shader = new Shader::Program("src/Function.glsl");
		m.new_object<Function>(30.0f, *cam, *function_shader);
	}

	void set_phong_uniforms(Shader::Program &shader) {
		shader.use();
		vec4 worldLight = vec4(5.0f, 5.0f, 2.0f, 1.0f);
		shader.set("mater.diffRefl", 0.9f, 0.5f, 0.3f);
		shader.set("light.diff", 1.0f, 1.0f, 1.0f);
		shader.set("light.camCoordPos", cam->get_WorldToView_Matrix() * worldLight); // world to view transformation
		shader.set("mater.ambiRefl", 0.9f, 0.5f, 0.3f);
		shader.set("light.ambi", 0.4f, 0.4f, 0.4f);
		shader.set("mater.specRefl", 0.8f, 0.8f, 0.8f);
		shader.set("light.spec", 1.0f, 1.0f, 1.0f);
		shader.set("mater.sheen", 100.0f);
	}

	void init_window(const char* title, int x, int y, int w, int h, int fullscreen)
	{
		window = SDL_CreateWindow(title, x, y, w, h, fullscreen);
		context = SDL_GL_CreateContext(window);
		keyStates = SDL_GetKeyboardState(nullptr);
	}

	void init_camera(int w, int h)
	{
		Camera::setWindowDimmensions(w, h);
		cam = new Camera::Viewport(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, -1.0f), 1.25f);
	}

	void startSDL()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) abort_MyGL_App("SDL initialization error: ", SDL_GetError() );
		if (IMG_Init(IMG_INIT_PNG) == 0) std::cout << IMG_GetError() << "\n\n";
	}

	void link_GLAPI()
	{
		specifyGL();
		glewExperimental = GL_TRUE;
		glewInit();
		submitDebugCallbackFunction();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void specifyGL()
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}

	void testGLEW() {
		GLuint vertexBuffer; glGenBuffers(1, &vertexBuffer);
		if (vertexBuffer == 1) {
			std::cout << "\nGLEW successful\n";
			glDeleteBuffers(1, &vertexBuffer);
		}
	}

	void showExtensions()
	{
		GLint nExtensions; glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
		for (int i = 0; i < nExtensions; i++)
			std::cout << glGetStringi(GL_EXTENSIONS, i) << "\n";
	}

	void setMusic(const char* MP3_file)
	{
		if (!Mix_Init(MIX_INIT_MP3)) std::cout << Mix_GetError() << "\n";
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) std::cout << Mix_GetError() << "\n";
		music = Mix_LoadMUS(MP3_file);
		if (music != nullptr) Mix_PlayMusic(music, -1);
		else std::cout << Mix_GetError() << "\n";
	}

	void poll_events()
	{
		SDL_PollEvent(&ev);
		if (ev.type == SDL_MOUSEMOTION) cam->mouse_motion(ev.motion.x, ev.motion.y);
		if (ev.type == SDL_MOUSEWHEEL)  cam->mouse_scroll(ev.wheel.y);
		if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_ENTER) cam->set_firstMouse(true);
		if (ev.type == SDL_QUIT) running = false;
		if (ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_ESCAPE) running = false;
	}

	void get_keystates()
	{
		SDL_PumpEvents();
		if (keyStates[SDL_SCANCODE_A]) cam->move(-.1f, cam->RIGHT);
		if (keyStates[SDL_SCANCODE_W]) cam->move(0.1f, cam->FORWARD);
		if (keyStates[SDL_SCANCODE_LSHIFT]) cam->move(-.1f, cam->UP);
		if (keyStates[SDL_SCANCODE_D]) cam->move(0.1f, cam->RIGHT);
		if (keyStates[SDL_SCANCODE_S]) cam->move(-.1f, cam->FORWARD);
		if (keyStates[SDL_SCANCODE_SPACE])  cam->move(0.1f, cam->UP);
	}
};