#pragma once
#include "Vertex.h"
#define MAX_NUM_PARTICLES_PER_D 1500

using glm::vec3, glm::vec4, glm::mat3, glm::mat4;

class Particles
    : public Scene::Object {

    glm::ivec3 nParticles;
    int totalParticles;

    Vertex::Array vao_particle;
    Vertex::Array vao_attractor;
    Shader::Program particle_shader;
    Shader::Program compute_shader;
    
    Vertex::Buffer<float, GL_ARRAY_BUFFER> vbo_attractor;
    Vertex::Buffer<float, GL_SHADER_STORAGE_BUFFER> vbo_pos;
    Vertex::Buffer<float, GL_SHADER_STORAGE_BUFFER> vbo_vel;

    float angle, rot_speed;
    float attractor_pos_data[8];
    vec3 att1_pos{-5.0f, 0.0f, 0.0f}, att2_pos{5.0f, 0.0f, 0.0f};
    mat4 rotationMatrix;

public:
    Particles(int numX, int numY, int numZ, Camera::Viewport& cam)
        : Scene::Object(0.0f, 0.0f, 0.0f, cam),
        nParticles(numX, numY, numZ),
        totalParticles(nParticles.x* nParticles.y* nParticles.z),
        particle_shader("src/Particle.glsl"), compute_shader("src/Gravity.glsl"),
        vbo_pos(totalParticles * sizeof(float), 0u), vbo_vel(totalParticles * sizeof(float), 1u),
        vbo_attractor(8 * sizeof(float)),
        vao_attractor(), vao_particle()
    {
        assert(numX < MAX_NUM_PARTICLES_PER_D); assert(numY < MAX_NUM_PARTICLES_PER_D); assert(numZ < MAX_NUM_PARTICLES_PER_D);
        init_buffers();
        rotationMatrix = mat4(1.0f);
    }

    void init_buffers() override
    {
        using std::vector;

        // buffers for position and velocity of each particle, both will be vec4 in size, which does leave an extra float per particle.
        unsigned nBufElements = totalParticles * 4;
        unsigned pBufSize = nBufElements * sizeof(float);

        
        //vbo_pos = new Vertex::Buffer<float, GL_SHADER_STORAGE_BUFFER>(nBufElements);
        //vbo_vel = new Vertex::Buffer<float, GL_SHADER_STORAGE_BUFFER>(nBufElements);
        //vector<float> initPos;
        //vector<float> initVel(totalParticles * 4, 0.0f);

        vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
        float dx = 2.0f / (nParticles.x - 1), dy = 2.0f / (nParticles.y - 1), dz = 2.0f / (nParticles.z - 1);

        // We want to center the particles at (0,0,0)
        mat4 transf = glm::translate(mat4(1.0f), vec3(-1, -1, -1));

        int idx = 0;
        // Set initial positions for the particles
        for (int i = 0; i < nParticles.x; i++) {
            for (int j = 0; j < nParticles.y; j++) {
                for (int k = 0; k < nParticles.z; k++) {
                    p.x = dx * i;
                    p.y = dy * j;
                    p.z = dz * k;
                    p.w = 1.0f;
                    p = transf * p;
                    vbo_pos[idx++] = p.x;
                    vbo_pos[idx++] = p.y;
                    vbo_pos[idx++] = p.z;
                    vbo_pos[idx++] = p.w;
                }
            }
        }

        vbo_pos.add_attribute<vec4>(0);
        vbo_vel.add_attribute<vec4>(1);

        // Set up the VAO
        //particle_vao = new Vertex::Array();
        vao_particle.bindBuffer(vbo_pos);

        // Set up a buffer and a VAO for drawing the attractors
        set_attractor_positions();
        vao_attractor.bindBuffer(vbo_attractor);
    }

    void update(float dt) override {
        angle += rot_speed * dt;
        if (angle > 360.0f) angle -= 360.0f;
    }

    void render() override {
        // Rotate attractors
        rotationMatrix = glm::rotate(mat4(1.0f), glm::radians(angle), vec3(0.0f, 0.0f, 1.0f));
        att1_pos = vec3( rotationMatrix * vec4(att1_pos, 1.0f) );
        att2_pos = vec3( rotationMatrix * vec4(att2_pos, 1.0f) );

        // compute positions and velocities
        compute_shader.use();
        compute_shader.set("attractor1_pos", att1_pos);
        compute_shader.set("attractor2_pos", att2_pos);
        glDispatchCompute(totalParticles / 1000, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Draw scene
        particle_shader.use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model = mat4(1.0f);
        mat4 mv = cam.get_WorldToView_Matrix() * model;
        mat3 norm = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));
        particle_shader.set("MVP", cam.get_ViewToProjection_Matrix() * mv);

        // Draw particles
        glPointSize(2.0f);
        particle_shader.set("Color", vec4(0.0f, 0.3f, 0.4f, 1.0f));
        vao_particle.bind();
        glDrawArrays(GL_POINTS, 0, totalParticles);

        // Draw the attractors
        glPointSize(5.0f);
        set_attractor_positions();
        vbo_attractor.subData(8 * sizeof(float), attractor_pos_data);

        particle_shader.set("Color", vec4(1.0f, 1.0f, 0.0f, 1.0f));
        vao_attractor.bind();
        glDrawArrays(GL_POINTS, 0, 2);
    }

private:
    
    void set_attractor_positions() {
        attractor_pos_data[0] = att1_pos.x;
        attractor_pos_data[1] = att1_pos.y;
        attractor_pos_data[2] = att1_pos.z;
        attractor_pos_data[3] = 1.0f;
        attractor_pos_data[4] = att2_pos.x;
        attractor_pos_data[5] = att2_pos.y;
        attractor_pos_data[6] = att2_pos.z;
        attractor_pos_data[7] = 1.0f;
    }

};