#pragma once
#include <vector>
#include <cmath>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/constants.hpp>
#include <gtx/string_cast.hpp>
#include "Vertex.h"
#include "Scene.h"

using glm::vec3, glm::vec4, glm::mat3, glm::mat4;
using glm::lookAt, glm::perspective, glm::radians;

namespace Geom
{
	using glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4;
	using std::vector;

	// Position data at index 0, norms at 1, texCoords 2, tangents 3. booleans specify whether the data has norms, texCoords, tangents 
	void makeTriangleMeshBuffers(vector<unsigned>* indices, vector<float>* data, bool hasNorm, bool hasTC, bool hasTan) {
		std::cout << "\nGeom::makeTriangleMeshBuffers()\n";

		Vertex::Index* elementBuffer = new Vertex::Index(indices->size() * sizeof(GLuint), indices->data(), GL_STATIC_DRAW);
		Vertex::Buffer<float>* arrayBuffer = new Vertex::Buffer<float>(data->size() * sizeof(GLfloat), data->data(), GL_STATIC_DRAW);
		
		//add_attribute<attrib_t>(attribute_index)
		arrayBuffer->add_attribute<vec3>(0); // position x,y,z
		if (hasNorm)
		    arrayBuffer->add_attribute<vec3>(1); // surface normal nx,ny,nz
		if (hasTC)
			arrayBuffer->add_attribute<vec2>(2); // tex Coord x,y
		if (hasTan)
			arrayBuffer->add_attribute<vec3>(3); // tangents tx,ty,tz

		arrayBuffer->enable_attributes();
	}
}