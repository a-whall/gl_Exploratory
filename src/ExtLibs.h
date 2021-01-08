#pragma once

// python c-api
#define PY_SSIZE_T_CLEAN
#include <python.h>

// std lib
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <sstream>
#include <unordered_map>

// glew / glm
#include <GL/glew.h>
#include <gtc/type_ptr.hpp>

// sdl framework
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>