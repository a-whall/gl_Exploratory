#pragma once
#include "vertex.h"



class Sphere : public Scene::Object
{
	
	const int nSlices = 300, nStacks = 300;
	const int numUniqVerts = (nSlices + 1) * (nStacks + 1),
		      numElements = (nSlices * 2 * (nStacks - 1)) * 3;

	float radius = 1.0f; // unit sphere
	Vertex::MeshArray vao;
	Vertex::Buffer<float> vbo;
	Vertex::Index ebo;

public:

	Sphere(float x, float y, float z, Camera::Viewport& cam, Shader::Program& shader)
		: Scene::Object(x, y, z, cam, &shader), vbo(3 * numUniqVerts + 3 * numUniqVerts + 2 * numUniqVerts), ebo(numElements)
	{
		std::cout << "sphere()\n";
		init_buffers();
		model = glm::mat4(1.0f); // identity matrix - leave the object coordinates at world origin.
		model = glm::translate(model, vec3(1.0f, 1.0f, -1.0f));
		model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	}
	void init_buffers() override {
		// Generate positions, normals, texture coordinates in a single data buffer
		float dTheta = glm::two_pi<float>() / nSlices; // unit of theta; horizontal angle
		float dPhi = glm::pi<float>() / nStacks;       // unit of phi;   vertical angle
		float nx, ny, nz, tcx, tcy, theta, phi;
		int idx = 0;
		for (float i = 0.0f; i <= nSlices; i++) {      // for each horizontal slice
			theta = i * dTheta;                        // increment horizontal angle
			tcx = i / nSlices;                         // compute tex coord x  
			for (float j = 0.0f; j <= nStacks; j++) {  // for each vertical stack
				phi = j * dPhi;                        // increment vertical angle

				nx = sinf(phi) * cosf(theta);          // calculate normal direction vector
				ny = sinf(phi) * sinf(theta);
				nz = cosf(phi);

				vbo[idx] = radius * nx;                // get model coordinates
				vbo[idx + 1] = radius * ny;
				vbo[idx + 2] = radius * nz;

				vbo[idx + 3] = nx;                     // store surface normal
				vbo[idx + 4] = ny;
				vbo[idx + 5] = nz;

				vbo[idx + 6] = tcx;                   // generated tex coords
				vbo[idx + 7] = tcy = j / nStacks;     // calculate tex coord y

				idx += 8;                             // next vertex
			}
		}
		// Generate the element list
		idx = 0;
		for (int i = 0; i < nSlices; i++) {
			unsigned stackStart = i * (nStacks + 1);
			unsigned nextStackStart = (i + 1) * (nStacks + 1);
			for (int j = 0; j < nStacks; j++) {
				if (j == 0) {
					ebo[idx] = stackStart;
					ebo[idx + 1] = stackStart + 1;
					ebo[idx + 2] = nextStackStart + 1;
					idx += 3;
				}
				else if (j == nStacks - 1) {
					ebo[idx] = stackStart + j;
					ebo[idx + 1] = stackStart + j + 1;
					ebo[idx + 2] = nextStackStart + j;
					idx += 3;
				}
				else {
					ebo[idx] = stackStart + j;
					ebo[idx + 1] = stackStart + j + 1;
					ebo[idx + 2] = nextStackStart + j + 1;
					ebo[idx + 3] = nextStackStart + j;
					ebo[idx + 4] = stackStart + j;
					ebo[idx + 5] = nextStackStart + j + 1;
					idx += 6;
				}
			}
		}
		//Geom::makeTriangleMeshBuffers(&el, &data, true, true, false);
		nVerts = ebo.getNumElements();
	}
	void update(float timeStep) override {
		set_uniforms();
		set_matrices();
	}
	
	void render() {
		vao.bind();
		glDrawElements(GL_TRIANGLES, nVerts, GL_UNSIGNED_INT, nullptr);
	}

private:

	void rotate() {
		model = glm::rotate(model, radians(1.0f), vec3(0.0f, 1.0f, 0.0f));
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