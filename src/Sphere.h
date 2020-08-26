#pragma once
#include "Geometry.h"

class Sphere : public Scene::Object
{
	float radius = 1.0f; // unit sphere
public:

	Sphere(float x, float y, float z, Camera::Viewport& cam, Shader::Program& shader) : Scene::Object(x, y, z, cam, &shader)
	{
		init_buffers();
		model = glm::mat4(1.0f); // identity matrix - leave the object coordinates at world origin.
		model = glm::translate(model, vec3(1.0f, 1.0f, -1.0f));
		model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	}
	void init_buffers() override {
		GLuint nSlices = 500, nStacks = 500;
		std::cout << "sphere()\n";
		int verts = (nSlices + 1) * (nStacks + 1);
		int elements = (nSlices * 2 * (nStacks - 1)) * 3;

		// data: vertex position, normal, and texture coordinates
		std::vector<GLfloat> data(3 * verts + 3 * verts + 2 * verts);
		std::vector<GLuint> el(elements);

		// Generate positions, normals, texture coordinates in a single data buffer
		GLfloat dTheta = glm::two_pi<float>() / nSlices; // unit of theta; horizontal angle
		GLfloat dPhi = glm::pi<float>() / nStacks;       // unit of phi;   vertical angle
		GLfloat nx, ny, nz, s, t, theta, phi;
		GLuint idx = 0;
		for (GLuint i = 0; i <= nSlices; i++) {     // for each horizontal slice
			theta = i * dTheta;                     // increment horizontal angle
			s = (GLfloat)i / nSlices;             // compute tex coord x  
			for (GLuint j = 0; j <= nStacks; j++) { // for each vertical stack
				phi = j * dPhi;                     // increment vertical angle

				nx = sinf(phi) * cosf(theta);       // calculate normal direction vector
				ny = sinf(phi) * sinf(theta);
				nz = cosf(phi);

				data[idx] = radius * nx;            // get model coordinates
				data[idx + 1] = radius * ny;
				data[idx + 2] = radius * nz;

				data[idx + 3] = nx;                 // store surface normal
				data[idx + 4] = ny;
				data[idx + 5] = nz;

				data[idx + 6] = s;                // generated tex coords
				data[idx + 7] = t = (GLfloat)j / nStacks;// calculate tex coord y

				idx += 8;                           // next vertex
			}
		}
		// Generate the element list
		idx = 0;
		for (GLuint i = 0; i < nSlices; i++) {
			GLuint stackStart = i * (nStacks + 1);
			GLuint nextStackStart = (i + 1) * (nStacks + 1);
			for (GLuint j = 0; j < nStacks; j++) {
				if (j == 0) {
					el[idx] = stackStart;
					el[idx + 1] = stackStart + 1;
					el[idx + 2] = nextStackStart + 1;
					idx += 3;
				}
				else if (j == nStacks - 1) {
					el[idx] = stackStart + j;
					el[idx + 1] = stackStart + j + 1;
					el[idx + 2] = nextStackStart + j;
					idx += 3;
				}
				else {
					el[idx] = stackStart + j;
					el[idx + 1] = stackStart + j + 1;
					el[idx + 2] = nextStackStart + j + 1;
					el[idx + 3] = nextStackStart + j;
					el[idx + 4] = stackStart + j;
					el[idx + 5] = nextStackStart + j + 1;
					idx += 6;
				}
			}
		}
		Geom::makeTriangleMeshBuffers(&el, &data, true, true, false);
		nVerts = el.size();
	}
	void update(float timeStep) override {
		set_uniforms();
		set_matrices();
	}
	
	void render() {
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