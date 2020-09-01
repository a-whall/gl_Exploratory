#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm.hpp>
#include "Scene.h"

namespace Vertex
{
	using glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4;
	using std::vector, std::initializer_list;

	struct Attribute
	{
		unsigned count;
		unsigned type;
		bool normalized;
		unsigned offset;
		int index;
		bool instanced; // implicit bool to int used as attrib divisor. false = per vertex, true = per instance
	};

	struct DataLayout
	{
		vector<Attribute> attrib_vec;
		unsigned nBytes = 0;
		template <typename T> void push(unsigned, int, bool = false) { static_assert(false); }
		template<> inline void push<float>(unsigned count, int attribIndex, bool isInstanced) {
			attrib_vec.push_back({ count, GL_FLOAT, false, nBytes, attribIndex, isInstanced });//nBytes given as offset
			nBytes += count * 4;// sizeof(float)                                              before getting updated
		}
		template<> inline void push<unsigned>(unsigned count, int attribIndex, bool isInstanced) {
			attrib_vec.push_back({ count, GL_UNSIGNED_INT, false, nBytes, attribIndex, isInstanced });
			nBytes += count * 4;// sizeof(unsigned)
		}
		template<> inline void push<unsigned char>(unsigned count, int attribIndex, bool isInstanced) {
			attrib_vec.push_back({ count, GL_UNSIGNED_BYTE, false, nBytes++, attribIndex, isInstanced });
		}
	};

	template<typename T, GLenum BindingTarget = GL_ARRAY_BUFFER>
	class Buffer
	{
	protected:
		DataLayout layout;
		vector<T> data_vec;

	public:

		unsigned handle;

		Buffer() : data_vec(0), layout() {
			gl_genDynamicBuffer();
		}
		Buffer(unsigned size, T data[], GLenum usageHint) {
			gl_genBuffer<T>(size, data, usageHint);//basic buffer = array buffer
		}
		Buffer(GLenum bindingTarget, unsigned size, T data[], GLenum usageHint) {
			gl_genBuffer<T>(size, data, usageHint);
		}
		Buffer(GLenum bindingTarget, unsigned size, float data[], GLenum usageHint, unsigned bindingIndex) {
			gl_genBufferBase<T>(size, data, usageHint, bindingIndex);//for mapping buffers to a binding target index
		}
		~Buffer() {
			glDeleteBuffers(1, &handle);
		}

		virtual void bind() const { glBindBuffer(GL_ARRAY_BUFFER, handle); }

		template<typename T> void add_attribute(int index, bool = false) { static_assert(false); }
		template<> inline void add_attribute<float>(int i, bool perInst) { layout.push<float>(1, i, perInst); }
		template<> inline void add_attribute<vec2>(int i, bool perInst) { layout.push<float>(2, i, perInst); }
		template<> inline void add_attribute<vec3>(int i, bool perInst) { layout.push<float>(3, i, perInst); }
		template<> inline void add_attribute<vec4>(int i, bool perInst) { layout.push<float>(4, i, perInst); }
		template<> inline void add_attribute<mat3>(int i, bool perInst) { layout.push<float>(4, i, perInst); } //TODO
		template<> inline void add_attribute<mat4>(int i, bool perInst) { layout.push<float>(4, i, perInst); } //TODO

		void subData(unsigned size, float data[]) {
			bind();
			glBufferSubData(BindingTarget, 0, size, data);
		}

		void enable_attributes() {
			bind();
			for (Attribute a : layout.attrib_vec) {
				glEnableVertexAttribArray(a.index);
				glVertexAttribPointer(a.index, a.count, a.type, a.normalized, layout.nBytes, (void*)a.offset);
				glVertexAttribDivisor(a.index, a.instanced);
			}
		}

		int getNumElements() {
			return data_vec.size();
		}

		void operator=(initializer_list<T> data) {
			data_vec.insert(data_vec.end(), data);
			gl_genBuffer(data_vec.size() * sizeof(T), data_vec.data(), GL_STATIC_DRAW); //not uber important but dynamic usage hint should be implemented
		}//need alternate method of calling genBuffer for when algorithmically generating data

	private:

		void gl_genDynamicBuffer() {
			glGenBuffers(1, &handle);
		}

		template <typename T>
		void gl_genBuffer(unsigned size, T data[], GLenum usageHint) {
			glGenBuffers(1, &handle);
			glBindBuffer(BindingTarget, handle);
			glBufferData(BindingTarget, size, data, usageHint);
		}
		
		template <typename T>
		void gl_genBufferBase( unsigned size, T data[], GLenum usageHint, unsigned bindingIndex) {
			glGenBuffers(1, &handle);
			glBindBufferBase(BindingTarget, bindingIndex, handle);
			glBufferData(BindingTarget, size, data, usageHint);
		}
	};

	struct Index : public Buffer<unsigned>
	{
		using Buffer::operator=;
		Index() : Index(0u, nullptr, GL_STATIC_DRAW) {}
		Index(unsigned size, GLuint data[], GLenum usageHint)
			: Buffer<unsigned>(GL_ELEMENT_ARRAY_BUFFER, size, data, usageHint) {}
		void bind() const override { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle); }
	};

	class Array
	{
	protected:
		GLuint handle;
	public:
		Array() { glGenVertexArrays(1, &handle); }
		~Array() { glDeleteVertexArrays(1, &handle); }

		void bind() const { 
			if (Scene::currentlyBoundVaoHandle == handle)
				return;
			glBindVertexArray(handle);
		}
		
		template<typename T> void bindBuffer(Buffer<T> &vbo) {
			bind();
			vbo.enable_attributes();
		}

		template<typename T> void bindBuffers(Index& ebo, Buffer<T>& vbo, bool hasNorm, bool hasTC, bool hasTan) {
			bind();
			ebo.bind();
			vbo.bind();
			vbo.add_attribute<vec3>(0);
			if (hasNorm)
				vbo.add_attribute<vec3>(1);
			if (hasTC)
				vbo.add_attribute<vec2>(2);
			if (hasTan)
				vbo.add_attribute<vec3>(3);

			vbo.enable_attributes();
		}
	};
}