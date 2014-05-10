#include "Mesh.h"

namespace ofx {
namespace blender {

Mesh::Mesh() {
    type = MESH;
}

Mesh::~Mesh() {
}

void Mesh::customDraw() {
    mesh.draw();
}

}
}
