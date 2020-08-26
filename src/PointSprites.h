#pragma once
#include <SDL.h>
#include <glm.hpp>
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"

class PointSprites : public Scene::Object
{
	int numSprites = 100;
	Vertex::Array* sprite_vao;
	Vertex::Buffer<float>* sprite_vbo;

public:

	PointSprites(const char* texFileName, Camera::Viewport& cam, Shader::Program& shader)
		: Scene::Object(0.0f, 0.0f, 0.0f, cam, &shader) {
		GLuint texID = Texture::load(texFileName);
		init_buffers();
		set_uniforms();
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
		sprite_vbo = new Vertex::Buffer<float>(nVerts * sizeof(float), locations, GL_STATIC_DRAW);
		sprite_vbo->add_attribute<vec3>(0);

		delete[] locations;

		sprite_vao = new Vertex::Array();
		sprite_vao->bindBuffer(*sprite_vbo);
	}

	void update(float timestep) override {
		set_matrices();
	}
	
	void render() override {
		shader->use();
		sprite_vao->bind();
		glDrawArrays(GL_POINTS, 0, numSprites);
		glFinish();
	}

private:

	void set_uniforms() {
		shader->use();
		shader->set("spriteTex", 0);
		shader->set("sizeOver2", 0.15f);
	}

	void set_matrices() {
		shader->use();
		mv = cam.get_WorldToView_Matrix() * model;
		shader->set("mv", mv);
		shader->set("p", cam.get_ViewToProjection_Matrix());
	}
};