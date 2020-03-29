#ifndef  CYLINDER_INC
#define  CYLINDER_INC

#include "geometry/Geometry.h"
#include <cmath>

/* \brief Represents a Cylinder WITHOUT THE TOP AND BOTTOM CIRCLES ! */
class Cylinder : public Geometry
{
    public:
        /* \brief Constructor
         * \param nbLattitude the number of lattitude. Minimum : 3 */
        Cylinder(uint32_t nbLattitude);
};

#endif
