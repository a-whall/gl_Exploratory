namespace d {

}

class Function : public Scene::Object {

	float nUnits,       // units width of square range [0,x] = [0,y]
		  nEdges,       // number of lines in the grid
		  nTriangles;   // number of triangles needed to construct the grid

	Vertex::Array vao;
	Vertex::Buffer<float> vbo;
	Vertex::Index ebo;
	
	PyObject* mFunc;       // python callable
	std::string mFuncID;   // python dict key to callable
	std::string pyFormat;  // format of argument data. eg: "f,f" = 2 float args

public:

	Function(float numUnits, Camera::Viewport& cam)
		: Scene::Object(0, 0, 0, cam),
		nUnits(numUnits), nEdges(nUnits + 1), nTriangles(nUnits* nUnits * 2),
		vao(), vbo(3 * (nUnits + 1) * (nUnits + 1)), ebo(3 * nTriangles)
	{
		std::string f;
		std::cout << "Enter a function of two variables: ";
		getline(std::cin, f);
		shader = new Shader::Program("src/Function.glsl");
		cpython::pyfile_buildFunction(f.c_str());
		mFunc = cpython::pyfile_retrieveCallable("Func", "f");
		init_buffers();
	}

	void init_buffers() override {
		gen_buffers_for_function();
		nVerts = ebo.getNumElements();
		vao.bind_buffers(ebo, vbo, false, false, false);
	}

	void update(float t) override {
		set_uniforms();
		set_matrices();
	}

	void render() override {
		shader->use();
		vao.bind();
		glDrawElements(GL_TRIANGLES, nVerts, GL_UNSIGNED_INT, 0);
		vao.unBind();
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
		ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
		ebo.buffer_data();
		vbo.bind();
		vbo.buffer_data();
	}

	void set_uniforms() {
		shader->use();
		shader->set("MVP", cam.get_ViewToProjection_Matrix() * mv);
	}

	void set_matrices() {
		mv = cam.get_WorldToView_Matrix() * model;
	}
};