#pragma once
#include "Scene.h"


class Skybox : public Scene::Object
{
    float sideLength;

    Vertex::MeshArray vao;
    Vertex::Buffer<float> vbo;
    Vertex::Index ebo;

public:
    Skybox(float side_l, Camera::Viewport& cam, Shader::Program& shader)
        : Scene::Object(0, 0, 0, cam, &shader) { sideLength = side_l; }
    
    void init_buffers() override {
        float side2 = sideLength * 0.5f;
        vbo = {
            // Front
           -side2, -side2,  side2,
            side2, -side2,  side2,
            side2,  side2,  side2,
           -side2,  side2,  side2,
            // Right
            side2, -side2,  side2,
            side2, -side2, -side2,
            side2,  side2, -side2,
            side2,  side2,  side2,
            // Back
           -side2, -side2, -side2,
           -side2,  side2, -side2,
            side2,  side2, -side2,
            side2, -side2, -side2,
            // Left
           -side2, -side2,  side2,
           -side2,  side2,  side2,
           -side2,  side2, -side2,
           -side2, -side2, -side2,
            // Bottom
           -side2, -side2,  side2,
           -side2, -side2, -side2,
            side2, -side2, -side2,
            side2, -side2,  side2,
            // Top
           -side2,  side2,  side2,
            side2,  side2,  side2,
            side2,  side2, -side2,
           -side2,  side2, -side2
        };
        ebo = {
            0, 2, 1, 0, 3, 2,
            4, 6, 5, 4, 7, 6,
            8, 10, 9, 8, 11, 10,
            12, 14, 13, 12, 15, 14,
            16, 18, 17, 16, 19, 18,
            20, 22, 21, 20, 23, 22
        };
        vao.bindBuffers(&ebo, &vbo, false, false, false);
    }
    void update(float timeStep) override {
    
    }
    void render() override {
        // TODO
    }
private:

    void set_matrices() {
        mv = cam.get_WorldToView_Matrix() * model;
        shader->set("ModelMatrix", model);
        shader->set("MVP", cam.get_ViewToProjection_Matrix() * mv);
    }

};