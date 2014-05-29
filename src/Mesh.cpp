#include "Mesh.h"

namespace ofx {
namespace blender {

Mesh::Mesh() {
	type = MESH;
	curPart = NULL;
	curMaterial = NULL;
	isTwoSided = true;
	boundsMin.set(std::numeric_limits<float>::max());
	boundsMax.set(std::numeric_limits<float>::min());
}

Mesh::~Mesh() {
}

void Mesh::customDraw() {
	/*
		if(isTwoSided) {
			glDisable(GL_CULL_FACE);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		} else {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
		}
	*/
	for(Part& part: parts) {
		if(part.hasTriangles)
			part.draw();
	}
}

void Mesh::addTriangle(unsigned int a, unsigned int b, unsigned int c) {
	if(!curPart)
		updatePart();

	curPart->hasTriangles = true;
	curPart->primitive.getMesh().addTriangle(a, b, c);
}

void Mesh::addVertex(ofVec3f pos, ofVec3f norm) {

	if(boundsMin.x > pos.x)
		boundsMin.x = pos.x;
	if(boundsMin.y > pos.y)
		boundsMin.y = pos.y;
	if(boundsMin.z > pos.z)
		boundsMin.z = pos.z;

	if(boundsMax.x < pos.x)
		boundsMax = pos;
	if(boundsMax.y > pos.y)
		boundsMax.y = pos.y;
	if(boundsMax.z > pos.z)
		boundsMax.z = pos.z;

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
	curPart->primitive.getMesh().setTexCoord(index, uv);
}

Mesh::Part* Mesh::getPart(Material* mat, Shading shading) {
	for(Part& part: parts) {
		if(part.material == mat && part.shading == shading)
			return &part;
	}
	parts.push_back(Part(mat, shading));

	//TODO: could be optimized, not all parts need all vertices
	parts.back().primitive.getMesh().addVertices(vertices);
	parts.back().primitive.getMesh().addNormals(normals);
	parts.back().primitive.getMesh().addTexCoords(uvs);

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
	else
		ofSetColor(255);

	primitive.draw(OF_MESH_FILL);

	if(material != NULL)
		material->end();

	/*
	ofSetColor(0, 255, 0);
	primitive.drawNormals(.1);

	ofSetColor(0, 255, 255);
	float length = .15;
	vector<ofMeshFace> faces = primitive.getMesh().getUniqueFaces();
	for(unsigned int i=0; i<faces.size(); i++) {
		ofMeshFace& face = faces[i];

		if(face.hasNormals()) {
			ofVec3f center = (face.getVertex(0) + face.getVertex(1) + face.getVertex(2)) / 3.f;
			ofLine(center, center + face.getFaceNormal() * length);
		}
	}
	 */
}

}
}
