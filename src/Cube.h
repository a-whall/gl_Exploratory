#pragma once

namespace Cube_Geom {
    // instanced data
}

class Cube: public Scene::Object
{
    float side = 1.0f;

    Vertex::Array vao;
    Vertex::Buffer<float> vbo;
    Vertex::Index ebo;
    
public:

    Cube(float x, float y, float z, Camera::Viewport& cam, Shader::Program& shader)
        : Scene::Object(x, y, z, cam, &shader), vao(), vbo(), ebo() {
        init_buffers();
    }
    
    void init_buffers() override {
        GLfloat side2 = side / 2.0f;
        vbo = { // listed per vertex : pos, normal, texCoords, (vec3) (vec3) (vec2)
            // Front
           -side2, -side2,  side2,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
            side2, -side2,  side2,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
            side2,  side2,  side2,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
           -side2,  side2,  side2,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
            // Right
            side2, -side2,  side2,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
            side2, -side2, -side2,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
            side2,  side2, -side2,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
            side2,  side2,  side2,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
            // Back
           -side2, -side2, -side2,    0.0f, 0.0f,-1.0f,    0.0f, 0.0f,
           -side2,  side2, -side2,    0.0f, 0.0f,-1.0f,    1.0f, 0.0f,
            side2,  side2, -side2,    0.0f, 0.0f,-1.0f,    1.0f, 1.0f,
            side2, -side2, -side2,    0.0f, 0.0f,-1.0f,    0.0f, 1.0f,
            // Left
           -side2, -side2,  side2,   -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
           -side2,  side2,  side2,   -1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
           -side2,  side2, -side2,   -1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
           -side2, -side2, -side2,   -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
            // Bottom
           -side2, -side2,  side2,    0.0f,-1.0f, 0.0f,    0.0f, 0.0f,
           -side2, -side2, -side2,    0.0f,-1.0f, 0.0f,    1.0f, 0.0f,
            side2, -side2, -side2,    0.0f,-1.0f, 0.0f,    1.0f, 1.0f,
            side2, -side2,  side2,    0.0f,-1.0f, 0.0f,    0.0f, 1.0f,
            // Top
           -side2,  side2,  side2,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
            side2,  side2,  side2,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
            side2,  side2, -side2,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
           -side2,  side2, -side2,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        };

        ebo = {
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23
        };

        nVerts = ebo.getNumElements();

        vao.bindBuffers(ebo, vbo, true, true, false);
    }

    void update(float t) override {
        set_matrices();
        set_uniforms();
    }

    void render() override {
        shader->use();
        vao.bind();
        glDrawElements(GL_TRIANGLES, nVerts, GL_UNSIGNED_INT, 0);
    }

private:
    void rotate() {
        model = glm::rotate(model, radians(0.3f), vec3(0.0f, 1.0f, 0.0f));
    }

    void set_uniforms() {
        shader->use();
        shader->set("ModelViewMatrix", mv);
        shader->set("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
        shader->set("MVP", cam.get_ViewToProjection_Matrix() * mv);
    }

    void set_matrices() {
        rotate();
        mv = cam.get_WorldToView_Matrix() * model;
    }
};