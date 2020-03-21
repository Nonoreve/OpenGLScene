#ifndef  CONE_INC
#define  CONE_INC

#include "Geometry.h"
#include <cmath>

class Cone : public Geometry
{
    public:
        /* \brief Constructor. Base radius = 1.0. Depth = 1.0. */
        Cone(uint32_t nbLattitude, float radiusTop);
};

#endif
