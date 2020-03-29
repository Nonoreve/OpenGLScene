#include "geometry/Circle.h"

Circle::Circle(uint32_t nbEdge) : Geometry()
{
	m_vertices = (float*)malloc(9*nbEdge*sizeof(float));
    m_uvs      = (float*)malloc(9*nbEdge*sizeof(float));
	m_normals  = (float*)malloc(6*nbEdge*sizeof(float));
    m_nbVertices = 3*nbEdge;

	for(uint32_t i=0; i < nbEdge; i++)
	{
		float pos[] = {(float)cos(i*2*M_PI/nbEdge), (float)sin(i*2*M_PI/nbEdge), 0.0f,
                       0.0f, 0.0f, 0.0f,
                       (float)cos((i+1)*2*M_PI/nbEdge), (float)sin((i+1)*2*M_PI/nbEdge), 0.0f};

		for(uint32_t j=0; j < 9; j++)
        {
			m_vertices[9*i+j] = 0.5*pos[j];
        }
        for(uint32_t j=0; j < 3; j++)
            for(uint32_t k = 0; k < 2; k++)
                m_uvs[6*i+2*j+k] = m_vertices[9*i+3*j+k]+0.5;

        for(uint32_t j=0; j < 3; j++)
        {
            float normal[] = {0.0, 0.0, 1.0};
            for(uint32_t k=0; k < 3; k++)
                m_normals[9*i+3*j+k] = normal[k];
        }
	}
}
