#include "Mesh.h"

namespace ofx {
namespace blender {

Mesh::Mesh() {
	type = MESH;
	curMaterial = NULL;
	isTwoSided = true;
	boundsMin.set(std::numeric_limits<float>::max());
	boundsMax.set(std::numeric_limits<float>::min());
	hasTransparency = false;
}

Mesh::~Mesh() {
}

void Mesh::customDraw() {
	if(isTwoSided) {
		glDisable(GL_CULL_FACE);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	} else {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	}
	
	//ofDrawBox(0, 0, 0, 1);
	
	for(Part& part: parts) {
		if(part.hasTriangles)
			part.draw();
	}
}

void Mesh::drawNormals(float length) {
	bool lightOn = ofGetLightingEnabled();
	if(lightOn)
		ofDisableLighting();

	ofPushStyle();
	transformGL();

	for(Part& part: parts) {
		if(part.hasTriangles) {
			ofSetColor(100, 255, 100);
			part.primitive.drawNormals(length);
			ofSetColor(255, 100, 100);
			part.primitive.drawNormals(length, true);
		}
	}
	restoreTransformGL();
	ofPopStyle();

	if(lightOn)
		ofEnableLighting();
}

void Mesh::addTriangle(Triangle triangle) {
	/*
	if(!curPart)
		updatePart();

	curPart->hasTriangles = true;

	//TODO: optimize this. Currently because of the UVs every triangle is sent separately
	ofMesh* mesh = curPart->primitive.getMeshPtr();
	unsigned int curIndex = mesh->getNumVertices();
	mesh->addVertex(vertices[a]);
	mesh->addVertex(vertices[b]);
	mesh->addVertex(vertices[c]);
	mesh->addNormal(normals[a]);
	mesh->addNormal(normals[b]);
	mesh->addNormal(normals[c]);
	mesh->addTriangle(curIndex, curIndex+1, curIndex+2);
	triangles.push_back(Triangle(a, b, c));
	*/

	triangle.material = curMaterial;
	triangle.shading = curShading;
	//cout << triangle.a << endl;
	triangles.push_back(triangle);
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
	if(boundsMax.y < pos.y)
		boundsMax.y = pos.y;
	if(boundsMax.z < pos.z)
		boundsMax.z = pos.z;

	vertices.push_back(pos);
	normals.push_back(norm);
}

void Mesh::addMesh(ofMesh& mesh) {
	std::vector<ofVec3f>& verts = mesh.getVertices();
	std::vector<ofVec3f>& normals = mesh.getNormals();
	//std::vector<ofVec3f>& colors = mesh.getColors();
	std::vector<ofVec2f>& uvs = mesh.getTexCoords();
	std::vector<ofIndexType>& indices = mesh.getIndices();

	for(unsigned int i=0; i<verts.size(); i++) {
		if(normals.size() > i)
			addVertex(verts[i], normals[i]);
		else
			addVertex(verts[i]);
	}

	ofPrimitiveMode mode = mesh.getMode();
	switch(mode) {
	case OF_PRIMITIVE_TRIANGLES:
		for(unsigned int i=2; i<indices.size(); i+=3) {
			Triangle tri(indices[i-2], indices[i-1], indices[i]);
			if(uvs.size() > tri.c) {
				TriangleUVs triUv(uvs[tri.a], uvs[tri.b], uvs[tri.c]);
				tri.uvs.push_back(triUv);
			}
			addTriangle(tri);
		}
		break;
	case OF_PRIMITIVE_TRIANGLE_STRIP:
		for(unsigned int i=2; i<indices.size(); i++) {
			Triangle tri(indices[i-2], indices[i-1], indices[i]);
			if(uvs.size() > tri.c) {
				TriangleUVs triUv(uvs[tri.a], uvs[tri.b], uvs[tri.c]);
				tri.uvs.push_back(triUv);
			}
			addTriangle(tri);
		}
		break;
	default:
		ofLogWarning(OFX_BLENDER) << "Mesh::addMesh - mesh mode not supported";
	}
}


ofVec3f& Mesh::getVertex(unsigned int pos) {
	return vertices[pos];
}

ofVec3f& Mesh::getNormal(unsigned int pos) {
	return normals[pos];
}

/*
void Mesh::setUV(unsigned int index, ofVec2f uv, bool flipY) {
	if(!curPart)
		updatePart();
	if(flipY)
		uv.y = 1-uv.y;
	//uv.x = 1-uv.x;
	//curPart->primitive.getMesh().setTexCoord(index, uv);
}
*/

Mesh::Part& Mesh::getPart(Material* mat, Shading shading, bool hasUvs) {
	for(Part& part: parts) {
		if(part.material == mat && part.shading == shading && part.hasUvs == hasUvs)
			return part;
	}
	parts.push_back(Part(mat, shading, hasUvs));
	return parts.back();
}

std::vector<Mesh::Part>& Mesh::getParts() {
	return parts;
}

void Mesh::pushMaterial(Material* material) {
	if(material == curMaterial)
		return;
	curMaterial = material;
}

void Mesh::pushShading(Shading shading) {
	if(curShading == shading)
		return;
	curShading = shading;
}

void Mesh::build() {
	clear();
	for(Triangle& tri: triangles) {
		Part& part = getPart(tri.material, tri.shading, true);
		part.hasTriangles = true;

		ofMesh& mesh = part.primitive.getMesh();

		//if(tri.uvs.size() > 0) {

		//TODO: currently each triangle is uploaded  separately, could be optimized, at least for meshes without UVs
		unsigned int curIndex = mesh.getNumVertices();

		ofVec3f v0 = vertices[tri.a];
		ofVec3f v1 = vertices[tri.b];
		ofVec3f v2 = vertices[tri.c];

		mesh.addVertex(v0);
		mesh.addVertex(v1);
		mesh.addVertex(v2);

		if(part.shading == FLAT) {
			ofVec3f n = (v1 - v2).crossed(v2-v0).normalized();
			mesh.addNormal(n);
			mesh.addNormal(n);
			mesh.addNormal(n);
		} else {
			ofVec3f n0 = normals[tri.a];
			ofVec3f n1 = normals[tri.b];
			ofVec3f n2 = normals[tri.c];
			mesh.addNormal(n0);
			mesh.addNormal(n1);
			mesh.addNormal(n2);
		}

		if(tri.uvs.size() > 0) {
			mesh.addTexCoord(tri.uvs[0].a);
			mesh.addTexCoord(tri.uvs[0].b);
			mesh.addTexCoord(tri.uvs[0].c);
		}
		mesh.addTriangle(curIndex, curIndex+1, curIndex+2);

		//add the material to the used materials
		if(part.material && std::find(materials.begin(), materials.end(), part.material)==materials.end())
			materials.push_back(part.material);
		
	}
	
	for(Material* mat: materials){
		
	}
	
	//cout << "NUMBER OF PARTS " << parts.size() << endl;
}

void Mesh::clear() {
	parts.clear();
	materials.clear();
}

void Mesh::exportUVs(int h, int w, unsigned int layer, string path) {
	//UVLayer* layer = getUVLayer(index);

	if(path == "") {
		path = name+"_uvs_"+ofToString(layer)+".png";
	}

	ofPushStyle();

	ofFbo fbo;
	fbo.allocate(w, h);
	fbo.begin();

	ofClear(0, 0, 0, 0);

	ofVec2f scale(w, h);

	for(Triangle& tri: triangles) {
		ofVec2f a,b,c;

		if(tri.uvs.size() > layer) {
			a = tri.uvs[layer].a * scale;
			b = tri.uvs[layer].b * scale;
			c = tri.uvs[layer].c * scale;
		}

		ofSetColor(0, 100);
		ofFill();
		ofTriangle(a, b, c);
		ofSetColor(10);
		ofNoFill();
		ofTriangle(a, b, c);

	}

	fbo.end();
	ofImage img;
	img.allocate(fbo.getWidth(), fbo.getHeight(), OF_IMAGE_COLOR_ALPHA);
	fbo.readToPixels(img.getPixelsRef());
	img.saveImage(path);
	img.clear();

	ofPopStyle();
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
