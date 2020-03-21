#include "Geometry.h"
#include <cstring>

Geometry::Geometry(){}

Geometry::Geometry(Geometry&& geom)
{
    m_vertices = geom.m_vertices;
    m_normals  = geom.m_normals;
    m_uvs      = geom.m_uvs;
    m_nbVertices = geom.m_nbVertices;

    geom.m_vertices = geom.m_normals = geom.m_uvs = NULL;
}

Geometry::Geometry(const Geometry& geom)
{
    *this = geom;
}

Geometry& Geometry::operator=(const Geometry& geom)
{
    if(this == &geom)
        return *this;

    if(geom.m_vertices != NULL)
    {
        m_vertices = (float*)malloc(sizeof(float) * 3 * geom.m_nbVertices);
        memcpy(m_vertices, geom.m_vertices, sizeof(float) * 3 * geom.m_nbVertices);
    }

    if (geom.m_normals != NULL)
    {
        m_normals = (float*)malloc(sizeof(float) * 3 * geom.m_nbVertices);
        memcpy(m_normals, geom.m_normals, sizeof(float) * 3 * geom.m_nbVertices);
    }

    if (geom.m_uvs != NULL)
    {
        m_uvs = (float*)malloc(sizeof(float) * 2 * geom.m_nbVertices);
        memcpy(m_uvs, geom.m_uvs, sizeof(float) * 2 * geom.m_nbVertices);
    }
    m_nbVertices = geom.m_nbVertices;

    return *this;
}

Geometry::~Geometry()
{
    if(m_vertices)
        free(m_vertices);
    if(m_normals)
        free(m_normals);
    if(m_uvs)
        free(m_uvs);
}
