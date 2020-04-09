#include "geometry/Square.h"

Square::Square() : Geometry() {
	const unsigned int VERTICES = 6;
	m_vertices = (float*)malloc(sizeof(float) * 2 * VERTICES);
	m_normals  = (float*)malloc(sizeof(float) * 3 * VERTICES);
	m_uvs      = (float*)malloc(sizeof(float) * 2 * VERTICES);

	float positions[] = {
		-0.50f, -0.50f,
		 0.50f, -0.50f,
		 0.50f,  0.50f,
		 0.50f,  0.50f,
		-0.50f,  0.50f,
		-0.50f, -0.50f
	};


	float texturesPositions[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	};

	float normals[] = {
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f
	};

	for(uint32_t i = 0; i < 2 * VERTICES; i++) {
		m_vertices[i] = positions[i];
		m_uvs[i] = texturesPositions[i];
	}
	for(uint32_t i = 0; i < 3 * VERTICES; i++)
		m_normals[i] = normals[i];
	m_nbVertices = VERTICES;
}
