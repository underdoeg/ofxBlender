#ifndef MESH_H
#define MESH_H

#include "Object.h"

namespace ofx {
namespace blender {

enum Shading{
	FLAT,
	SMOOTH
};

class Mesh: public ofx::blender::Object {
public:
	Mesh();
	~Mesh();

	void customDraw();

	string meshName;
	ofMesh mesh;
	Shading shading;
};

}
}

#endif // MESH_H
