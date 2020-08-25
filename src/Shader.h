#pragma once
#define STRING_NOT_FOUND std::string::npos
#define NUM_SUPPORTED_SHADER_TYPES 6
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <glm.hpp>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "debug.h"
#include "Vertex.h"

namespace Shader
{
	using std::vector, std::string, std::stringstream, std::ifstream;
	using glm::vec3, glm::vec4, glm::mat3, glm::mat4;

	enum Type : int { NONE = -1, VERT = 0, TESC = 1, TESE = 2, GEOM = 3, FRAG = 4, COMP = 5 };//if this works it will be funny but also indicate that t should never be -1

	class Compiler
	{
		GLuint* programHandle;
		GLuint handles[NUM_SUPPORTED_SHADER_TYPES]{ 0, 0, 0, 0, 0, 0 };//individual shaders
		string m_path;
		string line;
	
	public:

		Compiler(GLuint* glslProgram) : programHandle{ glslProgram } {}

		void createShadersFrom(const char* filePath) {
			m_path = filePath;
			stringstream ss[NUM_SUPPORTED_SHADER_TYPES];
			parse_shader_source_code(ss);
			gl_compile_shaders(ss);
			gl_link_verifyLinkStatus();
			gl_delete_precursor_shader_objects();
			std::cout << "\nShaders for program " << *programHandle << " compiled and linked.\n";
		}

	private:

		void parse_shader_source_code(stringstream ss[]) {
			ifstream stream{ m_path };
			Type t = NONE;
			while (getline(stream, line)) {
				if (line_contains("#shader")) t = find_shader_t();
				else if (line_contains("#include")) temp_ifstream_lines_to(ss[t]);
				else if (line_contains("#end")) break;
				else { assert(t != NONE); ss[t] << line << "\n"; }
			}
		}
		void gl_compile_shaders(stringstream ss[]) {
			for (GLuint i = VERT; i < NUM_SUPPORTED_SHADER_TYPES; i++)
				if (source_code_exists_in(ss[i]))
					gl_compile_attach((Type)i, &handles[i], ss[i].str().data());
		}
		void gl_delete_precursor_shader_objects() {
			for (GLuint i = VERT; i < NUM_SUPPORTED_SHADER_TYPES; i++)
				if (handles[i] > 0)
					gl_free_mem_of(handles[i]);
		}
		void gl_free_mem_of(GLuint shaderHandle) {
			glDetachShader(*programHandle, shaderHandle);
			glDeleteShader(shaderHandle);
		}
		void gl_compile_attach(Type myTypeEnum, GLuint* shaderHandle, const char* sourceCode) {
			gl_createShader(*shaderHandle, glEnum(myTypeEnum));
			glShaderSource(*shaderHandle, 1, &sourceCode, nullptr);
			glCompileShader(*shaderHandle);
			checkCompileStatus(*shaderHandle);
			glAttachShader(*programHandle, *shaderHandle);
		}
		void gl_link_verifyLinkStatus() { // create an executable for all attached shaders
			glLinkProgram(*programHandle);
			if (getProgramiv(GL_LINK_STATUS) == GL_FALSE)
				abort_MyGL_App("\nFailed to link shader program\n", getProgramErrorLog());
		}
		void temp_ifstream_lines_to(stringstream& ss) {
			size_t start = line.find("<") + 1, last = line.find(">");
			assert(start != 0 && last != -1);//indicate syntax error
			ifstream tifs{ "src/" + substrFileName(start, last) + ".glsl" };
			while (getline(tifs, line))
				ss << line << "\n";
		}
		void gl_createShader(GLuint& shaderHandle, GLenum shaderType) {
			if (!(shaderHandle = glCreateShader(shaderType)))
				abort_MyGL_App("Error creating shader program object.\n");
		}
		void checkCompileStatus(GLuint shader) {
			if (getiv(shader, GL_COMPILE_STATUS) == GL_FALSE)
				abort_MyGL_App("Shader compilation failed.\n", getShaderErrorLog(shader));
		}
		string getShaderErrorLog(GLuint shaderHandle) {
			int logLen = getiv(shaderHandle, GL_INFO_LOG_LENGTH);
			assert(logLen > 0);
			return getShaderLog(shaderHandle, logLen);
		}
		string getProgramErrorLog() {
			int logLen = getProgramiv(GL_INFO_LOG_LENGTH);
			assert(logLen > 0);
			return getProgramLog(logLen);
		}
		int getiv(GLuint shaderHandle, GLenum status_type) {
			int requestedObjParam;
			glGetShaderiv(shaderHandle, status_type, &requestedObjParam);
			return requestedObjParam;
		}
		int getProgramiv(GLenum status_type) {
			int requestedObjParam;
			glGetProgramiv(*programHandle, status_type, &requestedObjParam);
			return requestedObjParam;
		}
		string getShaderLog(GLuint handle, int logLength) {
			std::string log(logLength, ' ');  GLsizei written;
			glGetShaderInfoLog(handle, logLength, &written, &log[0]);
			return log;
		}
		string getProgramLog(int logLength) {
			std::string log(logLength, ' ');  GLsizei written;
			glGetProgramInfoLog(*programHandle, logLength, &written, &log[0]);
			return log;
		}
		string substrFileName(unsigned s, unsigned l) {
			return line.substr(s, l - s);
		}
		Type find_shader_t() {
			if (line_contains("vertex"))       return VERT;
			else if (line_contains("tes control"))  return TESC;
			else if (line_contains("tes evaluate")) return TESE;
			else if (line_contains("geometry"))     return GEOM;
			else if (line_contains("fragment"))     return FRAG;
			else if (line_contains("compute"))      return COMP;

			else return NONE; // perhaps throw a fatal compiler error and abort
		}
		bool line_contains(string phrase) {
			return (line.find(phrase) != STRING_NOT_FOUND);
		}
		bool source_code_exists_in(stringstream& ss) {
			return ss.tellp() != std::streampos(0);
		}
		GLenum glEnum(Type val) {
			assert(val != NONE);
			switch (val) {
			case VERT: return GL_VERTEX_SHADER;
			case TESC: return GL_TESS_CONTROL_SHADER;
			case TESE: return GL_TESS_EVALUATION_SHADER;
			case GEOM: return GL_GEOMETRY_SHADER;
			case FRAG: return GL_FRAGMENT_SHADER;
			case COMP: return GL_COMPUTE_SHADER;
			}
		}
    };


