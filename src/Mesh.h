#ifndef MESH_H
#define MESH_H

#include "Object.h"

namespace ofx {
namespace blender {

class Mesh: public ofx::blender::Object {
public:
	Mesh();
	~Mesh();

	void customDraw();

	string meshName;
	ofMesh mesh;
};

}
}

#endif // MESH_H
