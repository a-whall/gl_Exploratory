#pragma once
#include "Vertex.h"


class Line : public Scene::Object {

	glm::vec3 a, b;

public:

	Line(float a1, float a2, float a3, float b1, float b2, float b3, Camera::Viewport& cam) 
	: a(a1, a2, a3), b(b1, b2, b3), Scene::Object(0, 0, 0, cam) {}


};

class Grid : public Scene::Object {

	float width, nGridLines;
	unsigned nElements;

	Vertex::Array vao;
	Vertex::Buffer<float> vbo;
	Vertex::Index ebo;

public:

	Grid(float nUnits, Camera::Viewport& cam, Shader::Program& shader)
		: Scene::Object(0, 0, 0, cam, &shader),
		width(nUnits), nGridLines(nUnits + 1),
		vao(), vbo(3 * 4 * nGridLines), ebo(4*nGridLines)
	{
		init_buffers();
	}

	void init_buffers() override {
		gen_buffers_for_grid();
		nVerts = ebo.getNumElements();
		vao.bindBuffers(ebo, vbo, false, false, false);
	}

	void update(float t) override {
		set_uniforms();
		set_matrices();
	}

	void render() override {
		shader->use();
		vao.bind();
		glDrawElements(GL_LINES, nVerts, GL_UNSIGNED_INT, 0);
	}

private:

	void set_uniforms() {
		shader->use();
		vao.bind();
		shader->set("MVP", cam.get_ViewToProjection_Matrix() * mv);
	}

	void set_matrices() {
		mv = cam.get_WorldToView_Matrix() * model;
	}

	void gen_buffers_for_simple_square() {
		float start = -(width / 2);
		vbo = {
			-start, 0, -start,
			-start, 0, start,
			start, 0, -start,
			start, 0, start
		};
		ebo = {
			0, 1,
			1, 3,
			3, 2,
			2, 0
		};
	}

	void gen_buffers_for_grid() {
		float dx = 1.0f;
		float start = -(width / 2);
		int idx = 0;
		for (float i = 0; i < nGridLines; i++) {

			float pos_dx = start + i * dx;
			// positions incr in the x direction, with zs constant
			// from
			vbo[idx++] = pos_dx;// x
			vbo[idx++] = 0;     // y
			vbo[idx++] = start; // z
			std::cout << "( " << pos_dx << ", 0, " << start << "), ";
			// to
			vbo[idx++] = pos_dx;// x
			vbo[idx++] = 0;     // y
			vbo[idx++] = -start;// z
			std::cout << "(" << pos_dx << ", 0, " << -start << ")\n";
		}
		for (float i = 1; i < nGridLines - 1; i++) {

			float pos_dx = start + i * dx;

			vbo[idx++] = start; // x
			vbo[idx++] = 0;     // y
			vbo[idx++] = pos_dx;// z
			std::cout << "(" << start << ", 0, " << pos_dx << "), ";
			// to
			vbo[idx++] = -start;// x
			vbo[idx++] = 0;     // y
			vbo[idx++] = pos_dx;// z
			std::cout << "(" << -start << ", 0, " << pos_dx << ")\n";
		}
		idx = 0;
		for (unsigned i = 0; i < 2 * nGridLines; i++) {
			ebo[idx++] = i;
		}
		ebo[idx++] = 0;
		ebo[idx++] = 2 * nGridLines - 2;
		for (unsigned i = 2 * nGridLines; i < 4 * nGridLines - 4; i++) {
			ebo[idx++] = i;
		}
		ebo[idx++] = 1;
		ebo[idx++] = 2 * nGridLines - 1;

		// N E C E S S A R Y  FOR DYNAMIC DATA
		ebo.applyData();
		vbo.applyData();
	}
};