	class Debugger
	{
		GLuint* programHandle;

	public:

		Debugger(GLuint* glslProgram) : programHandle{ glslProgram } {}

		void printActiveVertexAttributes() {
			GLint numAttribs;
			glGetProgramInterfaceiv(*programHandle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);
			GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
			std::cout << "\nActive Vertex Attributes of linked shader program (id=" << *programHandle << "):\n";
			std::cout << "<index> <name> <type>\n";
			for (int i = 0; i < numAttribs; i++) {
				GLint results[3];
				glGetProgramResourceiv(*programHandle, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);
				GLint nameBufSize = results[0] + 1;
				char* name = new char[nameBufSize];
				glGetProgramResourceName(*programHandle, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
				std::cout << results[2] << " " << name << " (" << getTypeString(results[1]) << ")\n";
				delete[] name;
			}
			std::cout << "\n\n";
		}
		void printActiveUniformVariables() {
			GLint numUniforms = 0;
			glGetProgramInterfaceiv(*programHandle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
			GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };
			std::cout << "Active Uniforms of linked shader program (id=" << *programHandle << "):\n";
			for (int i = 0; i < numUniforms; ++i) {
				GLint results[4];
				glGetProgramResourceiv(*programHandle, GL_UNIFORM, i, 4, properties, 4, NULL, results);
				//if (results[3] != -1) continue; // skip uniform block variables
				GLint nameBuffSize = results[0] + 1;
				char* name = new char[nameBuffSize];
				glGetProgramResourceName(*programHandle, GL_UNIFORM, i, nameBuffSize, NULL, name);
				std::cout << results[2] << " " << name << " (" << getTypeString(results[1]) << ")\n";
				delete[] name;
			}
			std::cout << "\n\n";
		}
		void printActiveUniformBlocks() {
			GLint numBlocks = 0;
			glGetProgramInterfaceiv(*programHandle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks); // search program obj for uniform
			if (numBlocks == 0) {
				std::cout << "No active Uniform Blocks\n\n";
				return;
			}
			GLenum blockProps[] = { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH }; // block properties
			GLenum blockIndex[] = { GL_ACTIVE_VARIABLES };                     // block indices
			GLenum props[] = { GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX };      // properties of interest

			// for each u_block, print: "<blockname>:\n" 
			for (int block = 0; block < numBlocks; ++block) {
				GLint blockInfo[2];
				glGetProgramResourceiv(*programHandle, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, NULL, blockInfo);
				GLint numUnifs = blockInfo[0];
				char* blockName = new char[blockInfo[1] + 1];
				glGetProgramResourceName(*programHandle, GL_UNIFORM_BLOCK, block, blockInfo[1] + 1, NULL, blockName);
				std::cout << "Active Uniform Block : \" " << blockName << " \" \n";
				delete[] blockName;
				// for each u_index print: "<index> <GLtype>\n"
				GLint* unifIndices = new GLint[numUnifs];
				glGetProgramResourceiv(*programHandle, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnifs, NULL, unifIndices);
				for (int unif = 0; unif < numUnifs; ++unif) {
					GLint uniIndex = unifIndices[unif];
					GLint results[3];
					glGetProgramResourceiv(*programHandle, GL_UNIFORM, uniIndex, 3, props, 3, NULL, results);
					GLint nameBufSize = results[0] + 1;
					char* name = new char[nameBufSize];
					glGetProgramResourceName(*programHandle, GL_UNIFORM, uniIndex, nameBufSize, NULL, name);
					std::cout << name << " (" << getTypeString(results[1]) << ")\n";
					delete[] name;
				}
				delete[] unifIndices;
			}
			std::cout << "\n";
		}
		const char* getTypeString(GLenum type)
		{// Converts GLenum to string for debugging. note/ There are more, less common, types than are covered by this function.
			switch (type) {
			case GL_FLOAT: return "float";
			case GL_FLOAT_VEC2: return "vec2";
			case GL_FLOAT_VEC3: return "vec3";
			case GL_FLOAT_VEC4: return "vec4";
			case GL_DOUBLE: return "double";
			case GL_INT: return "int";
			case GL_UNSIGNED_INT: return "unsigned int";
			case GL_BOOL: return "bool";
			case GL_FLOAT_MAT2: return "mat2";
			case GL_FLOAT_MAT3: return "mat3";
			case GL_FLOAT_MAT4: return "mat4";
			default: return "?";
			}
		}
	};


