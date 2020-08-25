#pragma once
#include "Geometry.h"
#define MAX_NUM_PARTICLES_PER_D 1500

using glm::vec3, glm::vec4, glm::mat3, glm::mat4;

class Particles : public Scene::Object {

    Camera::Viewport& cam;

    glm::ivec3 numParticles;
    unsigned totalParticles;

    Vertex::Buffer<float>* attractor_vbo;
    Vertex::Array* particleVAO;
    Vertex::Array* attractor_vao;
    Shader::Program* particleShader;
    Shader::Program* computeShader;
    

    float angle, rot_speed;
    GLfloat attractor_pos_data[8];
    vec3 att1_pos{-5.0f, 0.0f, 0.0f}, att2_pos{5.0f, 0.0f, 0.0f};
    mat4 rotationMatrix;

public:
    Particles(int numX, int numY, int numZ, Camera::Viewport &c) : Scene::Object(), numParticles(numX, numY, numZ), cam(c) {
        std::cout << "Particles()\n\n";
        assert(numX < MAX_NUM_PARTICLES_PER_D); assert(numY < MAX_NUM_PARTICLES_PER_D); assert(numZ < MAX_NUM_PARTICLES_PER_D);
        totalParticles = numParticles.x * numParticles.y * numParticles.z;
        particleShader = new Shader::Program();
        computeShader = new Shader::Program();
        particleShader->create("src/Particle.glsl");
        computeShader->create("src/Gravity.glsl");
        init_buffers();
        rotationMatrix = mat4(1.0f);
    }

    void init_buffers() override
    {
        using std::vector;

        // Initial positions for the particles
        vector<float> initPos;
        vector<float> initVel(totalParticles * 4, 0.0f);

        vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
        float dx = 2.0f / (numParticles.x - 1),
            dy = 2.0f / (numParticles.y - 1),
            dz = 2.0f / (numParticles.z - 1);
        // We want to center the particles at (0,0,0)
        mat4 transf = glm::translate(mat4(1.0f), vec3(-1, -1, -1));
        for (int i = 0; i < numParticles.x; i++) {
            for (int j = 0; j < numParticles.y; j++) {
                for (int k = 0; k < numParticles.z; k++) {
                    p.x = dx * i;
                    p.y = dy * j;
                    p.z = dz * k;
                    p.w = 1.0f;
                    p = transf * p;
                    initPos.push_back(p.x);
                    initPos.push_back(p.y);
                    initPos.push_back(p.z);
                    initPos.push_back(p.w);
                }
            }
        }

        // buffers for position and velocity of each particle, both will be vec4 in size, which does leave an extra float 
        unsigned particleBufferSize = totalParticles * 4 * sizeof(float);

        Vertex::Buffer<float>* posBuf = new Vertex::Buffer<float>(GL_SHADER_STORAGE_BUFFER, particleBufferSize, &initPos[0], GL_DYNAMIC_DRAW, 0);//note: extra param on the end
        posBuf->add_attribute<vec4>(0);
        Vertex::Buffer<float>* velBuf = new Vertex::Buffer<float>(GL_SHADER_STORAGE_BUFFER, particleBufferSize, &initVel[0], GL_DYNAMIC_COPY, 1);
        velBuf->add_attribute<vec4>(1);

        // Set up the VAO
        particleVAO = new Vertex::Array();
        particleVAO->bindBuffer(*posBuf);

        // Set up a buffer and a VAO for drawing the attractors (the "black holes")
        set_attractor_positions();
        attractor_vbo = new Vertex::Buffer<float>(GL_ARRAY_BUFFER, 8 * sizeof(float), attractor_pos_data, GL_DYNAMIC_DRAW);
        attractor_vao = new Vertex::Array();
        attractor_vao->bindBuffer(*attractor_vbo);
    }

    void update(float dt) override {
        angle += rot_speed * dt;
        if (angle > 360.0f) angle -= 360.0f;
    }

    void set_uniforms(Camera::Viewport& cam, Shader::Program& shader) override {}

    void set_matrices(Camera::Viewport& cam, Shader::Program& shader) override {}

    void render() override {
        // Rotate attractors
        rotationMatrix = glm::rotate(mat4(1.0f), glm::radians(angle), vec3(0.0f, 0.0f, 1.0f));
        att1_pos = vec3( rotationMatrix * vec4(att1_pos, 1.0f) );
        att2_pos = vec3( rotationMatrix * vec4(att2_pos, 1.0f) );

        // compute positions and velocities
        computeShader->use();
        computeShader->set("attractor1_pos", att1_pos);
        computeShader->set("attractor2_pos", att2_pos);
        glDispatchCompute(totalParticles / 1000, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Draw scene
        particleShader->use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model = mat4(1.0f);
        mat4 mv = cam.get_WorldToView_Matrix() * model;
        mat3 norm = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));
        particleShader->set("MVP", cam.get_ViewToProjection_Matrix() * mv);

        // Draw particles
        glPointSize(2.0f);
        particleShader->set("Color", vec4(0.0f, 0.3f, 0.4f, 1.0f));
        particleVAO->bind();
        glDrawArrays(GL_POINTS, 0, totalParticles);

        // Draw the attractors
        glPointSize(5.0f);
        set_attractor_positions();
        attractor_vbo->subData(GL_ARRAY_BUFFER, 8 * sizeof(float), attractor_pos_data);

        particleShader->set("Color", vec4(1.0f, 1.0f, 0.0f, 1.0f));
        attractor_vao->bind();
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