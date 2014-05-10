#include "Mesh.h"

namespace ofx {
namespace blender {

Mesh::Mesh() {
    type = MESH;
	shading = FLAT;
}

Mesh::~Mesh() {
}

void Mesh::customDraw() {
	if(shading == FLAT)
		glShadeModel(GL_FLAT);
    else
		glShadeModel(GL_SMOOTH);
	mesh.draw();
}

}
}
