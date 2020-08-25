#pragma once
#include <SDL.h>
#include <glm.hpp>
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"

class PointSprites : public Scene::Object
{
	int numSprites = 100;
public:
	PointSprites(const char* texFileName) : Scene::Object(0, 0, 0) {
		GLuint texID = Texture::load(texFileName);
	}
	void init_buffers() {
		nVerts = numSprites * 3;
		float* locations = new float[nVerts];
		srand((unsigned)SDL_GetTicks());
		for (int i = 0; i < numSprites; i++) {
			glm::vec3 p(((float)rand() / RAND_MAX * 2.0f) - 1.0f,
				((float)rand() / RAND_MAX * 2.0f) - 1.0f,
				((float)rand() / RAND_MAX * 2.0f) - 1.0f);
			locations[i * 3] = p.x;
			locations[i * 3 + 1] = p.y;
			locations[i * 3 + 2] = p.z;
		}
		GLuint handle;
		glGenBuffers(1, &handle);

		glBindBuffer(GL_ARRAY_BUFFER, handle);
		glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(float), locations, GL_STATIC_DRAW);

		delete[] locations;

		glBindBuffer(GL_ARRAY_BUFFER, handle);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
		glEnableVertexAttribArray(0);  // Vertex position
	}
	void set_uniforms(Camera::Viewport& cam, Shader::Program& shader) override {
		shader.use();
		shader.set("spriteTex", 0);
		shader.set("sizeOver2", 0.15f);
	}
	void set_matrices(Camera::Viewport &cam, Shader::Program &shader) override {
		glm::mat4 mv = cam.get_WorldToView_Matrix() * model;
		shader.set("mv", mv);
		shader.set("p", cam.get_ViewToProjection_Matrix());
	}
	void render() override {
		glDrawArrays(GL_POINTS, 0, numSprites);
		glFinish();
	}

	//void setModel(glm::mat4 m) { model = m; }
	//glm::mat4 getModel() { return model; }

	
};