#include "Cone.h"
#include "logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

Cone::Cone(uint32_t nbLattitude, float topRadius) : Geometry()
{
    float radius = 0.5;
    m_nbVertices = nbLattitude * 6;
    m_vertices   = (float*)malloc(sizeof(float)*3*6*nbLattitude);
    m_normals    = (float*)malloc(sizeof(float)*3*6*nbLattitude);
    m_uvs        = (float*)malloc(sizeof(float)*2*6*nbLattitude);

	for(uint32_t i=0; i < nbLattitude; i++)
	{
		double pos[] = {radius*cos(i*2*M_PI/nbLattitude),        radius*sin(i*2*M_PI/nbLattitude), -1.0/2,
					    radius*cos((i+1)*2*M_PI/nbLattitude),    radius*sin((i+1)*2*M_PI/nbLattitude), -1.0/2,
					    topRadius*cos((i+1)*2*M_PI/nbLattitude), topRadius*sin((i+1)*2*M_PI/nbLattitude), 1.0/2,

					    radius*cos(i*2*M_PI/nbLattitude),        radius*sin(i*2*M_PI/nbLattitude), -1.0/2,
					    topRadius*cos((i+1)*2*M_PI/nbLattitude), topRadius*sin((i+1)*2*M_PI/nbLattitude), 1.0/2,
					    topRadius*cos(i*2*M_PI/nbLattitude),     topRadius*sin(i*2*M_PI/nbLattitude), 1.0/2
					   };

		double uvPos[] = {
                         i/(double)nbLattitude,   0.0,
					     (i+1)/(double)nbLattitude, 0.0,
					     (i+1)/(double)nbLattitude, 1.0,

					     i/(double)nbLattitude,     0.0,
					     (i+1)/(double)nbLattitude, 1.0,
					     i/(double)nbLattitude,    1.0
					     };

		for(uint32_t j=0; j < 18; j++)
			m_vertices[18*i+j] = pos[j];

        for(uint32_t j = 0; j < 12; j++)
            m_uvs[12*i+j] = uvPos[j];

        float angle        = atan2(1.0-topRadius, 1.0);
        glm::vec3 normalI  = glm::rotate(glm::mat4(1.0f), (float)(i*2*M_PI/nbLattitude), glm::vec3(0.0, 0.0, 1.0))           * glm::vec4(cos(angle), 0.0, sin(angle), 1.0f);
        glm::vec3 normalI2 = glm::rotate(glm::mat4(1.0f), (float)((i+1.0f)*2.0f*M_PI/nbLattitude), glm::vec3(0.0, 0.0, 1.0)) * glm::vec4(cos(angle), 0.0, sin(angle), 1.0f);

        for(uint32_t j = 0; j < 3; j++)
        {
            m_normals[18*i+0+j]  = normalI[j];
            m_normals[18*i+3+j]  = normalI2[j];
            m_normals[18*i+6+j]  = normalI2[j];
            m_normals[18*i+9+j]  = normalI[j];
            m_normals[18*i+12+j] = normalI2[j];
            m_normals[18*i+15+j] = normalI[j];
        }
	}
}
