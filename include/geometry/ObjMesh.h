#ifndef  ObjMesh_INC
#define  ObjMesh_INC

#include "geometry/Geometry.h"
/*
Classe permettant d'importer un obj exporte par exemple depuis Blender
Repris du tuto cf (.cpp)
*/

class ObjMesh : public Geometry {
    public:
		ObjMesh(const char * path);
};

#endif
