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

class Scene;

class Mesh: public ofx::blender::Object {
public:
	class TriangleUVs {
	public:
		TriangleUVs(ofVec2f a_, ofVec2f b_, ofVec2f c_) {
			a = a_;
			b = b_;
			c = c_;
		}

		ofVec2f a;
		ofVec2f b;
		ofVec2f c;
	};

	class Triangle {
	public:
		Triangle(unsigned int a_, unsigned int b_, unsigned int c_) {
			a = a_;
			b = b_;
			c = c_;
		}

		Triangle(unsigned int a_, unsigned int b_, unsigned int c_, ofVec2f uv1, ofVec2f uv2, ofVec2f uv3) {
			Triangle(a_, b_, c_);
			addUVs(uv1, uv2, uv3);
		}

		void addUVs(ofVec2f a, ofVec2f b, ofVec2f c) {
			uvs.push_back(TriangleUVs(a, b, c));
		}

		unsigned int a;
		unsigned int b;
		unsigned int c;
		Material* material;
		Shading shading;
		std::vector<TriangleUVs> uvs;
	};

	class Part {
	public:
		Part(Material* mat, Shading shade, bool hasUvs_) {
			material = mat;
			shading = shade;
			hasTriangles = false;
			hasUvs = hasUvs_;
			primitive.setUseVbo(true);
		}

		void draw();

		of3dPrimitive primitive;
		Material* material;
		Shading shading;
		std::vector<Triangle> polys;
		bool hasTriangles;
		bool hasUvs;
	};

	///////////////////////////////////////////////////////////////
	Mesh();
	~Mesh();

	void pushMaterial(Material* material);
	void pushShading(Shading shading);

	void addVertex(ofVec3f pos, ofVec3f norm=ofVec3f());
	void addTriangle(Triangle triangle);
	void addMesh(ofMesh& mesh);

	ofVec3f& getVertex(unsigned int index);
	ofVec3f& getNormal(unsigned int index);
	Triangle& getTriangle(unsigned int index);

	void exportUVs(int w=1024, int h=1024, unsigned int layer=0, string path="");

	std::vector<Part>& getParts();

	void clear();

	void build();

	void customDraw();
	void drawNormals(float length=1);

	string meshName;

	bool isTwoSided;

	ofVec3f boundsMin;
	ofVec3f boundsMax;

private:
	friend class Scene;

	Part& getPart(Material* mat, Shading shading, bool hasUvs);
	
	bool hasTransparency;
	
	Material* curMaterial;
	Shading curShading;
	std::vector<ofVec3f> vertices;
	std::vector<ofVec3f> normals;
	std::vector<Triangle> triangles;
	std::vector<Part> parts;
	std::vector<Material*> materials;
};

}
}

#endif // MESH_H
