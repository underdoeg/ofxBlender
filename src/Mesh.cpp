#include "Mesh.h"

namespace ofx {
namespace blender {

Mesh::Mesh() {
}

Mesh::~Mesh() {
}

void Mesh::customDraw() {
	mesh.drawWireframe();
}

}
}
