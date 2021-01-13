class Plane : public Scene::Object {

	float nUnits, nGridLines;
	unsigned nElements;

	Vertex::Array vao;
	Vertex::Buffer<float> vbo;
	Vertex::Index ebo;

public:

	Plane(float numUnits, Camera::Viewport& cam)
		: Scene::Object(0, 0, 0, cam),
		nUnits(numUnits), nGridLines(numUnits + 1),
		vao(), vbo(3 * 4 * nUnits), ebo(4 * nGridLines)
	{
		shader = new Shader::Program("src/GraphLines.glsl");
		init_buffers();
	}

	void init_buffers() override {
		gen_buffers_for_grid();
	}

	void update(float t) override {
		set_uniforms();
		set_matrices();
	}

	void render() override {
		shader->use();
		vao.bind();
		glDrawElements(GL_LINES, nVerts, GL_UNSIGNED_INT, 0);
		vao.unBind();
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
		float start = -(nUnits / 2);
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
		float dx = 1.0f; /// this value assumes the scale of the grid is 1:1
		float start = -(nUnits / 2);
		int idx = 0;
		for (float i = 0; i < nGridLines; i++)
		{ // positions incr in the x direction, with z constant
			float pos_dx = start + i * dx;
			// from
			vbo[idx++] = pos_dx;// x
			vbo[idx++] = 0;     // y
			vbo[idx++] = start; // z
			// to
			vbo[idx++] = pos_dx;// x
			vbo[idx++] = 0;     // y
			vbo[idx++] = -start;// z
		}
		for (float i = 1; i < nGridLines - 1; i++)
		{ // positions incr in the z direction, with x constant
			float pos_dx = start + i * dx;
			//from
			vbo[idx++] = start; // x
			vbo[idx++] = 0;     // y 
			vbo[idx++] = pos_dx;// z
			// to
			vbo[idx++] = -start;// x
			vbo[idx++] = 0;     // y
			vbo[idx++] = pos_dx;// z
		}

		idx = 0;
		for (unsigned i = 0; i < 2 * nGridLines; i++)
			ebo[idx++] = i;
		ebo[idx++] = 0;
		ebo[idx++] = 2 * nGridLines - 2;
		for (unsigned i = 2 * nGridLines; i < 4 * nGridLines - 4; i++)
			ebo[idx++] = i;
		ebo[idx++] = 1;
		ebo[idx++] = 2 * nGridLines - 1;

		ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
		ebo.buffer_data();
		vbo.bind();
		vbo.buffer_data();

		nVerts = ebo.getNumElements();
		vao.bind_buffers(ebo, vbo, false, false, false);
	}
};