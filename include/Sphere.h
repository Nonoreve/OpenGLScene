#ifndef  SPHERE_INC
#define  SPHERE_INC

#include "Geometry.h"
#include <cmath>
#include <glm/glm.hpp>

class Sphere : public Geometry
{
    public:
        /* \brief Constructor
         * \param nbLatitude the number of lattitude for this sphere
         * \param nbLongitude the number of longitude for this sphere */
        Sphere(uint32_t nbLatitude, uint32_t nbLongitude);
};

#endif
