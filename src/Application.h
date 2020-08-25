#pragma once
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 640;
#include <chrono>
#include <iostream>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "Camera.h"
#include "Shader.h"
#include "SceneObjects.h"
#include "Debug.h"


using glm::vec3, glm::vec4, glm::mat4;
using glm::radians;

class Application
{
	SDL_Window* window;
	SDL_GLContext context;
	SDL_Event event;
	const Uint8* keyStates;
	Mix_Music* music;
	bool running = false;

public:
	
	Camera::Viewport* cam;

	Particles* particles;
	Scene::Object* cube;

	Shader::Program* phongShader;

	void init(const char* title, int x, int y, int w, int h, int fullscreen) {
		initializeFramework_createWindow(title, x, y, w, h, fullscreen);
		cam = new Camera::Viewport(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -1.0f), radians(70.0f), 0.1f, 1000.0f);
		prep_scene();
	}

	void handleEvents() {
		SDL_PollEvent(&event);
		if (event.type == SDL_MOUSEMOTION)  cam->mouse_motion(event.motion.x, event.motion.y);
		if (event.type == SDL_MOUSEWHEEL)   cam->mouse_scroll(event.wheel.y);
		if (event.type == SDL_WINDOWEVENT)  if (event.window.event == SDL_WINDOWEVENT_ENTER) cam->set_firstMouse(true);
		if (event.type == SDL_QUIT)	        running = false;
		if (event.type == SDL_KEYUP)        if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
		
		SDL_PumpEvents();
		if (keyStates[SDL_SCANCODE_A])      cam->move(-.1f, cam->RIGHT);
		if (keyStates[SDL_SCANCODE_W])      cam->move(0.1f,  cam->FORWARD);
		if (keyStates[SDL_SCANCODE_LSHIFT]) cam->move(-.1f,  cam->UP);
		if (keyStates[SDL_SCANCODE_D])      cam->move(0.1f,   cam->RIGHT); 
		if (keyStates[SDL_SCANCODE_S])      cam->move(-.1f,  cam->FORWARD);
		if (keyStates[SDL_SCANCODE_SPACE])  cam->move(0.1f,  cam->UP);
	}
	void update( float timeStep )
	{
		particles->update(timeStep);
		//cube->set_matrices(*cam, *phongShader);
	}
	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		particles->render();

		//phongShader->use();
		//cube->render();
		
		SDL_GL_SwapWindow(window); // render back buffer to the screen
	}
	void clean()
	{
		std::cout << "Exiting..." << std::endl;
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
	static unsigned getTicks() { return static_cast<unsigned>(SDL_GetTicks() / 100); } // 1 MyGL::tick = 1/10 second
	
private:

	void prep_scene() {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//phongShader = new Shader::Program();
		//phongShader->create("src/Phong.glsl");
		//set_phong_uniforms(*phongShader);
		//cube = new Cube(0.0f, 0.0f, -1.0f);


		particles = new Particles(15, 15, 15, *cam);
	}

	void set_pointSprite_uniforms(Shader::Program &shader) {
		
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
	/*void set_skybox_uniforms(Program &shader) {
		mat4 mv = view * skybox->getModel();
		shader.set("ModelMatrix", skybox->getModel());
		shader.set("MVP", cam->get_ViewToProjection_Matrix() * mv);
	}*/
	void set_cubemap_uniforms(Shader::Program &shader) {
		shader.set("WorldCameraPosition", cam->get_position());
		shader.set("MaterialColor", vec4(0.5f, 0.5f, 0.5f, 1.0f));
		shader.set("ReflectFactor", 0.85f);
	}
	void set_computeBlackHole_Uniforms(Shader::Program &shader) {
	    
	}

	void initializeFramework_createWindow(const char* title, int x, int y, int w, int h, int fullscreen)
	{
		//auto t1 = std::chrono::high_resolution_clock::now();
		if (SDL_Init(SDL_INIT_VIDEO) > -1) {// initialize SDL for its Video (opengl uses this) & Event features
			
			if (!IMG_Init(IMG_INIT_PNG)) // SDL_image for texture loading
				std::cout << IMG_GetError() << "\n"; 

			window = SDL_CreateWindow(title, x, y, w, h, fullscreen);
			context = SDL_GL_CreateContext(window);
			keyStates = SDL_GetKeyboardState(NULL);
			linkGLAPI();
			submitDebugCallbackFunction();
			
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			//testGLEW();
			//showExtensions();
			//setMusic("03 sleep.mp3");
			
			running = true;
			std::cout << "SDL_GL_Initialized\n";
		}
		else {
			std::cout << SDL_GetError();
			exit(EXIT_FAILURE);
		}
		//auto t2 = std::chrono::high_resolution_clock::now();
		//std::cout << "initializeFramework_createWindow: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << " us\n";
	}
	void specifyGL()
	{ // specify the version of openGL associated with current GPU driver, as well as other context attributes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);//a core profile allows for forward-compatibility
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);//request debug functionality in this opengl context
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // MultiSample-Anti-Aliasing for less pixelated edges
	}
	void linkGLAPI()
	{// use GL-extensions-wrangler library to dynamically link gl functionality
		specifyGL();
		glewExperimental = GL_TRUE; //"glewExperimental": global bool decides whether extensions from pre-release or experimental
		glewInit();                 // drivers are exposed (as long as the functions they contain have valid entry points)
	}
	void testGLEW() {
		GLuint vertexBuffer; glGenBuffers(1, &vertexBuffer);
		if (vertexBuffer == 1) {
			std::cout << "GLEW successful\n\n";
			glDeleteBuffers(1, &vertexBuffer);
		}
	}
	void showExtensions()
	{ // list opengl extensions based on current GPU driver version
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
};