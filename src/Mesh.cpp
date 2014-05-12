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

void Mesh::addTriangle(unsigned int a, unsigned int b, unsigned int c)
{
}

void Mesh::addVertex(ofVec3f pos, ofVec3f norm)
{
}

Mesh::Part& Mesh::getPart(Material* mat, Shading shading)
{
}

void Mesh::pushMaterial(Material* material)
{
}

void Mesh::pushShading(Shading shading)
{
}


void Mesh::build()
{
}

}
}

