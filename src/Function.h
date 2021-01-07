#pragma once

class Function : public Scene::Object {

	float nUnits, nEdges, nTriangles;
	unsigned nElements;

	Vertex::Array vao;
	Vertex::Buffer<float> vbo;
	Vertex::Index ebo;

	PyObject* mFunc; // callable

public:

	Function(float numUnits, Camera::Viewport& cam, Shader::Program& shader)
		: Scene::Object(0, 0, 0, cam, &shader),
		nUnits(numUnits), nEdges(nUnits + 1), nTriangles(nUnits* nUnits * 2),
		vao(), vbo(3 * (nUnits + 1) * (nUnits + 1)), ebo(3 * nTriangles)
	{
		std::string f;
		std::cout << "Enter a function of two variables: ";
		getline(std::cin, f);
		cpython::pyfile_buildFunction(f.c_str());
		mFunc = cpython::pyfile_retrieveCallable("Func", "f");
		init_buffers();
	}

	void init_buffers() override {
		gen_buffers_for_function();
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
		glDrawElements(GL_TRIANGLES, nVerts, GL_UNSIGNED_INT, 0);
	}

private:

	void gen_buffers_for_function() {
		float x, y, start;
		start = -(nUnits / 2);
		int idx = 0;
		/// vertex buffer
		for (float i = 0; i < nEdges; i++) {
			x = start + i * 1.0f;
			for (float j = 0; j < nEdges; j++) {
				y = start + j * 1.0f;

				PyObject* result = PyObject_CallFunction(mFunc, "f,f", x, y);
				cpython::checkPyErr();
				float z = (float)PyFloat_AsDouble(result);

				vbo[idx++] = x;
				vbo[idx++] = z;
				vbo[idx++] = y;

				Py_DecRef(result);
			}
		}
		/// element buffer, generated triangle strips
		idx = 0;
		for (int s = 0; s < nUnits; s++) {
			unsigned elem = s * nEdges;
			for (int i = 0; i < nUnits; i++) {
				ebo[idx++] = elem;
				ebo[idx++] = ++elem;
				elem += nUnits;
				ebo[idx++] = elem;
				ebo[idx++] = elem++;
				ebo[idx++] = elem;
				elem = elem - nEdges;
				ebo[idx++] = elem;
			}
		}
		ebo.applyData();
		vbo.applyData();
	}

	void set_uniforms() {
		shader->use();
		vao.bind();
		shader->set("MVP", cam.get_ViewToProjection_Matrix() * mv);
	}

	void set_matrices() {
		mv = cam.get_WorldToView_Matrix() * model;
	}
};