#pragma once
#include "GL/glew.h"

namespace Scene { 
	using glm::mat4, glm::vec3;
    using std::vector;

	class Object { // needs a better name
	protected:

		int nVerts; // must be assigned from child, init_buffers
		mat4 model; // scene obj 

	public:
		Object() : model(1.0f) { std::cout << "\nSceneObject::"; }
		Object(float x, float y, float z) : Object() { model = glm::translate(model, vec3(x, y, z)); }


		virtual void init_buffers() = 0;
		virtual void update(float timestep) = 0;
		virtual void set_uniforms(Camera::Viewport&, Shader::Program&) = 0;
		virtual void set_matrices(Camera::Viewport&, Shader::Program&) = 0;
		virtual void render() = 0;

		virtual mat4 getModel() final { return model; };
		virtual void setModel(mat4 m) final { model = m; };
	};
}