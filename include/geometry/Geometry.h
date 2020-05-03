#ifndef  GEOMETRY_INC
#define  GEOMETRY_INC

#include <stdlib.h>
#include <stdint.h>
#include "buffer/complexvertexbuffer.h"
#include "buffer/vertexbufferlayout.h"

#define COL_CHANNELS 3 // components to define one color (rgb = 3, rgba = 4, ...)
#define VP_TRIANGLE 3 // Vertices per triangle

/* \brief Represent a geometry*/
class Geometry {
	public:
		/* \brief The constructor.*/
		Geometry();

		/* \brief Move constructor*/
		Geometry(Geometry&& geom);

		/* \brief Copy constructor*/
		Geometry(const Geometry& copy);

		/* \brief Copy assignment*/
		Geometry& operator=(const Geometry& geom);

		/* \brief Destructor. Destroy the data */
		virtual ~Geometry();

		/* Build a vertex buffer containing the geometry data */
		virtual ComplexVertexBuffer bufferFactory();
		virtual VertexBufferLayout bufferLayoutFactory();

		/* \brief Get the vertices data of the geometry
		 * \return const array on the vertices data. Use getNbVertices to get how many vertices the array contains (size(array) == 3*nbVertices) */
		const float* getVertices() const {
			return m_vertices;
		}

		/* \brief Get the normals data of the geometry
		 * \return const array on the normals data. Use getNbVertices to get how many vertices the array contains (size(array) == 3*nbVertices) */
		const float* getNormals() const {
			return m_normals;
		}

		/* \brief Get the UV mapping data of the geometry
		 * \return const array on the UV mapping data. Use getNbVertices to get how many vertices the array contains (size(array) == 2*nbVertices) */
		const float* getUVs() const {
			return m_uvs;
		}

		/* \brief Get how many vertices this geometry contains
		 * \return the number of vertices this geometry contains*/
		uint32_t getNbVertices() const {
			return m_nbVertices;
		}

	protected:
		uint32_t m_nbVertices = 0;
		float*   m_vertices   = NULL;
		float*   m_normals    = NULL;
		float*   m_uvs        = NULL;
		unsigned int m_DimPositions = 3;
		unsigned int m_DimNormals = 3;
		unsigned int m_DimUVs = 2;
};

#endif