	class Program
	{
		std::unordered_map<const char*, int> uniformLocationCache;
		Shader::Compiler* compiler;
		Shader::Debugger* debugger;

	public:
		GLuint handle;

		Program() : handle(glCreateProgram()),
			compiler(new Compiler(&handle)),
			debugger(new Debugger(&handle)) {
		}
		~Program() {
			glDeleteProgram(handle);
			delete compiler;
			delete debugger;
		}

		void use() const { glUseProgram(handle); }

		void create(const char* fileName) { compiler->createShadersFrom(fileName); }

		void set(const char* name, int i) { glUniform1i(getUniformLocation(name), i); }
		void set(const char* name, bool b) { glUniform1i(getUniformLocation(name), (int)b); }
		void set(const char* name, float f) { glUniform1f(getUniformLocation(name), f); }
		void set(const char* name, float x, float y, float z) { glUniform3f(getUniformLocation(name), x, y, z); }
		void set(const char* name, float x, float y, float z, float w) { glUniform4f(getUniformLocation(name), x, y, z, w); }
		void set(const char* name, vec3 v) { glUniform3f(getUniformLocation(name), v.x, v.y, v.z); }
		void set(const char* name, vec4 v) { glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w); }
		void set(const char* name, const mat3& m) { glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &m[0][0]); }
		void set(const char* name, const mat4& matrix) { glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]); }

		int getUniformLocation(const char* name) {
			if (uniformLocationCache.find(name) != uniformLocationCache.end())
				return uniformLocationCache[name];
			int location = glGetUniformLocation(handle, name);
			if (location == -1)
				std::cout << "\nWarning : '" << name << "' is not an active uniform,"
				          << " it either doesnt exist or opengl optimized it out of shader " << handle <<".\n";
			else
				uniformLocationCache[name] = location;
			return location;
		}

		void showVertexAttributes() { debugger->printActiveVertexAttributes(); }
		void showUniforms() { debugger->printActiveUniformVariables(); }
		void showUniformBlocks() { debugger->printActiveUniformBlocks(); }
	};
}