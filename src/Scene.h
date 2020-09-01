#pragma once
#include "GL/glew.h"
#include <vector>
#include <cmath>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/constants.hpp>
#include <gtx/string_cast.hpp>
#include "Vertex.h"

using glm::vec3, glm::vec4, glm::mat3, glm::mat4;
using glm::lookAt, glm::perspective, glm::radians;

namespace Scene { 
	using glm::mat4, glm::vec3;
    using std::vector;

	unsigned currentlyBoundVaoHandle = 0;

	class Object { // needs a better name
	protected:

		int nVerts; // must be assigned by child
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