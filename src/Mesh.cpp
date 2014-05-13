#include "Mesh.h"

namespace ofx {
namespace blender {

Mesh::Mesh() {
	type = MESH;
	curPart = NULL;
}

Mesh::~Mesh() {
}

void Mesh::customDraw() {
	for(Part& part: parts) {
		part.draw();
	}
}

void Mesh::addTriangle(unsigned int a, unsigned int b, unsigned int c) {
	if(!curPart)
		updatePart();

	curPart->mesh.addTriangle(a, b, c);
}

void Mesh::addVertex(ofVec3f pos, ofVec3f norm) {
	vertices.push_back(pos);
	normals.push_back(norm);
}

Mesh::Part* Mesh::getPart(Material* mat, Shading shading) {
	for(Part& part: parts) {
		if(part.material == mat && part.shading == shading)
			return &part;
	}
	parts.push_back(Part(mat, shading));

	//TODO: could be optimized, not all parts need all vertices
	parts.back().mesh.addVertices(vertices);
	parts.back().mesh.addNormals(normals);

	return &parts.back();
}

void Mesh::pushMaterial(Material* material) {
	if(material == curMaterial)
		return;
	curMaterial = material;
	updatePart();
}

void Mesh::pushShading(Shading shading) {
	if(curShading == shading)
		return;
	curShading = shading;
	updatePart();
}

void Mesh::updatePart() {
	curPart = getPart(curMaterial, curShading);
}

void Mesh::build() {
}

void Mesh::clear() {

}

//////////////////////////////// DRAWING
void Mesh::Part::draw() {
	if(shading == FLAT)
		glShadeModel(GL_FLAT);
	else
		glShadeModel(GL_SMOOTH);
	mesh.draw();
}

}
}
