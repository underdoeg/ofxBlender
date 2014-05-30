#ifndef MESH_H
#define MESH_H

#include "Object.h"
#include "Material.h"

namespace ofx {
namespace blender {

enum Shading {
    FLAT,
    SMOOTH
};

class Mesh: public ofx::blender::Object {
public:
	Mesh();
	~Mesh();

	class Triangle {
	public:
		Triangle(unsigned int a_, unsigned int b_, unsigned int c_) {
			a = a_;
			b = b_;
			c = c_;
		}
		unsigned int a;
		unsigned int b;
		unsigned int c;
	};

	class Vertex {
	public:
		Vertex(ofVec3f p, ofVec3f n=ofVec3f()) {
			pos = p;
			norm = n;
		}
		ofVec3f pos;
		ofVec3f norm;
	};
	
	class UVLayer {
	public:
		UVLayer(string n) {
			name = n;
		}
		string name;
		std::vector<ofVec2f> uvs;
	};

	void pushMaterial(Material* material);
	void pushShading(Shading shading);

	void addVertex(ofVec3f pos, ofVec3f norm=ofVec3f());
	ofVec3f getVertex(unsigned int pos);
	void addTriangle(unsigned int a, unsigned int b, unsigned int c);

	void setUV(unsigned int index, ofVec2f uv, bool flipY=true);
	
	UVLayer* getUVLayer(string name);

	void clear();

	void build();

	void customDraw();
	void drawNormals(float length=1);

	string meshName;

	bool isTwoSided;

	ofVec3f boundsMin;
	ofVec3f boundsMax;
	
	std::vector<UVLayer> uvLayers;
private:
	class Part {
	public:
		Part(Material* mat, Shading shade) {
			material = mat;
			shading = shade;
			hasTriangles = false;
			primitive.setUseVbo(true);
		}

		void draw();

		of3dPrimitive primitive;
		Material* material;
		Shading shading;
		std::vector<Triangle> polys;
		bool hasTriangles;
	};

	Part* getPart(Material* mat, Shading shading);
	void updatePart();

	Material* curMaterial;
	Shading curShading;
	Part* curPart;
	std::vector<ofVec3f> vertices;
	std::vector<ofVec3f> normals;
	std::vector<ofVec2f> uvs;
	std::vector<Part> parts;
};

}
}

#endif // MESH_H
