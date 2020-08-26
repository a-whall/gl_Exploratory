#pragma once
#include "GL/glew.h"

namespace Scene { 
	using glm::mat4, glm::vec3;
    using std::vector;

	class Object { // needs a better name
	protected:

		int nVerts; // must be assigned from child, init_buffers
		mat4 model; // scene obj coordinate space
		mat4 mv;    // "model to view" matrix storage

		Camera::Viewport& cam;
		Shader::Program* shader;

	public:

		Object(float x, float y, float z, Camera::Viewport& c, Shader::Program* s = nullptr)
			: model(1.0f), cam(c), shader(s) {
			model = glm::translate(model, vec3(x, y, z));
		}

		virtual void init_buffers() = 0;
		virtual void update( float) = 0;
		virtual void render() = 0;

		virtual mat4 getModel() final { return model; };
		virtual void setModel(mat4 m) final { model = m; };
	};
}