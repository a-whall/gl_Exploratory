#pragma once
#include <iostream>
#include <SDL_opengl.h>

// debug callback code to be supplied to OpenGL
static void GLAPIENTRY myDebugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* msg, const void* param)
{
	const char* sourceStr;
	switch (source) {
	 case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "WindowSys";      break;
	 case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "App";            break;
	 case GL_DEBUG_SOURCE_API:             sourceStr = "OpenGL";         break;
	 case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "ShaderCompiler"; break;
	 case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "3rdParty";       break;
	 case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other";          break;
	 default: sourceStr = "Unknown";
	}
	const char* typeStr;
	switch (type) {
	 case GL_DEBUG_TYPE_ERROR:               typeStr = "Error";          break;
	 case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated";     break;
	 case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeStr = "Undefined";      break;
	 case GL_DEBUG_TYPE_PORTABILITY:         typeStr = "Portability";    break;
	 case GL_DEBUG_TYPE_PERFORMANCE:         typeStr = "Performance";    break;
	 case GL_DEBUG_TYPE_MARKER:              typeStr = "Marker";         break;
	 case GL_DEBUG_TYPE_PUSH_GROUP:          typeStr = "PushGrp";        break;
	 case GL_DEBUG_TYPE_POP_GROUP:           typeStr = "PopGrp";         break;
	 case GL_DEBUG_TYPE_OTHER:               typeStr = "Other";          break;
	 default: typeStr = "Unknown";
	}
	const char* sevStr;
	switch (severity) {
	 case GL_DEBUG_SEVERITY_HIGH:             sevStr = "HIGH";           break;
	 case GL_DEBUG_SEVERITY_MEDIUM:           sevStr = "MED";            break;
	 case GL_DEBUG_SEVERITY_LOW:              sevStr = "LOW";            break;
	 case GL_DEBUG_SEVERITY_NOTIFICATION:     sevStr = "NOTIFY";         break;
	 default: sevStr = "UNK";
	}
	std::cerr << sourceStr << ":" << typeStr << "[" << sevStr << "]" << "(" << id << "): " << msg << std::endl;
}

// initialize a built-in OpenGL callback function defined in debugCallback()
static void submitDebugCallbackFunction() {
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);            // assure error message gets printed on crash
	glDebugMessageCallback(myDebugCallback, nullptr); // supply callback function
	glDebugMessageControl(GL_DONT_CARE,               // source of debug msgs to enable or disable
		                  GL_DONT_CARE,               // type of debug msgs to enable or disable
		                  GL_DONT_CARE,               // severity of debug msgs to enable or disable
		                  0,                          // length of array ids
		                  NULL,                       // address of the array of msg ids to enable or disable
		                  GL_TRUE);                   // bool for whether the msgs in the array should be enabled or disabled
}

template<typename... Args>
static void abort_MyGL_App(Args&&... args) {
	(std::cerr << ... << args) << '\n'; // fold expr c++17
	exit(EXIT_FAILURE);
}