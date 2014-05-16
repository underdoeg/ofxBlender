#include "Mesh.h"

namespace ofx {
namespace blender {

Mesh::Mesh() {
	type = MESH;
	curPart = NULL;
	curMaterial = NULL;
}

Mesh::~Mesh() {
}

void Mesh::customDraw() {
	for(Part& part: parts) {
		if(part.hasTriangles)
			part.draw();
	}
}

void Mesh::addTriangle(unsigned int a, unsigned int b, unsigned int c) {
	if(!curPart)
		updatePart();

	curPart->hasTriangles = true;
	curPart->mesh.addTriangle(a, b, c);
}

void Mesh::addVertex(ofVec3f pos, ofVec3f norm) {
	vertices.push_back(pos);
	normals.push_back(norm);
	uvs.push_back(ofVec2f());
}

ofVec3f Mesh::getVertex(unsigned int pos) {
	return vertices[pos];
}

void Mesh::setUV(unsigned int index, ofVec2f uv) {
	if(!curPart)
		updatePart();
	curPart->mesh.setTexCoord(index, uv);
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
	parts.back().mesh.addTexCoords(uvs);

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
	if(material != NULL)
		material->begin();
	mesh.drawInstanced(OF_MESH_FILL, 1);
	ofSetColor(100, 255, 100);
	mesh.drawFaceNormals(.1);
	ofSetColor(100, 100, 255);
	mesh.drawVertNormals(.01);
	//ofDrawBox(0, 0, 0, 2);
	if(material != NULL)
		material->end();
}

}
}
