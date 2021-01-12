class Particles : public Scene::Object
{
    Shader::Program render_shader, compute_shader;

    glm::ivec3 num_p;
    unsigned total_p;
    float speed, angle;
    glm::vec4 bh1, bh2;
    glm::vec3 bh1Pos, bh2Pos;
    glm::mat4 bhRot = mat4(1.0f);

    Vertex::Buffer<float> vbo_p_pos;
    Vertex::Buffer<float> vbo_p_vel;
    Vertex::Buffer<float> vbo_bh_pos;

    Vertex::Array vao_particles;
    Vertex::Array vao_blackHole;

public:

    Particles(int nx, int ny, int nz, Camera::Viewport& cam)
        : Scene::Object(0.0f, 0.0f, 0.0f, cam),
        num_p(20, 20, 20), total_p(num_p.x * num_p.y * num_p.z),
        speed(.005f), angle(0.0f),
        vao_particles(), vbo_p_pos(total_p * 4),
        vao_blackHole(), vbo_p_vel(total_p * 4, 0.0f),
        vbo_bh_pos({ 5.0f, 0.0f, 0.0f, 1.0f, -5.0f, 0.0f, 0.0f, 1.0f }),
        bh1(5, 0, 0, 1), bh2(-5, 0, 0, 1)
    {
        render_shader.create("src/Particle.glsl");
        compute_shader.create("src/Gravity.glsl");
        init_buffers();
    }

    void init_buffers() override
    {
        // Initial positions of the particles
        vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

        float dx = 2.0f / (num_p.x - 1),
                dy = 2.0f / (num_p.y - 1),
                dz = 2.0f / (num_p.z - 1);

        // center the particles at (0,0,0)
        mat4 transf = glm::translate(mat4(1.0f), vec3(-1.0f, -1.0f, -1.0f));

        int idx = 0;
        for (int i = 0; i < num_p.x; i++) {
            for (int j = 0; j < num_p.y; j++) {
                for (int k = 0; k < num_p.z; k++) {
                    p.x = dx * i;
                    p.y = dy * j;
                    p.z = dz * k;
                    p.w = 1.0f;
                    p = transf * p;
                    vbo_p_pos[idx++] = p.x;
                    vbo_p_pos[idx++] = p.y;
                    vbo_p_pos[idx++] = p.z;
                    vbo_p_pos[idx++] = p.w;
                }
            }
        }
        // bind position and velocity buffer to binding index 0 and 1 with usage hint for compute shader access.
        vbo_p_pos.bind_base(GL_SHADER_STORAGE_BUFFER, 0);
        vbo_p_pos.buffer_data(GL_DYNAMIC_DRAW);

        vbo_p_vel.bind_base(GL_SHADER_STORAGE_BUFFER, 1);
        vbo_p_vel.buffer_data(GL_DYNAMIC_COPY);

        // Set up a VAO for rendering the particles
        vao_particles.bind();
        vbo_p_pos.bind(GL_ARRAY_BUFFER);
        vbo_p_pos.add_attribute<vec4>(0);
        vao_particles.enable_attributes(vbo_p_pos);
        vao_particles.unBind();

        // Set up a buffer and a VAO for drawing the attractors (the "black holes")
        vbo_bh_pos.bind(GL_ARRAY_BUFFER);
        vbo_bh_pos.buffer_data(GL_DYNAMIC_DRAW);

        vao_blackHole.bind();
        vbo_bh_pos.bind();
        vbo_bh_pos.add_attribute<vec4>(0);
        vao_blackHole.enable_attributes(vbo_bh_pos);
        vao_blackHole.unBind();
    }

    void update(float dt) override {
        angle += speed * dt;
        if (angle > 360.0f)
            angle -= 360.0f;
        execute_compute_shader();
        set_matrices();
        set_uniforms();
    }

    void render() override
    {
        render_shader.use();
        
        // Draw the particles
        glPointSize(2.0f);
        render_shader.set("Color", vec4(0.0f, 0.3f, 0.4f, 1.0f));
        vao_particles.bind();
        glDrawArrays(GL_POINTS, 0, total_p);
        vao_particles.unBind();

        // Draw the attractors
        glPointSize(5.0f);
        render_shader.set("Color", vec4(1.0f, 1.0f, 0.0f, 1.0f));
        vao_blackHole.bind();
        glDrawArrays(GL_POINTS, 0, 2);
        vao_blackHole.unBind();
    }

private:

    void execute_compute_shader() {
        compute_shader.use();
        compute_shader.set("attractor1_pos", bh1Pos);
        compute_shader.set("attractor2_pos", bh2Pos);
        glDispatchCompute(total_p / 1000, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    void set_matrices() {
        mv = cam.get_WorldToView_Matrix() * model;
        update_bh_pos();
    }
    void set_uniforms() {
        render_shader.use();
        render_shader.set("ModelViewMatrix", mv);
        render_shader.set("MVP", cam.get_ViewToProjection_Matrix() * mv);
    }
    void update_bh_pos() {
        bhRot = glm::rotate(glm::mat4(1.0f), glm::radians(angle), vec3(0, 0, 1));
        bh1Pos = vec3(bhRot * bh1);
        bh2Pos = vec3(bhRot * bh2);
        vbo_bh_pos[0] = bh1Pos.x;
        vbo_bh_pos[1] = bh1Pos.y;
        vbo_bh_pos[2] = bh1Pos.z;
        vbo_bh_pos[3] = 1.0f;
        vbo_bh_pos[4] = bh2Pos.x;
        vbo_bh_pos[5] = bh2Pos.y;
        vbo_bh_pos[6] = bh2Pos.z;
        vbo_bh_pos[7] = 1.0f;
        vbo_bh_pos.bind();
        vbo_bh_pos.buffer_sub_data();
    }
};