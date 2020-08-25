#pragma once
#include "Geometry.h"


class Skybox: public Scene::Object
{
    float size;
public:
    Skybox(float sideLength) : Scene::Object(0, 0, 0) { size = sideLength; }
    
    void init_buffers() override {
        float side2 = size * 0.5f;
        std::vector<GLfloat> data = {
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
        std::vector<GLuint> el = {
            0,2,1,0,3,2,
            4,6,5,4,7,6,
            8,10,9,8,11,10,
            12,14,13,12,15,14,
            16,18,17,16,19,18,
            20,22,21,20,23,22
        };
        Geom::makeTriangleMeshBuffers(&el, &data, false, false, false);
    }
    void set_uniforms(Camera::Viewport& cam, Shader::Program& shader) override {
    
    }
    void set_matrices(Camera::Viewport &cam, Shader::Program &shader) override {
        mat4 mv = cam.get_WorldToView_Matrix() * model;
        shader.set("ModelMatrix", model);
        shader.set("MVP", cam.get_ViewToProjection_Matrix() * mv);
    }
    void render() override {
        // TODO
    }
};