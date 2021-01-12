class PointSprites : public Scene::Object
{
	const int numSprites = 100;
	Vertex::Array vao;
	Vertex::Buffer<float> vbo_pos;

public:

	PointSprites(const char* texFileName, Camera::Viewport& cam, Shader::Program& shader)
		: Scene::Object(0.0f, 0.0f, 0.0f, cam, &shader), vao(), vbo_pos(numSprites * 3)
	{
		GLuint texID = Texture::load(texFileName);
		init_buffers();
		set_texture_uniforms();
	}

	void init_buffers() override
	{
		nVerts = numSprites * 3;
		srand((unsigned)SDL_GetTicks());
		for (int i = 0; i < numSprites; i++) {
			glm::vec3 p(((float)rand() / RAND_MAX * 2.0f) - 1.0f,
				((float)rand() / RAND_MAX * 2.0f) - 1.0f,
				((float)rand() / RAND_MAX * 2.0f) - 1.0f);
			vbo_pos[i * 3] = p.x;
			vbo_pos[i * 3 + 1] = p.y;
			vbo_pos[i * 3 + 2] = p.z;
		}
		vbo_pos.add_attribute<vec3>(0);
		vbo_pos.bind();
		vbo_pos.buffer_data();

		vao.enable_attributes(vbo_pos);
	}

	void update(float timestep) override {
		set_matrices();
	}
	
	void render() override {
		shader->use();
		vao.bind();
		glDrawArrays(GL_POINTS, 0, numSprites);
		glFinish();
		vao.unBind();
	}

private:

	void set_texture_uniforms() {
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