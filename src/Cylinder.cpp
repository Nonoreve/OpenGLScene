#include "Cylinder.h"
#include "logger.h"

Cylinder::Cylinder(uint32_t nbLattitude) : Geometry()
{
    float radius = 0.5;
    m_nbVertices = nbLattitude * 6;
    m_vertices   = (float*)malloc(sizeof(float)*3*6*nbLattitude);
    m_normals    = (float*)malloc(sizeof(float)*3*6*nbLattitude);

    m_uvs        = (float*)malloc(sizeof(float)*2*6*nbLattitude);

	for(uint32_t i=0; i < nbLattitude; i++)
	{
		double pos[] = {radius*cos(i*2*M_PI/nbLattitude),     radius*sin(i*2*M_PI/nbLattitude), -1.0/2,
					    radius*cos((i+1)*2*M_PI/nbLattitude), radius*sin((i+1)*2*M_PI/nbLattitude), -1.0/2,
					    radius*cos((i+1)*2*M_PI/nbLattitude), radius*sin((i+1)*2*M_PI/nbLattitude), 1.0/2,

					    radius*cos(i*2*M_PI/nbLattitude),     radius*sin(i*2*M_PI/nbLattitude), -1.0/2,
					    radius*cos((i+1)*2*M_PI/nbLattitude), radius*sin((i+1)*2*M_PI/nbLattitude), 1.0/2,
					    radius*cos(i*2*M_PI/nbLattitude),     radius*sin(i*2*M_PI/nbLattitude), 1.0/2
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

        for(uint32_t j = 0; j < 6; j++)
        {
            for(uint32_t k = 0; k < 2; k++)
                m_normals[18*i+3*j+k]  = pos[3*j+k];
            m_normals[18*i+3*j+2] = 0.0;
        }
	}
}
