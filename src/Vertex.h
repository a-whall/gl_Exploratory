#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm.hpp>

namespace Vertex {
	using glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4;
	using std::vector;

	struct Attribute
	{
		unsigned count;
		unsigned type;
		bool normalized;
		unsigned offset;
		int index;
		bool instanced; // implicit bool to int used as attrib divisor. false = per vertex, true = per instance
	};

	struct Data
	{
		vector<Attribute> attribs;
		unsigned nBytes = 0;
		template <typename T> void push(unsigned, int, bool = false) { static_assert(false); }
		template<> inline void push<float>(unsigned count, int attribIndex, bool isInstanced) {
			attribs.push_back({ count, GL_FLOAT, false, nBytes, attribIndex, isInstanced });//nBytes given as offset
			nBytes += count * 4;// sizeof(float)                                              before getting updated
		}
		template<> inline void push<unsigned>(unsigned count, int attribIndex, bool isInstanced) {
			attribs.push_back({ count, GL_UNSIGNED_INT, false, nBytes, attribIndex, isInstanced });
			nBytes += count * 4;// sizeof(unsigned)
		}
		template<> inline void push<unsigned char>(unsigned count, int attribIndex, bool isInstanced) {
			attribs.push_back({ count, GL_UNSIGNED_BYTE, false, nBytes++, attribIndex, isInstanced });
		}
	};

	template<typename T>
	class Buffer
	{
	protected:
		Data* format;

	public:

		unsigned handle;

		Buffer(unsigned size, T data[], GLenum usageHint) : format{ new Data() } {
			gl_genBuffer<T>(GL_ARRAY_BUFFER, size, data, usageHint);//basic buffer = array buffer
		}
		Buffer(GLenum bindingTarget, unsigned size, T data[], GLenum usageHint) : format{ new Data() } {
			gl_genBuffer<T>(bindingTarget, size, data, usageHint);
		}
		Buffer(GLenum bindingTarget, unsigned size, float data[], GLenum usageHint, unsigned bindingIndex) : format{ new Data() } {
			gl_genBufferBase<T>(bindingTarget, size, data, usageHint, bindingIndex);//for mapping buffers to a binding target index
		}
		~Buffer() {
			glDeleteBuffers(1, &handle);
			delete format;
		}

		virtual void bind() const { glBindBuffer(GL_ARRAY_BUFFER, handle); }

		template<typename T> void add_attribute(int index, bool = false) { static_assert(false); }
		template<> inline void add_attribute<float>(int i, bool perInst) { format->push<float>(1, i, perInst); }
		template<> inline void add_attribute<vec2>(int i, bool perInst) { format->push<float>(2, i, perInst); }
		template<> inline void add_attribute<vec3>(int i, bool perInst) { format->push<float>(3, i, perInst); }
		template<> inline void add_attribute<vec4>(int i, bool perInst) { format->push<float>(4, i, perInst); }
		template<> inline void add_attribute<mat3>(int i, bool perInst) { format->push<float>(4, i, perInst); } //TODO
		template<> inline void add_attribute<mat4>(int i, bool perInst) { format->push<float>(4, i, perInst); } //TODO

		void subData(GLenum bufferType, unsigned size, float data[]) {
			bind();
			glBufferSubData(bufferType, 0, size, data);
		}

		// move enable_attributes() to Vertex::Array since (as per web bookmark "VAOs and binding") these are the calls which adjust a VAOs state.
		// this function should never have been encapsulated here since it is entirely to do with the currently bound VAO, and a buffer can
		// be used in other VAOs
		void enable_attributes() { // maybe rename to bind_buffer_enable_attributes()
			bind();
			for (Attribute a : format->attribs) {
				glEnableVertexAttribArray(a.index);
				glVertexAttribPointer(a.index, a.count, a.type, a.normalized, format->nBytes, (void*)a.offset);
				glVertexAttribDivisor(a.index, a.instanced);
			}
		}

	private:

		template <typename T>
		void gl_genBuffer(GLenum bufferType, unsigned size, T data[], GLenum usageHint) {
			glGenBuffers(1, &handle);
			glBindBuffer(bufferType, handle);
			glBufferData(bufferType, size, data, usageHint);
		}
		template <typename T>
		void gl_genBufferBase(GLenum bufferType, unsigned size, T data[], GLenum usageHint, unsigned bindingIndex) {
			glGenBuffers(1, &handle);
			glBindBufferBase(bufferType, bindingIndex, handle);
			glBufferData(bufferType, size, data, usageHint);
		}
	};

	struct Index : Buffer<unsigned>
	{
		Index(unsigned size, GLuint data[], GLenum usageType)
			: Buffer<unsigned>(GL_ELEMENT_ARRAY_BUFFER, size, data, usageType) {}
		void bind() const override { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle); }
	};

	class Array
	{
	protected:
		GLuint handle;
	public:
		Array() { glGenVertexArrays(1, &handle); }
		~Array() { glDeleteVertexArrays(1, &handle); }

		void bind() const { glBindVertexArray(handle); }
		
		template<typename T> void bindBuffer(Buffer<T> &vbo) const {
			bind();
			vbo.enable_attributes();
		}
	};
}