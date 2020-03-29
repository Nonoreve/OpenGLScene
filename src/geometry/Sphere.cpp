#include "geometry/Sphere.h"

Sphere::Sphere(uint32_t nbLatitude, uint32_t nbLongitude)
{
    float radius = 0.5;
    //Determine position
	glm::vec3* vertexCoord = (glm::vec3*)malloc(nbLongitude*nbLatitude*sizeof(glm::vec3));
    glm::vec2* uvCoord     = (glm::vec2*)malloc(nbLongitude*nbLongitude*sizeof(glm::vec3));
	for(unsigned int i=0; i < nbLongitude; i++)
	{
		double theta = 2*M_PI/nbLongitude * i;
		for(unsigned int j=0; j < nbLatitude; j++)
		{
			double phi = M_PI/(nbLatitude-1) * j;
			double pos[] = {sin(phi)*sin(theta), cos(phi), cos(theta)*sin(phi)};
            double uvs[] = {i/(double)nbLongitude, j/(double)(nbLatitude-1)};
			for(unsigned int k=0; k < 3; k++)
				vertexCoord[i*nbLatitude + j][k] = radius*pos[k];
            for(unsigned int k=0; k < 2; k++)
                uvCoord[i*nbLatitude+j][k] = uvs[k];
		}
	}

    //Determine draw orders
	unsigned int* order = (unsigned int*)malloc(nbLongitude*(nbLatitude)*sizeof(unsigned int)*6);
	for(unsigned int i=0; i < nbLongitude; i++)
	{
		for(unsigned int j=0; j < nbLatitude-1; j++)
		{
			unsigned int o[] = {i*(nbLatitude) + j, (i+1)*(nbLatitude) + j+1, (i+1)*(nbLatitude)+j,
								i*(nbLatitude) + j, i*(nbLatitude) + j+1, (i+1)*(nbLatitude) + j+1
						        
			};

			for(unsigned int k=0; k < 6; k++)
				order[(nbLatitude-1)*i*6 + j*6 + k] = o[k]%(nbLongitude*nbLatitude);
		}
	}

    //Merge everything
    m_nbVertices = nbLongitude*(nbLatitude-1)*6;
    m_vertices = (float*)malloc(sizeof(float)*m_nbVertices*3);
    m_uvs      = (float*)malloc(sizeof(float)*m_nbVertices*2);
    m_normals  = (float*)malloc(sizeof(float)*m_nbVertices*3);
    for(uint32_t i = 0; i < nbLatitude*(nbLatitude-1)*6; i++)
    {
        uint32_t indice   = order[i];
        for(uint32_t j = 0; j < 3; j++)
        {
            m_vertices[3*i+j] = vertexCoord[indice][j];
            m_normals [3*i+j] = glm::normalize(vertexCoord[indice])[j];
        }
        for(uint32_t j = 0; j < 2; j++)
            m_uvs[2*i+j] = uvCoord[indice][j];
    }

    free(order);
    free(vertexCoord);
    free(uvCoord);
}
